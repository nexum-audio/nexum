#include "SynthState.h"
#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include "MacroRouting.h"
#include "MixRouting.h"
#include "FxRack.h"
#include "Sequencer.h"
#include "LiveModulation.h"
#include "VoicingMath.h"
#include "EnvelopeTiming.h"
namespace noctrl {
inline float cleanOutput(float v)noexcept{float x=std::abs(v);return x<=.95f?v:std::copysign(.95f+.05f*std::tanh((x-.95f)/.05f),v);}
constexpr int tableSize=2048, frames=8, banks=5, mips=10;
struct Wavetables {
 std::vector<float> data;
 explicit Wavetables(bool generate=true);
 float sample(int bank,float position,double phase,double frequency,double sr) const noexcept;
 static float harmonic(int bank,int frame,int h);
};
// Keep the original four shape indices stable for saved sounds.
inline float subWaveform(int shape,double phase)noexcept{float t=(float)phase;switch(shape){case 1:return 1-4*std::abs(t-.5f);case 2:return 2*t-1;case 3:return t<.5f?1.f:-1.f;case 4:return std::tanh(2.f*std::sin(t*juce::MathConstants<float>::twoPi))/std::tanh(2.f);case 5:return t<.25f?1.f:-1.f/3.f;default:return std::sin(t*juce::MathConstants<float>::twoPi);}}
struct OscSettings {float position=0,level=0,detune=0;int octave=0,unison=1,bank=0;bool enabled=true;float semi=0,fine=0,initial=0,random=1,pan=0,blend=1;int warp[2]={};float warpAmount[2]={};int routing=0,sourceMode=0,loopMode=0;
 float coarse=0,keyTrack=1,bendTrack=1,phaseMemory=0,uniMode=0,uniStack=0,uniWidth=.75f,uniRange=35,uniPos=0,uniWarp1=0,uniWarp2=0;
 float scan=1,sampleStart=0,sampleEnd=1,loopStart=0,loopEnd=1,crossfade=.01f,reverse=0,scanKeyTrack=0,scanSync=0,scanBeats=4,exitLoop=0;
 float grainDensity=15,grainLength=.1f,grainOffset=0,grainDirection=0,grainPitch=0,grainLengthRand=.2f,grainPan=0,grainLevel=0,grainWindow=0,grainSkew=.5f,grainWindowAmount=1,grainMax=12,densityMode=0,lengthMode=0;
 float spectralCut=1,spectralMix=1,spectralLow=0,spectralHigh=1,timbre=0,velTrack=1,multiOverride=0,multiDelay=0,multiA=.001f,multiH=0,multiD=.1f,multiS=1,multiR=.1f;
 std::array<float,16> spectralCurve{{1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}};float warpVar[2]={};std::array<std::array<float,16>,2> remap{};
};
inline float unisonSpread(const OscSettings& s,int voice,float seed=.5f){if(s.unison<=1)return 0;float x=2.f*voice/(s.unison-1)-1;switch((int)s.uniMode){case 1:return std::copysign(std::pow(std::abs(x),1.7f),x);case 2:return x*x*x;case 3:return std::copysign(std::sqrt(std::abs(x)),x);case 4:{float v=std::sin((voice+1)*127.1f+seed*311.7f)*43758.5453f;return (v-std::floor(v))*2-1;}default:return x;}}
inline float unisonStack(const OscSettings& s,int voice){int mode=(int)s.uniStack;if(mode>=1&&mode<=3)return (float)(12*(voice%(mode+1)));if(mode>=4&&mode<=6){int n=voice%((mode-3)*2+1);return (float)((n/2)*12+(n%2?7:0));}bool centre=voice==s.unison/2||(s.unison%2==0&&voice==s.unison/2-1);return centre&&mode>=7?(mode==7?-12.f:-24.f):0;}
inline bool unisonCentre(int count,int voice){return voice==count/2||(count%2==0&&voice==count/2-1);}

struct Route {int source=0,dest=0;float amount=0;};
struct Settings {
 std::array<OscSettings,3> osc;MixSettings mix;
 juce::ADSR::Parameters amp,mod;std::array<juce::ADSR::Parameters,4> envelopes;float hold[4]={};bool envTempoSync[4]={};
 float sub=0,noise=0,cutoff[2]={1000,40},resonance[2]={.1f,.1f},filterEnv=0;
 int filterType[2]={0,3};
 float lfoRate[8]={1,1},lfoDepth[2]={0,0};int lfoShape[8]={},lfoSync[8]={};float lfoOffset[8]={};bool lfoEdited[8]={},lfoRetrigger[8]={},lfoOneShot[8]={};std::array<std::array<float,16>,8> lfoPoints;
 std::array<Route,8> routes;
 float macro[8]={},filterDrive[2]={},filterWet[2]={1,1},filterPan[2]={};bool filterEnabled[2]={true,true};int filterRoute[2]={0,0};bool subEnabled=true,noiseEnabled=true,serial=true,mono=false;int polyphony=12,subShape=0;float subPan=0,subOct=-1,subCoarse=0,subInitial=0,noisePan=0,noisePitch=0,glide=0;bool legato=false,portaAlways=true,portaScaled=false;float portaCurve=0;std::array<float,16> velocityCurve{},noteCurve{};
 float drive=0,chorus=0,delay=0,feedback=.3f,reverb=0,output=.65f;
 int delayTime=3;double bpm=120;
};
struct SVF {
 float s1=0,s2=0,g=.1f,k=1;
 void reset(){s1=s2=0;}
 void configure(float hz,float resonance,double sr){g=std::tan(juce::MathConstants<float>::pi*juce::jlimit(15.f,(float)sr*.43f,hz)/(float)sr);k=2-1.94f*resonance;}
 float process(float v,int type){if(type==3)return v;float a=1/(1+g*(g+k));float v1=a*(s1+g*(v-s2));float v2=s2+g*v1;s1=2*v1-s1;s2=2*v2-s2;return type==0?v2:type==1?v-k*v1-v2:v1;}
};
struct Expansion;struct ImportedAssets;
class SynthProcessor;
struct Sound:juce::SynthesiserSound {bool appliesToNote(int)override{return true;}bool appliesToChannel(int)override{return true;}};
struct LegatoVoice {virtual ~LegatoVoice()=default;virtual void retargetNote(int,float)=0;};
class SynthesiserEx:public juce::Synthesiser {
 int voiceLimit=12;bool mono=false,legato=false;uint32_t age=0;std::array<std::array<uint32_t,128>,16> held{};std::array<std::array<float,128>,16> velocities{};std::array<int,16> current{};
public:
 SynthesiserEx(){resetExpression();}
 bool transitionOverlap=false;
 void setLegato(bool on){legato=on;}
 bool retargetActive(int channel,int note,float velocity){for(int i=0;i<getNumVoices();++i){auto* voice=getVoice(i);if(voice->isVoiceActive()&&voice->isPlayingChannel(channel))if(auto* linked=dynamic_cast<LegatoVoice*>(voice)){linked->retargetNote(note,velocity);return true;}}return false;}
 std::array<std::array<float,3>,16> expression{};std::array<int,16> channelRange{{-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1}},rpnMsb{},rpnLsb{};
 void handleController(int channel,int cc,int value)override{int c=juce::jlimit(0,15,channel-1);if(cc==74)expression[c][0]=value/127.f;if(cc==71)expression[c][1]=value/127.f;if(cc==101)rpnMsb[c]=value;if(cc==100)rpnLsb[c]=value;if(cc==6&&rpnMsb[c]==0&&rpnLsb[c]==0)channelRange[c]=value;juce::Synthesiser::handleController(channel,cc,value);}
 void handleChannelPressure(int channel,int value)override{expression[juce::jlimit(0,15,channel-1)][2]=value/127.f;juce::Synthesiser::handleChannelPressure(channel,value);}
 void resetExpression(){expression={};channelRange.fill(-1);rpnMsb.fill(127);rpnLsb.fill(127);}
 void setVoiceLimit(int limit){voiceLimit=juce::jlimit(1,64,limit);}
 juce::SynthesiserVoice* findFreeVoice(juce::SynthesiserSound* sound,int channel,int note,bool steal)const override{int active=0;for(int i=0;i<getNumVoices();++i)active+=getVoice(i)->isVoiceActive()?1:0;if(active>=voiceLimit){if(!steal)return nullptr;juce::SynthesiserVoice* oldest=nullptr;for(int i=0;i<getNumVoices();++i){auto* voice=getVoice(i);if(voice->isVoiceActive()&&voice->canPlaySound(sound)&&(!oldest||voice->wasStartedBefore(*oldest)))oldest=voice;}return oldest;}return juce::Synthesiser::findFreeVoice(sound,channel,note,steal);}
 void setMono(bool on){if(on!=mono){juce::Synthesiser::allNotesOff(0,false);mono=on;for(auto& notes:held)notes.fill(0);current.fill(-1);}}
 void noteOn(int channel,int note,float velocity)override{int c=juce::jlimit(0,15,channel-1);transitionOverlap=std::any_of(held[c].begin(),held[c].end(),[](auto n){return n>0;});held[c][note]=++age;velocities[c][note]=velocity;if(!mono){juce::Synthesiser::noteOn(channel,note,velocity);return;}if(legato&&transitionOverlap&&retargetActive(channel,note,velocity)){current[c]=note;return;}juce::Synthesiser::allNotesOff(channel,false);current[c]=note;juce::Synthesiser::noteOn(channel,note,velocity);}
 void noteOff(int channel,int note,float velocity,bool tail)override{int c=juce::jlimit(0,15,channel-1);held[c][note]=0;if(!mono){juce::Synthesiser::noteOff(channel,note,velocity,tail);return;}if(current[c]!=note)return;int latest=-1;uint32_t order=0;for(int k=0;k<128;++k)if(held[c][k]>order){order=held[c][k];latest=k;}if(latest>=0){transitionOverlap=true;if(legato&&retargetActive(channel,latest,velocities[c][latest])){current[c]=latest;return;}juce::Synthesiser::allNotesOff(channel,false);current[c]=latest;juce::Synthesiser::noteOn(channel,latest,velocities[c][latest]);}else{current[c]=-1;for(int i=0;i<getNumVoices();++i){auto* voice=getVoice(i);if(voice->isVoiceActive()&&voice->isPlayingChannel(channel))juce::Synthesiser::noteOff(channel,voice->getCurrentlyPlayingNote(),velocity,tail);}}}
 void allNotesOff(int channel,bool tail)override{for(int c=0;c<16;++c)if(channel==0||c==channel-1){held[c].fill(0);current[c]=-1;}juce::Synthesiser::allNotesOff(channel,tail);}
};
struct VoiceExtras;class ExpandedFx;class PatternPlayer;
class Voice:public juce::SynthesiserVoice,public LegatoVoice {
 SynthProcessor& owner;std::unique_ptr<VoiceExtras> extra;
 std::array<std::array<double,16>,3> phase{},rememberedPhase{};std::array<bool,3> hasPhase{};
 PortamentoRamp portamento;int playedNote=60;
 int telemetrySlot=0;int lastPitch=8192;juce::uint64 liveSerial=0;double subPhase=0,freq=440,bend=1,glidingFrequency=440,envelopeTempo=120;float velocity=1,pan=0,wheel=0,pressure=0;std::array<float,3> expression{};
 juce::ADSR amp,mod;SVF filters[2][2];juce::Random random;int counter=0;bool pedal=false;
public:
 explicit Voice(SynthProcessor&);~Voice()override;
 bool canPlaySound(juce::SynthesiserSound* s)override{return dynamic_cast<Sound*>(s)!=nullptr;}
 void startNote(int,float,juce::SynthesiserSound*,int)override;
 void retargetNote(int,float)override;
 void stopNote(float,bool)override;
 void pitchWheelMoved(int)override;
 void controllerMoved(int,int)override;
 void aftertouchChanged(int)override;
 void channelPressureChanged(int)override;
 void renderNextBlock(juce::AudioBuffer<float>&,int,int)override;
 void prepare(double);
};
class SynthProcessor:public juce::AudioProcessor {
public:
 const bool effectOnly;
 juce::AudioProcessorValueTreeState state;
 Wavetables tables;Settings settings;SynthesiserEx synth;juce::MidiKeyboardState keyboard;
 std::vector<std::atomic<float>*> rawParameters; std::atomic<bool> panicRequested{false};
 std::array<std::atomic<float>,2> masterMeters{};std::atomic<float> meter{0};std::atomic<int> activeVoices{0};std::atomic<float> lastVelocity{0},lastNote{0};
 std::array<std::array<float,8192>,8> lfoBuffer{};
 juce::dsp::Chorus<float> chorus;juce::Reverb reverb;juce::AudioBuffer<float> delayBuffer;std::array<std::array<float,8192>,2> directBus{};
 std::vector<std::array<std::array<float,8192>,2>> mixBuses{2};
 std::vector<std::array<std::array<float,8192>,2>> mixTap{7};
 struct GraphTelemetry {
 struct OscVoice {std::array<std::atomic<float>,3> phase{},position{};std::array<std::atomic<int>,3> region{};std::atomic<int> note{-1};std::atomic<juce::uint64> serial{0};std::atomic<const Expansion*> asset{nullptr};};
 std::array<OscVoice,64> oscVoices;
 std::array<std::array<std::atomic<float>,3>,64> samplePlayheads;
 GraphTelemetry(){for(auto& voice:samplePlayheads)for(auto& position:voice)position.store(-1);}
 std::array<std::array<std::atomic<float>,512>,5> source{};
 std::array<std::atomic<float>,4> envLevel{},envTime{};std::array<std::atomic<int>,4> envStage{};
 std::array<std::atomic<float>,8> lfo{};std::array<std::atomic<float>,2> cutoff{};
 std::array<std::atomic<float>,3> position{};std::atomic<unsigned> cursor{0};std::atomic<float> rate{48000},tempo{120};
 };
 std::unique_ptr<GraphTelemetry> graphTelemetry=std::make_unique<GraphTelemetry>();
 std::array<std::atomic<float>,11> mixMeters{};std::array<float,11> mixPeak{};
 int delayIndex=0;double lfoPhase[8]={};double sampleRate=48000;double fxHostBeat=std::numeric_limits<double>::quiet_NaN();juce::SmoothedValue<float> master,delaySamples,drive;
 std::shared_ptr<const SequencerBank> sequenceBank=std::make_shared<const SequencerBank>();std::vector<std::shared_ptr<const SequencerBank>> retiredSequences;std::unique_ptr<SequencerEngine> sequenceEngine;
 std::array<std::atomic<int>,128> generatedNotes{};std::atomic<int> clipRequest{-1},clipPlayingMask{0},clipResumeMask{0},clipStopMask{0};std::atomic<bool> clearSequenceRecording{false};std::array<std::atomic<float>,12> clipRecordedLength{};std::array<float,8192> sequenceMetronome{};std::atomic<float> clipPlayhead{0},arpPlayhead{0};std::atomic<int> arpPlayingSlot{0},arpQueuedSlot{-1};std::array<std::atomic<int>,128> arpHeldNotes{};std::atomic<int> clipQueuedMask{0};std::array<std::atomic<float>,12> clipSlotProgress{};
 std::array<float,8> sequenceMacroOverride{{-1,-1,-1,-1,-1,-1,-1,-1}};std::array<std::atomic<float>,8> sequenceMacroLive{};
 juce::AbstractFifo sequenceRecordFifo{1024};std::array<RecordedSeqEvent,1024> sequenceRecorded{};juce::CriticalSection sequenceEditLock;
 std::shared_ptr<const SequencerBank> sequences()const{return std::atomic_load(&sequenceBank);}
 void setSequences(std::shared_ptr<const SequencerBank>);void drainSequenceRecording();bool importSequenceMidi(int,const juce::File&,juce::String&);bool exportSequenceMidi(int,const juce::File&,juce::String&);
 bool sequenceArpEnabled()const;void setSequenceArpEnabled(bool);
 std::unique_ptr<PatternPlayer> patternPlayer;std::unique_ptr<Expansion> nativeArpPattern;juce::MidiBuffer arpMidi,transposedMidi;
 std::array<std::array<int,128>,16> noteMap{};bool previousPatternActive=false;
 std::atomic<int> requestedPitch{-1},requestedMod{-1},pitchWheelValue{8192},modWheelValue{0};std::atomic<bool> modWheelTouched{false};
 void transformPerformanceMidi(juce::MidiBuffer&,int);
 std::unique_ptr<ExpandedFx> expandedFx;std::array<float,5> fxMod{};std::array<float,1024> importedFxMod{};std::atomic<double> previousPitch{0};std::array<std::array<double,16>,3> contiguousPhase{};
 explicit SynthProcessor(bool fxOnly=false);~SynthProcessor()override;
 static juce::AudioProcessorValueTreeState::ParameterLayout layout(bool fxOnly=false);
 std::shared_ptr<const FxRackState> fxRackState=std::make_shared<const FxRackState>();
 FxMeterValues fxMeters{};std::unique_ptr<FxAnalysisValues> fxAnalysis=std::make_unique<FxAnalysisValues>();std::shared_ptr<FxRackRuntime> fxRuntime;std::vector<std::shared_ptr<FxRackRuntime>> retiredFx;
 juce::AudioBuffer<float> fxBusBuffer{2,8192};
 std::shared_ptr<const FxRackState> rack()const{return std::atomic_load(&fxRackState);}
 int addFx(int bus,int type);void removeFx(int slot);void moveFx(int slot,int delta);void moveFxTo(int slot,int index);
 bool loadFxIR(int slot,const juce::File&,juce::String& error);
 void rebuildFxRack();void restoreFxRack(const juce::ValueTree&);
 std::shared_ptr<const MacroRouting> macroRouting=std::make_shared<const MacroRouting>();
 std::shared_ptr<const LiveModRouting> liveModRouting=std::make_shared<const LiveModRouting>(),blockLiveRouting;
 std::vector<std::shared_ptr<const LiveModRouting>> retiredLiveRouting;std::atomic<juce::uint64> liveVoiceSerial{0};juce::uint64 liveSourceVoice=0;std::atomic<int> selectedLiveSource{-1};std::array<std::atomic<float>,liveSourceCount> liveSources{};std::vector<float> liveModBaseValues;
 std::shared_ptr<const LiveModRouting> liveMods()const{return std::atomic_load(&liveModRouting);}
 bool assignLiveMod(int,const juce::String&);void setLiveModRange(int,const juce::String&,float,float);void removeLiveMod(int,const juce::String&);void restoreLiveMods(const juce::ValueTree&);void publishLiveMods(std::shared_ptr<LiveModRouting>);void refreshLiveGlobals();
 std::atomic<int> selectedMacro{-1};
 std::vector<float> effectiveParameters;
 std::vector<juce::SmoothedValue<float>> macroSmoothing;
 std::array<int,8> macroIndices{};
 std::shared_ptr<const MacroRouting> macros()const{return std::atomic_load(&macroRouting);}
 int visibleMacroCount()const;bool addMacro();
 bool assignMacro(int,const juce::String&);
 void setMacroRange(const juce::String&,float,float);
 void removeMacroLink(const juce::String&);
 void setMacroInfo(int,juce::String,juce::Colour);
 void restoreMacros(const juce::ValueTree&);
 float dspValue(const juce::String& id)const;
 void updateSettings(int advance=0);float value(const juce::String& id)const;void set(const juce::String&,float);
 std::shared_ptr<const ImportedAssets> importedAssets,blockAssets;
 bool loadSerumPreset(const juce::File&,const juce::File&,juce::String& error);
 bool setOscWavetable(int,const juce::AudioBuffer<float>&,juce::String,juce::String&);
 bool loadOscillatorAsset(int,const juce::File&,int,juce::String&);
 void resetOscillatorForAsset(int,ImportedAssets&,Expansion&,juce::var&);
 bool loadOscillatorAssetData(int,const juce::File&,int,const juce::MemoryBlock&,juce::String&);
 bool loadNoisePreset(int,juce::String&);
 void loadPreset(int);static juce::StringArray presetNames();int currentPreset=0;
 void prepareToPlay(double,int)override;void releaseResources()override{}
 void processBlock(juce::AudioBuffer<float>&,juce::MidiBuffer&)override;
 void renderChunk(juce::AudioBuffer<float>&,juce::MidiBuffer&,int,int);
 bool isBusesLayoutSupported(const BusesLayout& b)const override{return b.getMainInputChannelSet().isDisabled()&&b.getMainOutputChannelSet()==juce::AudioChannelSet::stereo();}
 const juce::String getName()const override{return "Nexum";}bool acceptsMidi()const override{return true;}bool producesMidi()const override{return true;}
 bool hasEditor()const override{return true;}juce::AudioProcessorEditor* createEditor()override;
 double getTailLengthSeconds()const override{return 12;}
 int getNumPrograms()override{return presetNames().size();}int getCurrentProgram()override{return currentPreset;}void setCurrentProgram(int n)override{loadPreset(n);}const juce::String getProgramName(int n)override{return presetNames()[n];}void changeProgramName(int,const juce::String&)override{}
 void getStateInformation(juce::MemoryBlock&)override;void loadSoundState(const void*,int);
 void setStateInformation(const void*,int)override;
};
float lfoWave(double phase,int shape);
}





