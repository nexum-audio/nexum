#pragma once
#include <NexumSidebar.h>
#include "OscPage.h"
namespace noctrl {
using namespace juce;
class SynthSidebar:public nxf::SidebarFrame {
 SynthProcessor& p;
 OwnedArray<OscKnob> controls;std::vector<std::shared_ptr<AudioParameterFloat>> boundExtra;
public:
 SynthSidebar(SynthProcessor& processor,int tabs=5,String suffix={}):SidebarFrame(tabs,suffix,JucePlugin_VersionString),p(processor){add.onClick=[this]{p.addMacro();refreshMacros();viewport.setViewPosition(0,jmax(0,list.getHeight()-viewport.getHeight()));};}
 void refreshMacros(){
  auto state=p.macros();int count=p.visibleMacroCount();
  // New extra slots preserve existing controls; preset replacement rebinds their attachments.
  bool replaced=boundExtra.size()>state->extra.size();for(size_t i=0;i<jmin(boundExtra.size(),state->extra.size());++i)if(boundExtra[i]!=state->extra[i])replaced=true;
  if(replaced)while(controls.size()>8)controls.removeLast();
  while(controls.size()>count)controls.removeLast();
  while(controls.size()<count){int i=controls.size();auto* control=controls.add(new OscKnob(p,"macro"+String(i+1),"MACRO "+String(i+1)));list.addAndMakeVisible(control);}
  boundExtra=state->extra;
  int width=getWidth()-((count+1)*66>viewport.getHeight()?5:0);list.setSize(width,(count+1)*66);
  for(int i=0;i<controls.size();++i){controls[i]->setBounds(0,i*66,width,65);controls[i]->refreshMacroLabel();controls[i]->repaint();}
  add.setBounds(0,count*66,width,66);list.repaint();
 }
};
}
