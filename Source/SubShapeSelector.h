#pragma once
#include <NexumDesign.h>
#include "Processor.h"
namespace noctrl {
class SubShapeButton:public juce::Button {
 int shape;
public:
 SubShapeButton(int value,juce::String name):Button(name),shape(value){setComponentID("sub-shape:"+juce::String(value));setTooltip(name);setTitle(name);setRadioGroupId(709,juce::dontSendNotification);setClickingTogglesState(true);setMouseCursor(juce::MouseCursor::PointingHandCursor);}
 void paintButton(juce::Graphics& g,bool over,bool down)override{nd::Scope suitScope("waveform");using namespace juce;auto blue=nd::colour(Colour(NEXUM_UI_ACCENT));bool selected=getToggleState();auto area=getLocalBounds().toFloat().reduced(7,9);Path wave;for(int i=0;i<128;++i){float t=i/127.f,value=subWaveform(shape,t==1.f?.999999:t),x=area.getX()+t*area.getWidth(),y=area.getCentreY()-value*area.getHeight()*.43f;if(i==0)wave.startNewSubPath(x,y);else wave.lineTo(x,y);}if(selected){g.setColour(blue.withAlpha(.14f));g.strokePath(wave,PathStrokeType(4.f));}g.setColour(selected?blue:nd::colour(Colour(over||down?0xffb6d2e4:0xff6d8794)));g.strokePath(wave,PathStrokeType(selected?1.8f:1.3f));}

};
class SubShapeSelector:public juce::Component {
 SynthProcessor& processor;std::array<std::unique_ptr<SubShapeButton>,6> buttons;
public:
 explicit SubShapeSelector(SynthProcessor& p):processor(p){setComponentID("sub-shape-selector");const int shapes[]={0,4,1,2,3,5};const char* names[]={"Sine","Rounded Square","Triangle","Saw","Square","Pulse 25%"};for(int i=0;i<6;++i){buttons[i]=std::make_unique<SubShapeButton>(shapes[i],names[i]);addAndMakeVisible(*buttons[i]);buttons[i]->onClick=[this,shape=shapes[i]]{auto* parameter=processor.state.getParameter("subShape");parameter->beginChangeGesture();parameter->setValueNotifyingHost(parameter->convertTo0to1((float)shape));parameter->endChangeGesture();refresh();};}refresh();}
 void refresh(){int shape=(int)processor.value("subShape");for(auto& button:buttons)button->setToggleState(button->getComponentID()=="sub-shape:"+juce::String(shape),juce::dontSendNotification);}
 void paint(juce::Graphics& g)override{g.setColour(nd::colour(juce::Colour(0xff304151)));g.drawVerticalLine(getWidth()/2,0.f,(float)getHeight());for(int row=1;row<3;++row)g.drawHorizontalLine(row*getHeight()/3,0.f,(float)getWidth());}
 void resized()override{for(int i=0;i<6;++i){int col=i%2,row=i/2,x=col*getWidth()/2,y=row*getHeight()/3;buttons[i]->setBounds(x,y,(col+1)*getWidth()/2-x,(row+1)*getHeight()/3-y);}}
};
}
