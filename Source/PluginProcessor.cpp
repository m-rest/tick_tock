/*
  ==============================================================================

  Copyright (c) 2015-2018 - Maximilian Rest

  Permission is granted to use this software under the terms of either:
  a) the GPL v2 (or any later version)
  b) the Affero GPL v3
  Details of these licenses can be found at: www.gnu.org/licenses
  This Software is distributed in the hope that it will be useful, but WITHOUT ANY
  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
  A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "SyncSamples.h"



//==============================================================================
AudioClockAudioProcessor::AudioClockAudioProcessor()
{
    Measure = 6;
    CreatePulses = false;
    SampleNo = 0;
    AccelTicks = false;
    SR = 1;
    PlaySample = false;
    OutputData = NULL; // crucial!
}

AudioClockAudioProcessor::~AudioClockAudioProcessor()
{
    // clean up temporary output buffer (!)
    if(OutputData != NULL)
        delete [] OutputData;
}

//==============================================================================
const String AudioClockAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int AudioClockAudioProcessor::getNumParameters()
{
    return 1;
}

float AudioClockAudioProcessor::getParameter (int index)
{
    return (float)Measure.get() / NUM_COMBO_ELEM;
}

void AudioClockAudioProcessor::setParameter (int index, float newValue)
{
    switch (index)
    {
        case PMeasure:
            Measure = roundf(newValue*(NUM_COMBO_ELEM));
            UiUpdateFlag = true;
            break;
            
        default:
            break;
    }
}

const String AudioClockAudioProcessor::getParameterName (int index)
{
    switch (index)
    {
        case PMeasure:
            return "Time Signature";
            break;
        default:
            return String();
            break;
    }
}

const String AudioClockAudioProcessor::getParameterText (int index)
{
    return String();
}

const String AudioClockAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String AudioClockAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool AudioClockAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool AudioClockAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool AudioClockAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool AudioClockAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool AudioClockAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double AudioClockAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioClockAudioProcessor::getNumPrograms()
{
    return 1;
}

int AudioClockAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioClockAudioProcessor::setCurrentProgram (int index)
{
}

const String AudioClockAudioProcessor::getProgramName (int index)
{
    return JucePlugin_Name;
}

void AudioClockAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void AudioClockAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    //get samplerate before playback starts
    SR = getSampleRate();
    
    //Create buffer for output data and set to zero
    if (OutputData != NULL)
        delete [] OutputData;
    OutputData = new float[ samplesPerBlock ]{ };
    memset( OutputData, 0, sizeof(*OutputData) * samplesPerBlock );
    
    //initialize last host ppq to something waaaay back in the past
    lastHostPpqPositionAtBeginning = (-1.f) * std::numeric_limits<double>::max();
}

void AudioClockAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

void AudioClockAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // Create Position Info object and get current position and state from the playhead
    AudioPlayHead::CurrentPositionInfo CurrentPosition;
    getPlayHead()->getCurrentPosition (CurrentPosition);
    
    // clear output buffer, just in case
    buffer.clear();
    
    // Get number of output channels
    int OutputChannels = getTotalNumOutputChannels();
    
    // Calculate sync stream only if we are playing or recording
    if ( (CurrentPosition.isPlaying) || (CurrentPosition.isRecording) )
    {
        
        DBG(String(CurrentPosition.ppqPositionOfLastBarStart)+" ppqPosofLastBarStart");
        DBG(String(CurrentPosition.ppqPosition)+" ppqPos");

        
        
        // initialize ticks per bar and anti-mute-noise phase
        uint32_t _Measure = Measure.get();
        double TicksPerBar = 12.0f*( (double)_Measure + 2.0f );
        char NoisePol = 0;
        
        // initialize tick counter towards 2nd SPP in a bar for tick accelerator
        int32_t TicksUntil2ndSPP;
        
        // save bpm, current position and calculate ppq increment per sample based on current tempo
        double bpm = CurrentPosition.bpm;
        double currentPpqPosition = CurrentPosition.ppqPosition;
        double dppqPerSample = bpm / ( 60.0f*SR );
      
        // wether the host is in cycle mode AND the playhead cycled from end marker to start marker
        bool hasLooped = CurrentPosition.isLooping && currentPpqPosition < lastHostPpqPositionAtBeginning;      
      
        // check if ppq info got updated since last block - PT12 updates only every second block for fs>48kHz.
        // if we got no new data, start this block with our own last estimate from the block before
        if (currentPpqPosition == lastHostPpqPositionAtBeginning)
        {
            currentPpqPosition = lastPluginPpqPositionAtEnd;
        }
        lastHostPpqPositionAtBeginning = currentPpqPosition;

        // resync current ppqPosition to start of current bar, so we always get the ppq position from the
        // beginning of the current bar. Needed to resync after tempo change and restart of playhead.
        // ALERT: does not work in PT according to JUCE API
        if(!hasLooped)
        {
          currentPpqPosition -= CurrentPosition.ppqPositionOfLastBarStart;
        }
        else
        {
          // in this block, the playhead went back to the start of the loop
          // resync accordingly
          currentPpqPosition = CurrentPosition.ppqLoopStart;
        }         
        
        DBG(String(currentPpqPosition)+" OUR CURRENT BAR PPQ POS");

       
        // sample loop
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            // only create ticks if transport is in the positive part of time (no ticks during pre-rolls in PT etc.)
            if (currentPpqPosition >= 0)
            {
                // the amount of ppq that elapsed since the last 96th note/24ppq tick
                double ppqSinceLastTick = fmod(currentPpqPosition, (1.0f/24.0f));
                
                // calculate the amount of ppq that elapsed since the last downbeat
                double ppqSinceLastBar = fmod(currentPpqPosition, (TicksPerBar/24.0f));
                
                // calculate elapsed ticks from current bar
                int ElapsedTicks = floor(ppqSinceLastBar*24);
                
                // pattern:
                // 1st SPP                 2nd SPP
                //       6 | 5 | 4 | 3 | 2 | 1 | 0 | -1 | -2 | -3 | -4 | -5 |
                //                               ^                        ^
                //                           accel start              accel stop
                TicksUntil2ndSPP = 6 - ElapsedTicks;
                
                // if we are not yet running, check start condition
                if ( CreatePulses == false )
                {
                    // start clock ticks if we are at the beginning of a bar
                    if ( ElapsedTicks == 0 )
                    {
                        SampleNo = 0;
                        CreatePulses = true;
                    }
                    // accelerate clock ticks if we are between 1st and 2nd SPP of a bar as 'Live' sends the 1st
                    // position as SPP via MIDI
                    else if ( TicksUntil2ndSPP == 0 )
                    {
                        SampleNo = 0;
                        CreatePulses = true;
                        AccelTicks = true;
                    }
                    // create noise if we didn't start yet to avoid being muted
                    else
                    {
                        NoisePol ^= 1;
                        
                        if (NoisePol == 1)
                        {
                            OutputData[i] =   ANTI_MUTE_NOISE_LEVEL;
                        }
                        else
                        {
                            OutputData[i] =  -ANTI_MUTE_NOISE_LEVEL;
                        }
                    }
                }
                
                // if we are running, generate ticks
                if ( CreatePulses == true )
                {
                    // if we accelerate, stop acceleration after 2nd SPP (see scheme above)
                    if ( TicksUntil2ndSPP < -5 )
                    {
                        AccelTicks = false;
                    }
                    
                    // calculate elapsed time of a tick at this sample...
                    double TickTime;
                    if ( AccelTicks == true )
                    {
                        TickTime = fmod( ppqSinceLastTick, 1.0f/48.0f );
                    }
                    else
                    {
                        TickTime = ppqSinceLastTick;
                    }
                    
                    //create trigger window of length 20*dppqPerSample to deal with glitches/roundoff
                    //errors in TickTime at buffer boundaries. Window must be smaller than sampleXXXXX Len!
                    if ( ( TickTime <= (20 * dppqPerSample) ) && ( PlaySample == false ) )
                    {
                        PlaySample = true;
                    }
                    
                    
                    // play pulse if necessary
                    if ( (PlaySample == true) && (SampleNo < sample44100Len) )
                    {
                        OutputData[i] = (float)(0.95f*sample44100[SampleNo]);
                        SampleNo++;
                    }
                    // Otherwise, generate noise to avoid mute and reset sample player
                    else
                    {
                        PlaySample = false;
                        SampleNo = 0;
                        NoisePol ^= 1;
                        
                        if (NoisePol == 1)
                        {
                            OutputData[i] =   ANTI_MUTE_NOISE_LEVEL;
                        }
                        else
                        {
                            OutputData[i] =  -ANTI_MUTE_NOISE_LEVEL;
                        }
                    }
                }
            }
            // if we are in the negative time domain, generate noise to avoid mute
            else
            {
                NoisePol ^= 1;
                
                if (NoisePol == 1)
                {
                    OutputData[i] =   ANTI_MUTE_NOISE_LEVEL;
                }
                else
                {
                    OutputData[i] =  -ANTI_MUTE_NOISE_LEVEL;
                }
            }
            
            // increment ppq position in this block by adding deltaPPQ per sample
            currentPpqPosition = currentPpqPosition + dppqPerSample;
        }
        
        // save last estimated ppq position to use at the beginning of the next block
        lastPluginPpqPositionAtEnd = currentPpqPosition;
    }
    
    
    
    // If we are not playing or recording, generate noise to avoid being muted by the host
    else
    {
        //re-initialize last host ppq to something waaaay back in the past for a fresh start!
        lastHostPpqPositionAtBeginning = (-1.f) * std::numeric_limits<double>::max();
        
        CreatePulses = 0;
        char NoisePol = 0;
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            NoisePol ^= 1;
            if (NoisePol == 0)
            {
                OutputData[i] =  0.0001f;
            }
            else
            {
                OutputData[i] = -0.0001f;
            }
        }
    }
    
    
    
    // Finally, fill output buffers of all channels with the same data from OutputData
    for (int channel = 0; channel < OutputChannels; ++channel)
    {
        float* channelData = buffer.getWritePointer (channel);
        
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            channelData[i] = OutputData[i];
        }
    }
    
}


//==============================================================================
bool AudioClockAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* AudioClockAudioProcessor::createEditor()
{
    return new AudioClockEditor (this);
}

//==============================================================================
void AudioClockAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    //save time signature parameter
    destData.setSize(1, false);
    uint8_t _Measure = (uint8_t)Measure.get();
    destData.insert((void *)&_Measure, 1, 0);
}

void AudioClockAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    //restore time signature and signalize UI to update
    unsigned char memData[1];
    memcpy(memData, data, 1);
    Measure = memData[0];
    
    UiUpdateFlag = true;
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioClockAudioProcessor();
}
