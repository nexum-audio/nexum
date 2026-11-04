#pragma once
#include "PresetLibrary.h"
#include <iostream>
static int buildGenres(){
 using namespace juce;using namespace noctrl;
 auto recipes=JSON::parse(File::getCurrentWorkingDirectory().getChildFile("GenreRecipes.json").loadFileAsString());if(!recipes.isObject())return 130;
 auto staging=File::getCurrentWorkingDirectory().getChildFile("Tests/GenreBuild").getNonexistentSibling();PresetLibrary library(staging);library.refresh();
 String report="NoCTRL Genre Collection / 8 packs / 40 original synth presets\n",csv="pack,preset,energy,peak,state_recall\n";int total=0;
 auto fail=[](String text){std::cerr<<text<<"\n";return 131;};
 for(auto& genre:recipes.getDynamicObject()->getProperties()){
  String genreName=genre.name.toString(),id;auto created=library.createPack("NoCTRL - "+genreName,"NoCTRL","Five original sounds for "+genreName,id);if(created.failed())return fail(created.getErrorMessage());
  auto* entries=genre.value.getArray();if(!entries||entries->size()!=5)return 132;
  for(auto& recipe:*entries){SynthProcessor p;p.loadPreset(0);auto* parameters=recipe.getProperty("parameters",{}).getDynamicObject();if(!parameters)return 133;
   for(auto& setting:parameters->getProperties()){auto parameterId=setting.name.toString();auto* parameter=p.state.getParameter(parameterId);if(!parameter)return fail("Unknown parameter "+parameterId);auto value=(float)setting.value;auto range=parameter->getNormalisableRange();if(value<range.start||value>range.end)return fail("Out of range "+parameterId);p.set(parameterId,value);}
   PresetMetadata metadata;metadata.name=recipe.getProperty("name",{}).toString();metadata.author="NoCTRL";metadata.category=recipe.getProperty("category",{}).toString();metadata.description=recipe.getProperty("description",{}).toString();metadata.tags=genreName+", factory, original";metadata.notes="Macro 1 adds brightness; Macro 2 adds wavetable movement. Save Current creates an editable copy in your own pack.";
   // Every sound has two useful macro destinations. Leave authored slots intact.
   p.set("mod7src",7);p.set("mod7dst",5);p.set("mod7amt",.22f);p.set("mod8src",8);p.set("mod8dst",1);p.set("mod8amt",.2f);p.set("noise",0);
   MemoryBlock state;p.getStateInformation(state);File saved;auto result=library.savePreset(id,metadata,state,saved);if(result.failed())return fail(result.getErrorMessage());MemoryBlock bytes;if(!saved.loadFileAsData(bytes))return 134;auto xml=AudioProcessor::getXmlFromBinary(bytes.getData(),(int)bytes.getSize());xml->setAttribute("factoryPreset",true);AudioProcessor::copyXmlToBinary(*xml,bytes);if(!saved.replaceWithData(bytes.getData(),bytes.getSize()))return 135;
   SynthProcessor restored;restored.setStateInformation(bytes.getData(),(int)bytes.getSize());for(auto* base:p.getParameters()){auto* parameter=dynamic_cast<AudioProcessorParameterWithID*>(base);if(parameter&&std::abs(p.value(parameter->paramID)-restored.value(parameter->paramID))>.001f)return fail("Recall mismatch "+metadata.name+" "+parameter->paramID);}
   restored.prepareToPlay(48000,512);AudioBuffer<float> audio(2,512);MidiBuffer midi;double energy=0;float peak=0;int note=metadata.category=="Bass"?48:60;
   for(int block=0;block<480;++block){midi.clear();if(block==0){midi.addEvent(MidiMessage::noteOn(1,note,(uint8)100),0);if(metadata.category=="Pad"||metadata.category=="Chord"||metadata.category=="Keys"){midi.addEvent(MidiMessage::noteOn(1,note+4,(uint8)85),0);midi.addEvent(MidiMessage::noteOn(1,note+7,(uint8)80),0);}}if(block==240)midi.addEvent(MidiMessage::allNotesOff(1),0);restored.processBlock(audio,midi);for(int ch=0;ch<2;++ch)for(int i=0;i<512;++i){auto sample=audio.getSample(ch,i);if(!std::isfinite(sample)||std::abs(sample)>1.001f)return fail("Invalid audio "+metadata.name);energy+=sample*sample;peak=jmax(peak,std::abs(sample));}}
   if(energy<.01||peak<.0001f)return fail("Silent sound "+metadata.name);csv+=genreName+",\""+metadata.name+"\","+String(energy,4)+","+String(peak,4)+",PASS\n";++total;std::cout<<"PASS "<<genreName<<" / "<<metadata.name<<" energy="<<energy<<"\n";
  }
 }
 if(total!=40)return 136;report+="40/40 disk-state recalls and finite, bounded, audible 48 kHz renders passed.\nNo external samples or imported Serum assets required.\nSynthesized instrument styles; no acoustic realism or Serum equivalence claimed.\nSource: GenreRecipes.json\n";staging.getChildFile("Genre-Test-Results.csv").replaceWithText(csv);staging.getChildFile("Genre-Test-Summary.txt").replaceWithText(report);File::getCurrentWorkingDirectory().getChildFile("genre-build-path.txt").replaceWithText(staging.getFullPathName());std::cout<<report<<"Output: "<<staging.getFullPathName()<<"\n";return 0;
}
