#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
namespace noctrl {
inline bool isSynthState(const juce::XmlElement& xml){
 if(xml.hasTagName("NEXUM_SYNTH"))return true;
 return xml.getChildByAttribute("id","osc1enabled")!=nullptr && xml.getChildByAttribute("id","osc2enabled")!=nullptr;
}
inline juce::ValueTree readSynthState(const juce::XmlElement& xml){
 auto source=juce::ValueTree::fromXml(xml);juce::ValueTree result("NEXUM_SYNTH");
 result.copyPropertiesAndChildrenFrom(source,nullptr);return result;
}
}
