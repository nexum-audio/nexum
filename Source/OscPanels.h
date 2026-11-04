#pragma once
#include <NexumDesign.h>
#include "AutomationSlider.h"
#include "OscAdvanced.h"
namespace noctrl {
// Reusable parameter inspector: all edits go through host parameters and retain modulation support.
class OscParameterPanel:public juce::Component {
 juce::OwnedArray<juce::Label> labels;juce::OwnedArray<AutomationSlider> sliders;juce::OwnedArray<juce::ComboBox> choices;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> links;
 std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> comboLinks;
public:
 OscParameterPanel(SynthProcessor& p,int osc,juce::String title,juce::StringArray fields){using namespace juce;setName(title);int rows=(fields.size()+1)/2;setSize(480,44+rows*55);auto* heading=labels.add(new Label());addAndMakeVisible(heading);heading->setText(title,dontSendNotification);heading->setFont(FontOptions(nd::number("fontSize",11.5f),Font::bold));heading->setColour(Label::textColourId,nd::colour(Colour(NEXUM_UI_ACCENT)));heading->setBounds(12,6,456,29);
  for(int i=0;i<fields.size();++i){auto id="osc"+String(osc+1)+fields[i];auto* parameter=p.state.getParameter(id);if(!parameter)continue;int x=12+(i%2)*234,y=40+(i/2)*55;auto* label=labels.add(new Label());addAndMakeVisible(label);label->setText(parameter->getName(50),dontSendNotification);label->setFont(FontOptions(nd::number("fontSize",11)));label->setBounds(x,y,224,19);
   if(auto* choice=dynamic_cast<AudioParameterChoice*>(parameter)){auto* box=choices.add(new ComboBox());addAndMakeVisible(box);box->addItemList(choice->choices,1);box->setBounds(x,y+20,222,25);box->setComponentID("detail:"+id);comboLinks.push_back(std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,id,*box));}
   else{auto* slider=sliders.add(new AutomationSlider());addAndMakeVisible(slider);slider->bindAutomation(parameter,&p);slider->setSliderStyle(Slider::LinearHorizontal);slider->setTextBoxStyle(Slider::TextBoxRight,false,64,23);slider->setBounds(x,y+20,222,25);slider->setComponentID("detail:"+id);slider->textFromValueFunction=[](double v){return String(v,std::abs(v)>=100?0:3);};links.push_back(std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.state,id,*slider));}
  }
 }
 void paint(juce::Graphics& g)override{nd::Scope suitScope("controls");g.fillAll(UiStyle::canvas());UiStyle::card(g,getLocalBounds().toFloat());UiStyle::header(g,getLocalBounds().toFloat().withHeight(36));}
};
class OscRemapPanel:public juce::Component {
 SynthProcessor& p;int osc,warp;juce::TextButton reset{"RESET"};juce::String id;int last=-1;
 juce::Rectangle<float> plot()const{return {14,38,(float)getWidth()-28,(float)getHeight()-52};}
 void draw(juce::Point<float> at){auto r=plot();int point=juce::jlimit(0,15,juce::roundToInt((at.x-r.getX())/r.getWidth()*15));float v=juce::jlimit(0.f,1.f,(r.getBottom()-at.y)/r.getHeight());int from=last<0?point:last;float start=p.value(id+juce::String(from+1));for(int k=juce::jmin(from,point);k<=juce::jmax(from,point);++k)p.set(id+juce::String(k+1),from==point?v:start+(v-start)*(k-from)/(point-from));last=point;repaint();}
public:
 OscRemapPanel(SynthProcessor& processor,int index,int slot,juce::String custom={}):p(processor),osc(index),warp(slot),id("osc"+juce::String(index+1)+"warp"+juce::String(slot+1)+"remap"){if(custom.isNotEmpty())id=custom;setSize(420,230);addAndMakeVisible(reset);reset.setBounds(330,7,76,24);reset.onClick=[this]{for(int k=0;k<16;++k)p.set(id+juce::String(k+1),id.contains("spectralPoint")?1.f:k/15.f);repaint();};}
 void paint(juce::Graphics& g)override{nd::Scope suitScope("controls");using namespace juce;g.fillAll(UiStyle::canvas());UiStyle::card(g,getLocalBounds().toFloat());UiStyle::header(g,getLocalBounds().toFloat().withHeight(34));g.setColour(nd::colour(nd::colour(Colour(NEXUM_UI_ACCENT))));g.setFont(FontOptions(nd::number("fontSize",11.5f),Font::bold));g.drawText(id.contains("spectralPoint")?String("SPECTRAL FILTER / 20 Hz - 20 kHz"):"WARP "+String(warp+1)+" / REMAP",14,5,300,28,Justification::centredLeft);auto r=plot();UiStyle::plot(g,r);Path path;for(int k=0;k<16;++k){float x=r.getX()+k/15.f*r.getWidth(),y=r.getBottom()-p.value(id+String(k+1))*r.getHeight();if(k==0)path.startNewSubPath(x,y);else path.lineTo(x,y);g.setColour(nd::colour(nd::colour(Colour(NEXUM_UI_ACCENT))));g.fillEllipse(x-2,y-2,4,4);}UiStyle::fillCurve(g,path,r,r.getBottom());g.setColour(nd::colour(UiStyle::blue()));g.strokePath(path,PathStrokeType(nd::number("stroke",1.5f)));}
 void mouseDown(const juce::MouseEvent& e)override{if(e.mods.isPopupMenu())return;last=-1;draw(e.position);}
 void mouseDrag(const juce::MouseEvent& e)override{if(!e.mods.isPopupMenu())draw(e.position);}
 void mouseUp(const juce::MouseEvent&)override{last=-1;}
};
inline void showOscPanel(SynthProcessor& p,int osc,juce::Component* anchor,juce::String title,juce::StringArray ids){juce::CallOutBox::launchAsynchronously(std::make_unique<OscParameterPanel>(p,osc,title,ids),anchor->getScreenBounds(),nullptr);}
}
