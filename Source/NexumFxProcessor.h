#pragma once
#include "Processor.h"
namespace noctrl {
class NexumFxProcessor:public SynthProcessor {
 juce::AudioBuffer<float> work;juce::SmoothedValue<float> outputGain,globalMix;bool wasBypassed=false;
 std::atomic<double> tempo{120};
 void updateMeters(const juce::AudioBuffer<float>&);
public:
 NexumFxProcessor():SynthProcessor(true){}
 const juce::String getName()const override{return "Nexum FX";}
 bool acceptsMidi()const override{return false;}bool producesMidi()const override{return false;}
 bool isBusesLayoutSupported(const BusesLayout&)const override;
 void prepareToPlay(double,int)override;
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 void processBlockBypassed(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 juce::AudioProcessorParameter* getBypassParameter()const override{return state.getParameter("fxBypass");}
 juce::AudioProcessorEditor* createEditor()override;
 int getNumPrograms()override{return 1;}int getCurrentProgram()override{return 0;}
 void setCurrentProgram(int)override{}const juce::String getProgramName(int)override{return "Default";}
 double getTailLengthSeconds()const override;
 void getStateInformation(juce::MemoryBlock&)override;void setStateInformation(const void*,int)override;
};
}
