#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
namespace noctrl {
struct FxParam {const char* name;float lo,hi,initial;const char* unit;bool logarithmic=false;float from(float n)const{return logarithmic?lo*std::pow(hi/lo,n):lo+(hi-lo)*n;}float to(float v)const{return logarithmic?std::log(v/lo)/std::log(hi/lo):(v-lo)/(hi-lo);}};
struct FxDefinition {juce::String name;std::vector<FxParam> controls;float wet=1;};
const std::vector<FxDefinition>& fxDefinitions();
inline juce::String fxId(int slot,juce::String suffix){return "rack"+juce::String(slot+1)+suffix;}
void addTrancegateParameters(juce::AudioProcessorValueTreeState::ParameterLayout&);
inline float trancegateDefault(int step){return step%2==0?1.f:0.f;}
inline double trancegateStepBeats(int rate){static const double beats[]={1.,.5,.25,.125,.5/3.,.25/3.};return beats[juce::jlimit(0,5,rate)];}
void addFxParameters(juce::AudioProcessorValueTreeState::ParameterLayout&,bool includeLegacy=true);
struct FxEntry {int slot=0,type=0,bus=0;juce::MemoryBlock ir;juce::String irName;};
struct FxRackState {std::vector<FxEntry> entries;juce::ValueTree save()const;};
using FxMeterValues=std::array<std::array<std::atomic<float>,2>,64>;
struct FxAnalysis {
 std::array<std::atomic<float>,1024> input{},output{};std::atomic<unsigned> cursor{0};
 std::array<std::atomic<float>,2> inputPeak{};std::atomic<float> reduction{0},phase{0},correlation{0},rate{48000},tempo{120};
 std::array<std::atomic<float>,3> bandInput{},bandOutput{},bandReduction{};
};
using FxAnalysisValues=std::array<FxAnalysis,64>;
class NativeFxAdapter {
 struct Impl;std::unique_ptr<Impl> impl;
public:
 NativeFxAdapter(int,juce::AudioProcessorValueTreeState&,double);~NativeFxAdapter();
 void reset();void process(juce::AudioBuffer<float>&,int,int,const std::array<float,16>&,double);
};
class FxUnit;
class FxRackRuntime {
 std::vector<std::unique_ptr<FxUnit>> units;std::vector<int> buses,slots;FxMeterValues& meters;FxAnalysisValues& analysis;double sampleRate;
public:
 FxRackRuntime(const FxRackState&,juce::AudioProcessorValueTreeState&,double,FxMeterValues&,FxAnalysisValues&);~FxRackRuntime();
 void process(int bus,juce::AudioBuffer<float>&,int start,int count,const std::vector<float>&,double bpm,double beat=std::numeric_limits<double>::quiet_NaN());
 void reset();
};
}
