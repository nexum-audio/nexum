#pragma once
#include "AutomationSlider.h"
#include "Processor.h"
#include <iostream>
static int automationChecks(){
 using namespace juce;using namespace noctrl;
 struct Context:AudioProcessorEditorHostContext {
  mutable const AudioProcessorParameter* last=nullptr;mutable int requests=0,shown=0;mutable Point<int> position;
  struct Menu:HostProvidedContextMenu {const Context& owner;explicit Menu(const Context& c):owner(c){}PopupMenu getEquivalentPopupMenu()const override{PopupMenu m;m.addItem(1,"Create automation clip");return m;}void showNativeMenu(Point<int> p)const override{++owner.shown;owner.position=p;}};
  std::unique_ptr<HostProvidedContextMenu> getContextMenuForParameter(const AudioProcessorParameter* p)const override{last=p;++requests;return std::make_unique<Menu>(*this);}
 } context;
 SynthProcessor p;for(int type=0;type<16;++type)p.addFx(0,type);std::unique_ptr<AudioProcessorEditor> editor(p.createEditor());editor->setHostContext(&context);int checked=0;bool valid=true;
 std::function<void(Component&)> visit=[&](Component& c){if(auto* slider=dynamic_cast<AutomationSlider*>(&c)){auto id=slider->getComponentID().fromFirstOccurrenceOf("parameter:",false,false);auto* parameter=p.state.getParameter(id);if(!parameter){valid=false;return;}if(editor->getControlParameterIndex(*slider)!=parameter->getParameterIndex())valid=false;auto before=slider->getValue();auto normalized=parameter->getValue();auto now=Time::getCurrentTime();MouseEvent event(Desktop::getInstance().getMainMouseSource(),{22,27},ModifierKeys(ModifierKeys::rightButtonModifier),1,0,0,0,0,slider,slider,now,{22,27},now,1,false);int previous=context.shown;slider->mouseDown(event);slider->mouseDrag(event.withNewPosition(Point<int>(45,4)));slider->mouseUp(event);slider->mouseDoubleClick(event);if(context.shown!=previous+1||context.last!=parameter||context.position!=editor->getLocalPoint(slider,Point<int>(22,27))||slider->getValue()!=before||parameter->getValue()!=normalized)valid=false;++checked;}
  for(auto* child:c.getChildren())visit(*child);
 };visit(*editor);editor->setHostContext(nullptr);if(!valid||checked<50||context.requests!=checked){std::cerr<<"Automation test failed / controls="<<checked<<" requests="<<context.requests<<"\n";return 150;}
 std::cout<<"PASS: all rotary/linear controls request the correct host parameter menu; editor parameter mapping; native menu coordinates; right-click, right-drag and right-double-click preserve values. Mock VST3 host context tested; live FL Studio clip creation not exercised.\n";return 0;
}

