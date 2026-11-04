#include "Processor.h"

namespace noctrl {
class NexumSynthProcessor final : public SynthProcessor {
public:
 const juce::String getName() const override { return "Nexum"; }
};
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
 return new noctrl::NexumSynthProcessor();
}
