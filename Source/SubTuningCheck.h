#pragma once
#include "OscPage.h"
#include <iostream>
namespace noctrl {
inline int subTuningChecks(){
 using namespace juce;
 auto fail=[](const char* why){std::cerr<<"SUB TUNING FAIL: "<<why<<"\n";return 1;};
 auto render=[](float coarse,float phase){
  auto p=std::make_unique<SynthProcessor>();p->loadPreset(0);
  for(int o=1;o<=3;++o)p->set("osc"+String(o)+"enabled",0);
  p->set("noise",0);p->set("sub",.5f);p->set("subEnabled",1);p->set("subRouting",5);
  p->set("subShape",0);p->set("subOct",0);p->set("subCoarse",coarse);p->set("subPhase",phase);
  p->prepareToPlay(48000,512);AudioBuffer<float> output(2,24576),block(2,512);MidiBuffer midi;
  midi.addEvent(MidiMessage::noteOn(1,60,(uint8)100),0);
  for(int n=0;n<48;++n){block.clear();p->processBlock(block,midi);midi.clear();for(int c=0;c<2;++c)output.copyFrom(c,n*512,block,c,0,512);}
  return output;
 };
 auto base=render(0,0),up=render(12,0),down=render(-12,0),inverted=render(0,180),cycle=render(0,360);
 auto frequency=[](const AudioBuffer<float>& audio){int count=0;double first=0,last=0;for(int i=4097;i<audio.getNumSamples();++i){float a=audio.getSample(0,i-1),b=audio.getSample(0,i);if(a<=0&&b>0){double crossing=i-1-a/(b-a);if(count++==0)first=crossing;last=crossing;}}return (count-1)*48000/(last-first);};
 double f=frequency(base);
 if(std::abs(f-261.625565)>1||std::abs(frequency(up)/f-2)>.002||std::abs(frequency(down)/f-.5)>.002)return fail("coarse tuning did not change actual audio pitch");
 for(int i=0;i<base.getNumSamples();++i){if(!std::isfinite(up.getSample(0,i))||std::abs(base.getSample(0,i)+inverted.getSample(0,i))>.00001f)return fail("180 degree phase");if(std::abs(base.getSample(0,i)-cycle.getSample(0,i))>.00001f)return fail("360 degree wrap");}
 auto p=std::make_unique<SynthProcessor>();p->set("subCoarse",7.5f);p->set("subPhase",123);MemoryBlock saved;p->getStateInformation(saved);p->set("subCoarse",0);p->set("subPhase",0);p->setStateInformation(saved.getData(),(int)saved.getSize());
 if(std::abs(p->value("subCoarse")-7.5f)>.001f||std::abs(p->value("subPhase")-123)>.001f)return fail("state recall");
 auto xml=AudioProcessor::getXmlFromBinary(saved.getData(),(int)saved.getSize());auto legacy=ValueTree::fromXml(*xml);for(auto id:{"subCoarse","subPhase"})legacy.removeChild(legacy.getChildWithProperty("id",id),nullptr);MemoryBlock old;AudioProcessor::copyXmlToBinary(*legacy.createXml(),old);p->setStateInformation(old.getData(),(int)old.getSize());
 if(p->value("subCoarse")!=0||p->value("subPhase")!=0)return fail("legacy sound defaults");
 OscPage page(*p);page.setSize(1208,680);{FileOutputStream out(File::getCurrentWorkingDirectory().getChildFile("Reports/Nexum-Sub.png"));out.setPosition(0);out.truncate();PNGImageFormat().writeImageToStream(page.createComponentSnapshot(page.getLocalBounds()),out);}
 std::cout<<"PASS: actual Sub pitch at +/-12 semitones, 180-degree phase inversion, 360-degree wrap, state recall and neutral legacy defaults.\n";
 return 0;
}
}
