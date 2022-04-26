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

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"


#define ANTI_MUTE_NOISE_LEVEL 0.0001f


//==============================================================================
/**
*/
class AudioClockAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    AudioClockAudioProcessor();
    ~AudioClockAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock);
    void releaseResources();

    void processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages);

    //==============================================================================
    AudioProcessorEditor* createEditor();
    bool hasEditor() const;

    //==============================================================================
    const String getName() const;

    int getNumParameters();

    float getParameter (int index);
    void setParameter (int index, float newValue);

    const String getParameterName (int index);
    const String getParameterText (int index);

    const String getInputChannelName (int channelIndex) const;
    const String getOutputChannelName (int channelIndex) const;
    bool isInputChannelStereoPair (int index) const;
    bool isOutputChannelStereoPair (int index) const;

    bool acceptsMidi() const;
    bool producesMidi() const;
    bool silenceInProducesSilenceOut() const;
    double getTailLengthSeconds() const;

    //==============================================================================
    int getNumPrograms();
    int getCurrentProgram();
    void setCurrentProgram (int index);
    const String getProgramName (int index);
    void changeProgramName (int index, const String& newName);

    //==============================================================================
    void getStateInformation (MemoryBlock& destData);
    void setStateInformation (const void* data, int sizeInBytes);
    
    bool NeedsUiUpdate(){ return UiUpdateFlag; };
	void RequestUiUpdate(){ UiUpdateFlag = true; };
	void ClearUiUpdateFlag(){ UiUpdateFlag = false; };

	enum Parameters
	{
		PMeasure = 0,
		totalNumParams
	};

    Atomic<uint32_t> Measure;

private:
    //==============================================================================
	bool UiUpdateFlag;
    
    double SR;
    float* OutputData;
    
    double lastPluginPpqPositionAtEnd;
    double lastHostPpqPositionAtBeginning;
    bool _triggerCubaseBugCompensation = false;
    bool CreatePulses;
    bool PlaySample;
    bool AccelTicks;
    uint32_t SampleNo;
        
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioClockAudioProcessor)
};

#endif  // PLUGINPROCESSOR_H_INCLUDED
