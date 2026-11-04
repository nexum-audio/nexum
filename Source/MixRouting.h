#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
namespace noctrl {
inline const juce::String mixSources[5]={"Osc1","Osc2","Osc3","Noise","Sub"};
struct MixSettings {
 float balance[5]={.5f,.5f,.5f,.5f,.5f},send[7][2]={},gain[6]={1,1,1,1,1,1},pan[4]={};
 int extraRoute[2]={},busRoute[2]={};bool enabled[4]={true,true,true,true};
};
inline float mixDbGain(float db){return db<=-60.f?0.f:juce::Decibels::decibelsToGain(db);}
inline void addMixParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout){using namespace juce;
 auto f=[&](String id,String name,float lo,float hi,float initial){layout.add(std::make_unique<AudioParameterFloat>(id,name,NormalisableRange<float>(lo,hi),initial));};
 for(int i=0;i<7;++i){String key=i<5?mixSources[i]:"Filter"+String(i-4);if(i<5)f("mix"+key+"Balance",key+" filter 1/2 balance",0,1,.5f);for(int b=1;b<=2;++b)f("mix"+key+"Bus"+String(b),key+" send to bus "+String(b),0,1,0);}
 for(auto source:{String("sub"),String("noise")})layout.add(std::make_unique<AudioParameterChoice>(source+"Routing",source+" routing",StringArray{"Original","Filter 1","Filter 2","Both filters","Main","Direct","None","Bus 1","Bus 2"},0));
 const String channels[]={"Filter1","Filter2","Bus1","Bus2","Main","Direct"};for(auto channel:channels)f("mix"+channel+"Db",channel+" channel dB",-60,12,0);
 for(int i=2;i<6;++i){auto key=channels[i];f("mix"+key+"Pan",key+" pan",-1,1,0);layout.add(std::make_unique<AudioParameterBool>("mix"+key+"Enabled",key+" enabled",true));}
 for(int b=1;b<=2;++b)layout.add(std::make_unique<AudioParameterChoice>("mixBus"+String(b)+"Route","Bus "+String(b)+" output",StringArray{"Main","Direct",b==1?"Bus 2":"Bus 1"},0));
}
}
