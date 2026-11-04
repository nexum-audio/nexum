#pragma once
#include <NexumDesign.h>
#include <juce_audio_utils/juce_audio_utils.h>
namespace noctrl {
struct Settings;inline constexpr int liveSourceCount=13;
inline juce::String liveSourceName(int source){return source==11?juce::String("VELO"):source==12?juce::String("NOTE"):source<3?"ENV "+juce::String(source+2):"LFO "+juce::String(source-2);}
inline juce::String liveSourceBadge(int source){return source==11?juce::String("V"):source==12?juce::String("N"):source<3?"E"+juce::String(source+2):"L"+juce::String(source-2);}
inline juce::Colour liveSourceColour(int source){constexpr juce::uint32 colours[]={NEXUM_UI_ACCENT,0xff51d5ac,0xfffa7298,0xffbc84ff,0xffffa34d,0xff5bcfe0,0xffa6d46a,0xffe9cb57,0xfffa7298,0xff51d5ac,NEXUM_UI_ACCENT,NEXUM_UI_ACCENT,NEXUM_UI_ACCENT};return juce::Colour(colours[juce::jlimit(0,liveSourceCount-1,source)]);}
struct LiveModLink {int source=0,index=-1;juce::String target;float base=0,start=0,end=1;juce::RangedAudioParameter* parameter=nullptr;float offset(float amount)const{return start-base+(end-start)*juce::jlimit(0.f,1.f,amount);}};
struct LiveBinding {int offset=-1,kind=0;bool voice=false;void apply(Settings&,float)const;};
struct LiveModGroup {int index=-1;juce::String target;juce::RangedAudioParameter* parameter=nullptr;LiveBinding binding;bool macro=false;std::vector<LiveModLink> links;float value(float base,const std::array<float,liveSourceCount>& sources)const{float n=parameter->convertTo0to1(base);for(auto& link:links)n+=link.offset(sources[(size_t)link.source]);return parameter->convertFrom0to1(juce::jlimit(0.f,1.f,n));}};
struct LiveModRouting {std::vector<LiveModLink> links;std::vector<LiveModGroup> groups;const LiveModLink* find(int source,const juce::String& id)const{for(auto& link:links)if(link.source==source&&link.target==id)return &link;return nullptr;}const LiveModLink* findTarget(const juce::String& id,int selected=-1)const{if(auto* preferred=find(selected,id))return preferred;for(auto& link:links)if(link.target==id)return &link;return nullptr;}juce::ValueTree save()const;void compile();};
LiveBinding liveBinding(const juce::String&);
}
