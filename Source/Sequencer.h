#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
namespace noctrl {
class SynthProcessor;
struct SeqPoint {double time=0;float value=0,curve=0;};
struct SeqNote {double start=0,length=.25;int pitch=60,channel=1;float velocity=.8f,chance=1;bool accent=false,muted=false;float strum=0,octaveRandom=0,expressionX=0,expressionY=0,expressionZ=0;std::vector<SeqPoint> pitchCurve;};
// Targets: CC 0..127; 128 pitch bend; 129 pressure; 130..137 macros.
struct SeqLane {int target=1,channel=1;std::vector<SeqPoint> points;};
struct SeqSlot {juce::String name;std::vector<SeqNote> notes;std::vector<SeqLane> lanes;};
struct SequencerBank {std::array<SeqSlot,12> clips,arps;int preview=0;SequencerBank();juce::ValueTree save()const;static std::shared_ptr<SequencerBank> restore(const juce::ValueTree&);static std::shared_ptr<SequencerBank> fromSerum(const juce::var&);};
inline juce::String seqId(bool arp,int slot,juce::String key){return (arp?"seqA":"seqC")+juce::String(slot+1)+key;}
void addAdvancedClipParameters(juce::AudioProcessorValueTreeState::ParameterLayout&);
void copyClipSlot(SynthProcessor&,int,int);
void addAdvancedArpParameters(juce::AudioProcessorValueTreeState::ParameterLayout&);
void copyArpSlot(SynthProcessor&,int,int);
void addSequencerParameters(juce::AudioProcessorValueTreeState::ParameterLayout&);
void importSequencerSettings(SynthProcessor&,const juce::var&);
struct RecordedSeqEvent {int slot=0,kind=0,pitch=60,channel=1,target=1;double start=0,length=.25;float value=.8f;};
class SequencerEngine {
 struct Impl;std::unique_ptr<Impl> impl;
public:
 explicit SequencerEngine(SynthProcessor&);~SequencerEngine();void reset();
 void process(SynthProcessor&,const juce::MidiBuffer&,juce::MidiBuffer&,int,double,double,bool,double,bool);
 void applyAutomation(SynthProcessor&,int);bool automationActive()const;
 const juce::MidiBuffer& clipOutput()const;
};
}
