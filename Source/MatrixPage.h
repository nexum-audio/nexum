#pragma once
#include "Processor.h"
#include "UiStyle.h"
namespace noctrl {
class MatrixPage : public juce::Component, private juce::Timer {
 SynthProcessor& p;
 struct Row : juce::Component {
  juce::Label kind;juce::ComboBox source,target;juce::Slider start,end;juce::TextButton remove{"X"};
  std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> srcLink,dstLink;
  std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountLink;
  int type=0,index=0;juce::String id;
  Row(){for(auto* c:std::initializer_list<juce::Component*>{&kind,&source,&target,&start,&end,&remove})addAndMakeVisible(c);for(auto* s:{&start,&end}){s->setSliderStyle(juce::Slider::LinearHorizontal);s->setTextBoxStyle(juce::Slider::TextBoxRight,false,65,24);s->setRange(0,1,.001);s->textFromValueFunction=[](double v){return juce::String(v*100,1)+" %";};s->valueFromTextFunction=[](const juce::String& t){return t.getDoubleValue()/100;};s->setColour(juce::Slider::thumbColourId,UiStyle::blue());}kind.setColour(juce::Label::textColourId,juce::Colour(0xff9eb1c0));remove.setTooltip("Remove this modulation connection");}
  void resized()override{int w=getWidth();kind.setBounds(12,8,92,30);source.setBounds(108,10,170,28);target.setBounds(290,10,w-850,28);start.setBounds(w-548,10,234,28);end.setBounds(w-302,10,234,28);remove.setBounds(w-48,10,32,28);}
  void paint(juce::Graphics& g)override{g.setColour(juce::Colour(0xff344650));g.drawHorizontalLine(getHeight()-1,12.f,(float)getWidth()-12);}
 };
 juce::Viewport viewport;juce::Component content;juce::OwnedArray<Row> rows;
 juce::ComboBox newSource,newTarget;juce::TextButton add{"ADD CONNECTION"};juce::Label status;
 juce::StringArray targetIDs;juce::String signature;
 void sources(juce::ComboBox& box){box.clear(juce::dontSendNotification);for(int i=0;i<liveSourceCount;++i)box.addItem(liveSourceName(i),i+1);auto m=p.macros();for(int i=0;i<p.visibleMacroCount();++i)box.addItem("Macro "+juce::String(i+1)+(m->names[i].isEmpty()?"":" / "+m->names[i]),1000+i);}
 juce::String targetName(const juce::String& id,juce::RangedAudioParameter& parameter){auto name=parameter.getName(100);for(auto prefix:{juce::String("filter"),juce::String("env"),juce::String("lfo")})if(id.startsWith(prefix)){int n=id.substring(prefix.length()).getIntValue();if(n>0)return (prefix=="filter"?"Filter ":prefix=="env"?"ENV ":"LFO ")+juce::String(n)+" / "+name;}return name;}
 void targets(juce::ComboBox& box){box.clear(juce::dontSendNotification);for(int i=0;i<targetIDs.size();++i)if(auto* parameter=p.state.getParameter(targetIDs[i]))box.addItem(targetName(targetIDs[i],*parameter),i+1);}
 bool create(int source,const juce::String& target){if(source>=1000){if(p.macros()->find(target))return false;return p.assignMacro(source-1000,target);}if(p.liveMods()->find(source-1,target))return false;return p.assignLiveMod(source-1,target);}
 void erase(Row& row){if(row.type==1)p.removeLiveMod(row.index,row.id);else if(row.type==2)p.removeMacroLink(row.id);else {auto id="mod"+juce::String(row.index+1);p.set(id+"src",0);p.set(id+"dst",0);p.set(id+"amt",0);}}
 void dynamic(Row& row,int type,int source,const juce::String& id){
  row.setComponentID("matrix-link:"+juce::String(type)+":"+juce::String(source)+":"+id);row.type=type;row.index=source;row.id=id;row.kind.setText(type==1?"Live":"Macro",juce::dontSendNotification);sources(row.source);targets(row.target);row.source.setSelectedId(type==1?source+1:1000+source,juce::dontSendNotification);row.target.setSelectedId(targetIDs.indexOf(id)+1,juce::dontSendNotification);
  auto change=[this,&row]{int sourceID=row.source.getSelectedId();auto targetID=targetIDs[row.target.getSelectedId()-1];if((row.type==1?row.index+1:1000+row.index)==sourceID&&row.id==targetID)return;float a=(float)row.start.getValue(),b=(float)row.end.getValue();if(row.type==2&&sourceID>=1000&&targetID==row.id){p.assignMacro(sourceID-1000,targetID);row.index=sourceID-1000;return;}if(create(sourceID,targetID)){erase(row);if(sourceID>=1000)p.setMacroRange(targetID,a,b);else p.setLiveModRange(sourceID-1,targetID,a,b);row.type=sourceID>=1000?2:1;row.index=sourceID>=1000?sourceID-1000:sourceID-1;row.id=targetID;}else {row.source.setSelectedId(row.type==1?row.index+1:1000+row.index,juce::dontSendNotification);row.target.setSelectedId(targetIDs.indexOf(row.id)+1,juce::dontSendNotification);status.setText("Connection already exists or the destination is unavailable.",juce::dontSendNotification);}};
  row.source.onChange=change;row.target.onChange=change;
  auto range=[this,&row]{if(row.type==1)p.setLiveModRange(row.index,row.id,(float)row.start.getValue(),(float)row.end.getValue());else p.setMacroRange(row.id,(float)row.start.getValue(),(float)row.end.getValue());};row.start.onValueChange=range;row.end.onValueChange=range;
  row.remove.onClick=[this,&row]{erase(row);};
 }
 void refresh(){
  auto live=p.liveMods();auto macro=p.macros();juce::String key=juce::String(p.visibleMacroCount());for(auto& name:macro->names)key+="|"+name;for(auto& link:live->links)key+="L"+juce::String(link.source)+":"+link.target+";";for(auto& link:macro->links)key+="M"+juce::String(link.macro)+":"+link.target+";";
  if(key!=signature){signature=key;rows.clear();sources(newSource);
   for(auto& link:live->links){auto* row=rows.add(new Row);content.addAndMakeVisible(row);dynamic(*row,1,link.source,link.target);}
   for(auto& link:macro->links){auto* row=rows.add(new Row);content.addAndMakeVisible(row);dynamic(*row,2,link.macro,link.target);}
   for(int i=0;i<8;++i){auto* row=rows.add(new Row);content.addAndMakeVisible(row);row->setComponentID("matrix-slot:"+juce::String(i+1));row->index=i;row->kind.setText("Slot "+juce::String(i+1),juce::dontSendNotification);auto id="mod"+juce::String(i+1);for(auto pair:{std::make_pair(&row->source,id+"src"),std::make_pair(&row->target,id+"dst")}){auto* parameter=dynamic_cast<juce::AudioParameterChoice*>(p.state.getParameter(pair.second));pair.first->addItemList(parameter->choices,1);}row->srcLink=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,id+"src",row->source);row->dstLink=std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,id+"dst",row->target);row->start.setRange(-1,1,.001);row->amountLink=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,id+"amt",row->start);row->start.textFromValueFunction=[](double v){return juce::String(v*100,1)+" %";};row->start.valueFromTextFunction=[](const juce::String& text){return text.getDoubleValue()/100;};row->start.updateText();row->end.setVisible(false);row->remove.onClick=[this,row]{erase(*row);};}resized();
  }
  for(auto* row:rows)if(row->type){float a=0,b=0;bool found=false;if(row->type==1){if(auto* link=live->find(row->index,row->id)){a=link->start;b=link->end;found=true;}}else if(auto* link=macro->find(row->id)){a=link->start;b=link->end;found=true;}if(found){if(!row->start.isMouseButtonDown(true))row->start.setValue(a,juce::dontSendNotification);if(!row->end.isMouseButtonDown(true))row->end.setValue(b,juce::dontSendNotification);}}
  repaint();
 }
 void timerCallback()override{if(isShowing())refresh();}
public:
 explicit MatrixPage(SynthProcessor& processor):p(processor){setComponentID("modulation-matrix");newSource.setComponentID("matrix-new-source");newTarget.setComponentID("matrix-new-target");add.setComponentID("matrix-add");for(auto* c:std::initializer_list<juce::Component*>{&viewport,&newSource,&newTarget,&add,&status})addAndMakeVisible(c);viewport.setViewedComponent(&content,false);viewport.setScrollBarsShown(true,false);for(auto* base:p.getParameters())if(auto* parameter=dynamic_cast<juce::RangedAudioParameter*>(base))if(!dynamic_cast<juce::AudioParameterBool*>(parameter)&&!dynamic_cast<juce::AudioParameterChoice*>(parameter)){auto id=parameter->getParameterID();if(!id.startsWith("mod"))targetIDs.add(id);}targets(newTarget);newSource.setTextWhenNothingSelected("Choose a source");newTarget.setTextWhenNothingSelected("Choose a destination");status.setText("Live and macro ranges use 0-100% of the destination. Slots use a signed amount.",juce::dontSendNotification);status.setColour(juce::Label::textColourId,juce::Colour(0xff9eb1c0));add.onClick=[this]{int s=newSource.getSelectedId(),t=newTarget.getSelectedId();if(s<=0||t<=0){status.setText("Choose a source and destination first.",juce::dontSendNotification);return;}status.setText(create(s,targetIDs[t-1])?"Connection added.":"Connection already exists or the destination is unavailable.",juce::dontSendNotification);};refresh();startTimerHz(8);}
 ~MatrixPage()override{stopTimer();viewport.setViewedComponent(nullptr,false);}
 void resized()override{int w=getWidth();newSource.setBounds(12,42,230,28);newTarget.setBounds(254,42,w-466,28);add.setBounds(w-200,42,188,28);status.setBounds(12,getHeight()-32,w-24,24);viewport.setBounds(0,118,w,getHeight()-154);content.setSize(w-16,rows.size()*50);int y=0;for(auto* row:rows){row->setBounds(0,y,w-16,50);y+=50;}}
 void paint(juce::Graphics& g)override{UiStyle::card(g,getLocalBounds().toFloat(),"MODULATION MATRIX");g.setColour(juce::Colour(0xff9eb1c0));g.setFont(juce::FontOptions(10.f,juce::Font::bold));int w=getWidth()-16;for(auto label:{std::make_pair("TYPE",12),std::make_pair("SOURCE",108),std::make_pair("DESTINATION",290),std::make_pair("AMOUNT / RANGE START",w-548),std::make_pair("RANGE END",w-302)})g.drawText(label.first,label.second,88,220,24,juce::Justification::centredLeft);}
};
}
