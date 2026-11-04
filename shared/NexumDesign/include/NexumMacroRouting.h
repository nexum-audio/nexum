#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <array>
namespace noctrl {
struct MacroLink {
 int macro=0,index=-1;juce::String target;float start=0,end=1;
 juce::RangedAudioParameter* parameter=nullptr;
 float normalized(float amount)const noexcept{return juce::jlimit(0.f,1.f,start+(end-start)*amount);}
};
struct MacroRouting {
 int visibleCount=4;
 std::vector<juce::String> names=std::vector<juce::String>(8);
 static juce::uint32 defaultColour(int m){constexpr juce::uint32 palette[]={0xff48aaff,0xffffa34d,0xffbc84ff,0xff51d5ac,0xfffa7298,0xffe9cb57,0xff5bcfe0,0xffa6d46a};return palette[m%8];}
 std::vector<juce::uint32> colours{defaultColour(0),defaultColour(1),defaultColour(2),defaultColour(3),defaultColour(4),defaultColour(5),defaultColour(6),defaultColour(7)};
 // Extra controls are preset-owned parameters, never added to the host's fixed parameter list.
 // Shared ownership keeps attachments and audio snapshots alive across preset replacement.
 std::vector<std::shared_ptr<juce::AudioParameterFloat>> extra;
 void ensureSize(int count){while((int)names.size()<count){int m=(int)names.size();names.emplace_back();colours.push_back(defaultColour(m));extra.push_back(std::make_shared<juce::AudioParameterFloat>(juce::ParameterID{"macro"+juce::String(m+1),1},"Macro "+juce::String(m+1),0.f,1.f,0.f));}}

 std::vector<MacroLink> links;
 const MacroLink* find(const juce::String& target)const {for(auto& link:links)if(link.target==target)return &link;return nullptr;}
 juce::ValueTree save()const {using namespace juce;ValueTree tree("MacroRouting");tree.setProperty("visibleCount",visibleCount,nullptr);for(int m=0;m<(int)names.size();++m){ValueTree meta("Macro");meta.setProperty("index",m,nullptr);meta.setProperty("name",names[m],nullptr);if(m>=8)meta.setProperty("value",extra[m-8]->get(),nullptr);meta.setProperty("colour",String::toHexString((int)colours[m]),nullptr);tree.addChild(meta,-1,nullptr);}for(auto& link:links){ValueTree node("Link");node.setProperty("macro",link.macro,nullptr);node.setProperty("target",link.target,nullptr);node.setProperty("start",link.start,nullptr);node.setProperty("end",link.end,nullptr);tree.addChild(node,-1,nullptr);}return tree;}
};
}
