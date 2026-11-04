#pragma once
#include "OscPage.h"
#include <iostream>
namespace noctrl {
inline int envelopeTempoChecks(){
 using namespace juce;
 auto fail=[](const String& why){std::cerr<<"ENV BPM FAIL: "<<why<<"\n";return 1;};
 struct Tempo:AudioPlayHead{double bpm=120;Optional<PositionInfo> getPosition()const override{PositionInfo info;info.setBpm(bpm);return info;}} head;
 auto p=std::make_unique<SynthProcessor>();p->loadPreset(0);p->setPlayHead(&head);
 for(int e=1;e<=4;++e){String id="env"+String(e);p->set(id+"sync",1);p->set(id+"aBeat",14);p->set(id+"holdBeat",11);p->set(id+"dBeat",17);p->set(id+"rBeat",8);}
 p->prepareToPlay(48000,480);AudioBuffer<float> audio(2,480);MidiBuffer midi;
 auto process=[&](int blocks,bool note){for(int i=0;i<blocks;++i){midi.clear();if(note&&i==0)midi.addEvent(MidiMessage::noteOn(1,60,(uint8)100),0);p->processBlock(audio,midi);}};
 process(10,true);
 for(int e=0;e<4;++e){if(std::abs(p->settings.envelopes[e].attack-.5f)>.00001f||std::abs(p->settings.hold[e]-.25f)>.00001f||std::abs(p->settings.envelopes[e].decay-1)>.00001f||std::abs(p->settings.envelopes[e].release-.125f)>.00001f)return fail("all four stage divisions");if(std::abs(p->graphTelemetry->envLevel[e].load()-.2f)>.003f)return fail("actual envelope attack at 120 BPM");}
 head.bpm=60;process(10,false);for(int e=0;e<4;++e)if(std::abs(p->graphTelemetry->envLevel[e].load()-.3f)>.004f)return fail("tempo change did not preserve envelope progress");
 if(std::abs(envelopeBeatSeconds(9,120)-1.f/6)>.00001f||std::abs(envelopeBeatSeconds(10,120)-.1875f)>.00001f)return fail("triplets / dotted divisions");
 {head.bpm=120;auto hold=std::make_unique<SynthProcessor>();hold->loadPreset(0);hold->setPlayHead(&head);hold->set("env1sync",1);hold->set("env1aBeat",0);hold->set("env1holdBeat",14);hold->prepareToPlay(48000,480);for(int block=0;block<20;++block){midi.clear();if(block==0)midi.addEvent(MidiMessage::noteOn(1,60,(uint8)100),0);hold->processBlock(audio,midi);}float elapsed=hold->graphTelemetry->envTime[0].load();head.bpm=60;midi.clear();hold->processBlock(audio,midi);if(hold->graphTelemetry->envStage[0].load()!=5||std::abs(hold->graphTelemetry->envTime[0].load()-(elapsed*2+.01f))>.002f)return fail("Hold tempo change lost musical progress");}
 p->set("env2sync",0);p->set("env2a",.123f);process(1,false);if(std::abs(p->settings.envelopes[1].attack-.123f)>.00001f||p->settings.envelopes[0].attack<.99f)return fail("independent MS / BPM modes");
 MemoryBlock saved;p->getStateInformation(saved);p->loadPreset(0);p->setStateInformation(saved.getData(),(int)saved.getSize());if(p->value("env1sync")!=1||p->value("env2sync")!=0||p->value("env4rBeat")!=8)return fail("mode and division recall");
 p->setPlayHead(nullptr);p->set("seqTempo",90);process(1,false);if(std::abs(p->settings.envelopes[0].attack-2.f/3)>.00001f)return fail("standalone tempo fallback");
 std::unique_ptr<AudioProcessorEditor> editor(p->createEditor());TextButton *ms=nullptr,*bpm=nullptr;std::function<void(Component&)> find=[&](Component& c){if(auto* button=dynamic_cast<TextButton*>(&c))if(button->isVisible()){if(button->getButtonText()=="MS")ms=button;if(button->getButtonText()=="BPM")bpm=button;}for(auto* child:c.getChildren())find(*child);};find(*editor);if(!ms||!bpm)return fail("BPM / MS buttons absent");ms->onClick();if(p->value("env1sync")!=0||p->value("env3sync")!=1)return fail("MS button affected another ENV");bpm->onClick();if(p->value("env1sync")!=1)return fail("BPM button");FileOutputStream out(File::getCurrentWorkingDirectory().getChildFile("Reports/Nexum-ENV-BPM.png"));out.setPosition(0);out.truncate();PNGImageFormat().writeImageToStream(editor->createComponentSnapshot(editor->getLocalBounds()),out);
 std::cout<<"PASS: all four ENV beat divisions, real attack timing, continuous tempo changes, triplets/dotted values, independent MS mode, standalone fallback and state recall.\n";return 0;
}
}
