#pragma once
#include "Processor.h"
#include "AutomationSlider.h"
#include "PresetLibrary.h"
#include <iostream>
namespace noctrl {
inline int macroSlotsChecks(){using namespace juce;auto p=std::make_unique<SynthProcessor>();p->loadPreset(0);auto fail=[](const char* why){std::cerr<<"MACRO SLOTS FAIL: "<<why<<"\n";return 1;};int parameters=p->getParameters().size();
 std::unique_ptr<AudioProcessorEditor> editor(p->createEditor());Component* sidebar=nullptr;Button* add=nullptr;Viewport* viewport=nullptr;MacroDragHandle* handle=nullptr;AutomationSlider* target=nullptr;AutomationSlider* macro24=nullptr;
 std::function<void(Component&)> find=[&](Component& c){auto id=c.getComponentID();if(id=="nexum-sidebar")sidebar=&c;if(id=="macro-add")add=dynamic_cast<Button*>(&c);if(id=="macro-viewport")viewport=dynamic_cast<Viewport*>(&c);if(id=="macro-drag:24")handle=dynamic_cast<MacroDragHandle*>(&c);if(id=="parameter:macro24")macro24=dynamic_cast<AutomationSlider*>(&c);if(id=="parameter:osc1level")target=dynamic_cast<AutomationSlider*>(&c);for(auto* child:c.getChildren())find(*child);};
 auto scan=[&]{sidebar=nullptr;add=nullptr;viewport=nullptr;handle=nullptr;target=nullptr;macro24=nullptr;find(*editor);};scan();
 if(!sidebar||!add||!viewport||p->visibleMacroCount()!=4||!add->isVisible())return fail("default four macros and add field");
 auto snapshot=[&](String name){FileOutputStream out(File::getCurrentWorkingDirectory().getChildFile("Reports/"+name));out.setPosition(0);out.truncate();PNGImageFormat().writeImageToStream(editor->createComponentSnapshot(editor->getLocalBounds()),out);};snapshot("Nexum-Four-Macros.png");
 for(int i=4;i<24;++i)add->onClick();scan();
 if(p->visibleMacroCount()!=24||!add->isVisible()||!handle||!macro24||!target||viewport->getViewPositionY()<=0||!viewport->getLocalBounds().intersects(viewport->getLocalArea(add,add->getLocalBounds())))return fail("dynamic controls / scroll to new macro");
 p->setMacroInfo(23,"Vocal Motion",Colour(0xff51d5ac));macro24->setValue(.73,sendNotificationSync);if(std::abs(p->value("macro24")-.73f)>.0001f)return fail("dynamic knob value");
 DragAndDropTarget::SourceDetails source(handle->dragDescription(),handle,{10,10});if(!target->isInterestedInDragSource(source))return fail("extra macro drop rejected");target->itemDropped(source);p->setMacroRange("osc1level",0,.85f);p->updateSettings();if(!p->macros()->find("osc1level")||p->macros()->find("osc1level")->macro!=23||std::abs(p->dspValue("osc1level")-.73f*.85f)>.0001f)return fail("macro24 DSP route");snapshot("Nexum-Dynamic-Macros.png");
 auto root=File::getCurrentWorkingDirectory().getChildFile("Tests/DynamicMacros").getNonexistentSibling();PresetLibrary library(root);String packId;if(library.createPack("Macro test","","",packId).failed())return fail("preset pack");PresetMetadata metadata;metadata.name="24 Macros";MemoryBlock saved;p->getStateInformation(saved);File file;if(library.savePreset(packId,metadata,saved,file).failed())return fail("save preset file");MemoryBlock fromFile;if(!file.loadFileAsData(fromFile))return fail("read preset file");
 // Restore while the old editor exists, then reconstruct it as on reopening the plugin.
 p->loadPreset(0);if(p->visibleMacroCount()!=4||p->value("macro24")!=0||!p->macros()->links.empty())return fail("Init did not clear dynamic macros");p->setStateInformation(fromFile.getData(),(int)fromFile.getSize());editor.reset();editor.reset(p->createEditor());scan();
 if(p->visibleMacroCount()!=24||p->macros()->names[23]!="Vocal Motion"||p->macros()->colours[23]!=0xff51d5ac||std::abs(p->value("macro24")-.73f)>.0001f||!handle||!macro24||p->macros()->links.size()!=1)return fail("preset recall metadata / values / assignments / editor");
 p->set("osc1random",0);p->set("osc1routing",3);p->set("env1s",1);p->set("macro24",0);p->prepareToPlay(48000,512);AudioBuffer<float> audio(2,512);MidiBuffer midi;double before=0,after=0;
 for(int b=0;b<100;++b){midi.clear();if(b==0)midi.addEvent(MidiMessage::noteOn(1,60,(uint8)100),0);if(b==40)p->set("macro24",1);p->processBlock(audio,midi);for(int i=0;i<512;++i){float v=audio.getSample(0,i);if(!std::isfinite(v)||std::abs(v)>1.001f)return fail("extra macro audio bounds");if(b>10&&b<40)before+=v*v;if(b>50)after+=v*v;}}
 if(before>1e-7||after<.1)return fail("extra macro not audible on held note");
 editor.reset();p->loadPreset(0);p->assignMacro(7,"filter1cut");if(p->visibleMacroCount()!=8)return fail("legacy macro8 route hidden");p->loadPreset(0);p->setMacroInfo(6,"Legacy name",Colours::blue);if(p->visibleMacroCount()!=7)return fail("legacy name hidden");p->loadPreset(0);p->set("macro5",.3f);if(p->visibleMacroCount()!=5)return fail("legacy value hidden");
 p->loadPreset(0);for(int i=4;i<128;++i)if(!p->addMacro())return fail("arbitrary eight-slot limit");if(p->visibleMacroCount()!=128||p->getParameters().size()!=parameters)return fail("growth or stable host parameters");p->getStateInformation(saved);p->loadPreset(0);p->setStateInformation(saved.getData(),(int)saved.getSize());if(p->visibleMacroCount()!=128)return fail("128 slot recall");
 std::cout<<"PASS: 4 default / 128 dynamic macros, plus and scroll, macro24 drag assignment and audible held-note modulation, real preset file metadata/value/link recall, Init reset and stable legacy automation parameters.\n";return 0;
}
}
