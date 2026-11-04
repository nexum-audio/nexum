#pragma once
#include "Processor.h"
namespace noctrl {
struct OscControlSpec {const char* id;const char* name;float OscSettings::*field;float low,high,initial;juce::StringArray choices;};
inline const std::vector<OscControlSpec>& oscAdvancedSpecs(){static const std::vector<OscControlSpec> specs={
 {"coarse","CRS",&OscSettings::coarse,-64,64,0,{}},
 {"keyTrack","Pitch tracking",&OscSettings::keyTrack,0,1,1,{"Off","On"}},
 {"bendTrack","Pitch bend tracking",&OscSettings::bendTrack,0,1,1,{"Off","On"}},
 {"phaseMemory","Phase memory",&OscSettings::phaseMemory,0,2,0,{"All Voices","Contiguous","Per Voice"}},
 {"uniMode","Detune mode",&OscSettings::uniMode,0,4,0,{"Linear","Super","Exp","Inv","Random"}},
 {"uniStack","Stack",&OscSettings::uniStack,0,8,0,{"Off","12 (1x)","12 (2x)","12 (3x)","12+7 (1x)","12+7 (2x)","12+7 (3x)","Center -12","Center -24"}},
 {"uniWidth","WIDTH",&OscSettings::uniWidth,0,1,.75f,{}},
 {"uniRange","RANGE (ct)",&OscSettings::uniRange,0,1200,35,{}},
 {"uniPos","WT POS spread",&OscSettings::uniPos,0,1,0,{}},
 {"uniWarp1","WARP 1 spread",&OscSettings::uniWarp1,0,1,0,{}},
 {"uniWarp2","WARP 2 spread",&OscSettings::uniWarp2,0,1,0,{}},
 {"scan","SCAN",&OscSettings::scan,-8,8,1,{}},
 {"sampleStart","START",&OscSettings::sampleStart,0,.999f,0,{}},
 {"sampleEnd","END",&OscSettings::sampleEnd,.001f,1,1,{}},
 {"loopStart","LOOP START",&OscSettings::loopStart,0,.999f,0,{}},
 {"loopEnd","LOOP END",&OscSettings::loopEnd,.001f,1,1,{}},
 {"crossfade","XFADE",&OscSettings::crossfade,0,.5f,.01f,{}},
 {"reverse","Reverse",&OscSettings::reverse,0,1,0,{"Off","On"}},
 {"scanKeyTrack","Scan key track",&OscSettings::scanKeyTrack,0,1,0,{"Off","On"}},
 {"scanSync","Scan tempo sync",&OscSettings::scanSync,0,1,0,{"Off","On"}},
 {"scanBeats","Sample beats",&OscSettings::scanBeats,.0625f,64,4,{}},
 {"exitLoop","Exit loop on release",&OscSettings::exitLoop,0,1,0,{"Off","On"}},
 {"grainDensity","DENS",&OscSettings::grainDensity,.25f,256,15,{}},
 {"grainLength","LENGTH (s)",&OscSettings::grainLength,.001f,2,.1f,{}},
 {"grainOffset","OFFSET",&OscSettings::grainOffset,0,1,0,{}},
 {"grainDirection","DIR",&OscSettings::grainDirection,0,1,0,{}},
 {"grainPitch","PITCH RAND (st)",&OscSettings::grainPitch,0,48,0,{}},
 {"grainLengthRand","LENGTH RAND",&OscSettings::grainLengthRand,0,1,.2f,{}},
 {"grainPan","PAN RAND",&OscSettings::grainPan,0,1,0,{}},
 {"grainLevel","LEVEL RAND",&OscSettings::grainLevel,0,1,0,{}},
 {"grainWindow","Window shape",&OscSettings::grainWindow,0,7,0,{"Hann","Triangle","Welch","Gaussian","Rectangle","Exp Decay","Exp Rise","Sine"}},
 {"grainSkew","Window skew",&OscSettings::grainSkew,.01f,.99f,.5f,{}},
 {"grainWindowAmount","Window amount",&OscSettings::grainWindowAmount,0,1,1,{}},
 {"grainMax","Max grains / voice",&OscSettings::grainMax,1,12,12,{}},
 {"densityMode","Density mode",&OscSettings::densityMode,0,2,0,{"Free Hz","BPM sync","Grains"}},
 {"lengthMode","Length mode",&OscSettings::lengthMode,0,2,0,{"Free seconds","BPM sync","Percent of period"}},
 {"spectralCut","CUT",&OscSettings::spectralCut,0,1,1,{}},
 {"spectralMix","FILTER MIX",&OscSettings::spectralMix,0,1,1,{}},
 {"spectralLow","Low frequency",&OscSettings::spectralLow,0,1,0,{}},
 {"spectralHigh","High frequency",&OscSettings::spectralHigh,0,1,1,{}},
 {"timbre","TIMBRE (st)",&OscSettings::timbre,-48,48,0,{}},
 {"velTrack","Velocity tracking",&OscSettings::velTrack,0,1,1,{}},
 {"multiOverride","Envelope override",&OscSettings::multiOverride,0,1,0,{"Off","On"}},
 {"multiDelay","Delay (s)",&OscSettings::multiDelay,0,5,0,{}},
 {"multiA","Attack (s)",&OscSettings::multiA,.001f,5,.001f,{}},
 {"multiH","Hold (s)",&OscSettings::multiH,0,5,0,{}},
 {"multiD","Decay (s)",&OscSettings::multiD,.001f,5,.1f,{}},
 {"multiS","Sustain",&OscSettings::multiS,0,1,1,{}},
 {"multiR","Release (s)",&OscSettings::multiR,.005f,8,.1f,{}}
 };return specs;}
inline void addOscAdvancedParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout){using namespace juce;for(int o=1;o<=3;++o){String prefix="osc"+String(o);for(auto& spec:oscAdvancedSpecs()){if(spec.choices.isEmpty())layout.add(std::make_unique<AudioParameterFloat>(prefix+spec.id,spec.name,NormalisableRange<float>(spec.low,spec.high),spec.initial));else layout.add(std::make_unique<AudioParameterChoice>(prefix+spec.id,spec.name,spec.choices,(int)spec.initial));}for(int k=0;k<16;++k)layout.add(std::make_unique<AudioParameterFloat>(prefix+"spectralPoint"+String(k+1),"Spectral filter point",0.f,1.f,1.f));for(int w=0;w<2;++w){auto id=prefix+"warp"+String(w+1);layout.add(std::make_unique<AudioParameterFloat>(id+"var","Warp variation",0.f,1.f,0.f));for(int k=0;k<16;++k)layout.add(std::make_unique<AudioParameterFloat>(id+"remap"+String(k+1),"Remap point",0.f,1.f,k/15.f));}}}
inline void applyOscAdvancedSettings(SynthProcessor& p){for(int o=0;o<3;++o){auto prefix="osc"+juce::String(o+1);for(auto& spec:oscAdvancedSpecs())p.settings.osc[o].*spec.field=p.dspValue(prefix+spec.id);for(int k=0;k<16;++k)p.settings.osc[o].spectralCurve[k]=p.dspValue(prefix+"spectralPoint"+juce::String(k+1));for(int w=0;w<2;++w){auto id=prefix+"warp"+juce::String(w+1);p.settings.osc[o].warpVar[w]=p.dspValue(id+"var");for(int k=0;k<16;++k)p.settings.osc[o].remap[w][k]=p.dspValue(id+"remap"+juce::String(k+1));}}}
inline juce::StringArray oscillatorWarpNames(){return {"Off","Bend (legacy)","Sync","Phase distortion","Saturate (legacy)","LPF","HPF","FM Noise (legacy)","PD Next OSC","PD Other OSC","Bend +","Bend -","Bend +/-","PWM","Asym +","Asym -","Asym +/-","Flip","Mirror","Remap 1","Remap 2","Remap 3","Remap 4","Quantize","Odd/Even","Tube","Soft Clip","Hard Clip","Diode 1","Diode 2","Linear Fold","Sine Fold","Zero-Square","Asym","Rectify","Sine Shaper","Stomp Box","Tape Sat","Soft Sat","FM Next OSC","FM Other OSC","FM Sub","FM Filter 1","FM Filter 2","PD Sub","PD Filter 1","PD Filter 2","PD Self","AM Next OSC","AM Other OSC","AM Noise","AM Sub","AM Filter 1","AM Filter 2","RM Next OSC","RM Other OSC","RM Noise","RM Sub","RM Filter 1","RM Filter 2","FM Noise","PD Noise"};}
}
