#include "NexumFxProcessor.h"
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter(){return new noctrl::NexumFxProcessor();}
