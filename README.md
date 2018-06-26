# E-RM multiclock VST/AU/AX plugin.

Creates a 24ppq sync pulse for running host transport, starting at the next 
available downbeat, which is configurable by setting time signature as a
plugin parameter.



## Changelog


**v1.0.6**
Switched to JUCE 5.3.2. Added feature to resync starting after a time signature 
change has occured by taking ppqPosSinceLastBarStart into account.
Compiled against AAX SDK 2.3.1 and VST3 SDK 369_01_03_2018.

**v1.0.5**
Switched to JUCE 4.3.0. Fixed bug with PT which reports ppqPosition only
every other block for fs > 48kHz during playback mode. Added version string
and URL to plugin UI. Changed Plugin name from 'Multiclock' to 'E-RM - multiclock'

**v1.0.4**
Re-wrote internal trigger logic completely, based on ppq position and modulo
instead of samples.

**v1.0.3**
no real changes except fixed UI label

**v1.0.2**
Fixed uninitialized temp audio buffer memory which causes spurious glitches on
many machines.

**v1.0.1** 
Added AAX support and SPP accelerator for Ableton Live. Switched to click 
sample at 44.1kHz which should cause problems at higher samplerates due to
shorter duration. Started to re-write/cleanup trigger logic.

**v1.0.0** initial version. 
VST/AU support on Win/OSX. Creates ticks by naively setting audio signal 
to +1.0 for 5ms. Supports all samplerates. Uses Sample-based scheme to determine
'samples to/from next 96th/bar'


