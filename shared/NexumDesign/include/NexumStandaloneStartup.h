#pragma once
#include <juce_data_structures/juce_data_structures.h>
namespace nexum::family {
// Only the standalone launcher calls this; explicit preset/project loads do not.
inline void prepareInitStartup(juce::PropertySet& settings) {
    const auto previous=settings.getValue("filterState");
    if(previous.isNotEmpty())settings.setValue("previousSessionState",previous);
    settings.removeValue("filterState");
}
}
