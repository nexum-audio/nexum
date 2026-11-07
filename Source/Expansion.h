#pragma once
#include "ExternalPresetImport.h"
namespace noctrl {
struct SampleData {
 juce::AudioBuffer<float> audio;double rate=44100;juce::MemoryBlock original;juce::String name;
 static std::shared_ptr<SampleData> decode(const juce::MemoryBlock&,juce::String,juce::String&);
 float read(double position,int channel,bool loop)const noexcept;
};
struct SampleRegion {int lo=0,hi=127,lowVelocity=0,highVelocity=127,root=60;float tune=0,gain=1;std::shared_ptr<const SampleData> sample;juce::String path;};
struct OscExtra {
 int mode=0,root=60,loopMode=1;bool keytrack=true;float start=0,end=1,loopStart=0,loopEnd=1,crossfade=.01f;
 float scanRate=1,grainLength=.1f,density=15,randomOffset=0,randomPitch=0,randomPan=0;
 std::shared_ptr<const SampleData> sample;std::shared_ptr<const ImportedTable> spectral;juce::String sampleName;std::vector<SampleRegion> regions;
 int warp[2]={};float warpAmount[2]={};
};
template<class Setter>inline void oscAssetDefaults(const OscExtra& x,Setter set){set("keyTrack",x.keytrack?1.f:0.f);set("sampleStart",x.start);set("sampleEnd",x.end);set("loopStart",x.loopStart);set("loopEnd",x.loopEnd);set("crossfade",x.crossfade);set("scan",x.scanRate);set("grainLength",x.grainLength);set("grainDensity",x.density);set("grainOffset",x.randomOffset);set("grainPitch",x.randomPitch);set("grainPan",x.randomPan);}
struct ModExtra {int source=0,aux=0,target=0;float amount=0;bool bipolar=false;};
struct EnvExtra {juce::ADSR::Parameters params;float hold=0;float curve[3]={50,50,50};};
struct LfoExtra {std::array<float,256> curve{};double rate=6.25,beats=1;int uiSync=3;bool sync=true,retrigger=false,oneShot=false;};
struct FxModExtra {int source=0,aux=0,unit=0,target=0;float amount=0;bool bipolar=false;};
struct FxExtra {juce::String type;int index=0,rack=0,sourceIndex=0,nativeType=-1;bool enabled=true;std::array<float,16> controls{};std::map<juce::String,int> targets;juce::var document;std::map<juce::String,double> params;double value(const juce::String& key,double fallback)const {auto i=params.find(key);return i==params.end()?fallback:i->second;}};
struct PatternStep {double start=0,length=.25;int note=0;float velocity=.8f;};
struct Expansion {
 std::array<OscExtra,3> osc;std::array<EnvExtra,4> env;std::array<LfoExtra,8> lfo;
 std::array<float,8> macros{};std::vector<ModExtra> mods;std::vector<FxExtra> fx;std::vector<FxModExtra> fxMods;
 std::shared_ptr<const SampleData> noise;bool noiseEnabled=false,whiteNoise=false,noiseOneShot=false,noiseKeytrack=false;
 bool missingAllTonal=false;juce::String noiseName;float noisePitch=0,noiseStart=0,noiseColour=.5f;bool mono=false,legato=false;float glide=0;
 int filterOrder[2]={1,1};float filterRouting[2][5]={{1,0,0,0,0},{0,0,0,0,0}};bool filterSerial=false,filterKeytrack[2]={};float filterWet[2]={1,1},filterDrive[2]={};
 bool arpEnabled=false;double patternLength=4;std::vector<PatternStep> pattern;
 juce::String sourceDocument;
};
struct CurveEnvelope {
 EnvExtra config;double sr=48000;int stage=0;float level=0,releaseLevel=0;double t=0;
 void start(const EnvExtra&,double);void stop();float tick();
};
struct VoiceExtras {
 struct Grain {double pos=0,step=1;int age=0,length=0;float pan=0,gain=1;};
 struct Player {int regionIndex=-1;bool initialized=false;double cursor=-1,scan=0;int direction=1,untilGrain=0;std::array<Grain,12> grains;};
 std::array<std::array<Player,16>,3> players;
 std::array<CurveEnvelope,3> multiEnv;std::array<double,3> multiTime{};struct SpectralFrame{std::array<float,4096> data{};int countdown=0;};std::array<SpectralFrame,3> spectralFrames;juce::dsp::FFT spectralFFT{11};std::array<SVF,3> spectralFilter;std::array<CurveEnvelope,4> env;std::array<double,8> lfoPhase{};std::array<float,64> source{};std::array<float,128> dest{};std::array<float,1024> fxDest{};
 std::array<float,3> toneSources{};std::array<float,4> warpSources{};SVF warpFilters[3][16][2];bool nativeNoiseEnabled=true;SVF secondPole[2][2];
 std::array<const SampleRegion*,3> region{};
 double sr=48000,noiseCursor=0,smoothedFreq=440;float vel=1,note=60,noteRandom=.5f,alternate=0;
 juce::Random random;std::shared_ptr<const Expansion> expansion;
 void start(std::shared_ptr<const Expansion>,double,int,float,double,bool retrigger=true);
 void stop();void modulation(const Settings&,double,float,float);
 std::array<float,2> sampleOsc(int,int,double,int modeOverride=-1,float position=0,int loopOverride=0,const OscSettings* settings=nullptr,double bpm=120);
 float noise(int,double,float pitchOverride=std::numeric_limits<float>::quiet_NaN());
 double warpPhase(int,double,double,float,const OscSettings* settings=nullptr,int unison=0);
 float warpValue(int,float,const OscSettings* settings=nullptr,int unison=0);
};
class PatternPlayer {
 std::array<float,128> held{};std::array<double,128> ending{};double beat=0;int channel=1;
public:
 void reset(){held.fill(0);ending.fill(0);beat=0;}
 void process(const juce::MidiBuffer&,juce::MidiBuffer&,int,double,double,const Expansion&,double swing=0,double grid=.5);
};
class ExpandedFx {
 struct Unit;std::vector<std::unique_ptr<Unit>> units;double sr=48000;
 void chain(juce::AudioBuffer<float>&,int,int,int,int,int,const std::array<float,1024>&,double);
public:
 ExpandedFx();~ExpandedFx();
 void prepare(const Expansion&,double,int,juce::AudioProcessorValueTreeState&);void reset();
 void process(juce::AudioBuffer<float>&,int,int,const Settings&,int bus=0,const std::array<float,1024>* mods=nullptr);
};
void configureImportedFx(FxExtra&);
juce::String describeImportedFx(const Expansion&);
std::shared_ptr<Expansion> parseExpansion(const juce::var&,const juce::File&,ImportResult&);
std::shared_ptr<ImportedTable> spectralTable(const SampleData&,int);
juce::File resolveAsset(juce::File,const juce::String&,juce::String);
juce::var saveExpansionAssets(const Expansion&);
void restoreExpansionAssets(Expansion&,const juce::var&,juce::StringArray&);
}





