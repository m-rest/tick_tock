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

#ifndef __JUCE_HEADER_F464E1FC338828A7__
#define __JUCE_HEADER_F464E1FC338828A7__


#include "JuceHeader.h"
#include "PluginProcessor.h"

#define NUM_COMBO_ELEM  30.0




class AudioClockEditor  : public AudioProcessorEditor,
                          public Timer,
                          public ComboBox::Listener
{
public:
    //==============================================================================
    AudioClockEditor (AudioClockAudioProcessor* ownerFilter);
    ~AudioClockEditor();

    //==============================================================================
	void timerCallback();
	AudioClockAudioProcessor* getProcessor() const
	{
		return static_cast <AudioClockAudioProcessor*>(getAudioProcessor());
	}

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    
    // Binary resources:
    static const char* plugin_logo_jpg;
    static const int plugin_logo_jpgSize;

private:
    //==============================================================================
    ScopedPointer<ComboBox> measureBox;
    ScopedPointer<Label> measureLabel;
    Image cachedImage_plugin_logo_jpg;
    ScopedPointer<HyperlinkButton> websiteLink;
    
    LookAndFeel_V3 myLookAndFeelV3;
    


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioClockEditor)
};



#endif   // __JUCE_HEADER_F464E1FC338828A7__
