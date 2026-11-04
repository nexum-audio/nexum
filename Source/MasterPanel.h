#pragma once
#include <NexumDesign.h>
#include "UiStyle.h"
#include "AutomationSlider.h"
namespace noctrl {
class MasterPanel:public juce::Component {
 SynthProcessor& p;bool joinedHeader=false;
public:
 AutomationSlider gain;
private:
 std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
public:
 MasterPanel(SynthProcessor& processor):p(processor){setComponentID("master-panel");addAndMakeVisible(gain);gain.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);gain.setTextBoxStyle(juce::Slider::NoTextBox,false,0,0);gain.setRotaryParameters(juce::MathConstants<float>::pi*1.24f,juce::MathConstants<float>::pi*2.76f,true);auto* param=p.state.getParameter("output");gain.bindAutomation(param,&p);gain.setDoubleClickReturnValue(true,param->convertFrom0to1(param->getDefaultValue()));gain.setTooltip("Master output gain / right-click for automation. L/R: actual final output peak in dBFS.");gain.textFromValueFunction=[](double v){return v<=0?juce::String("-inf dB"):juce::String(juce::Decibels::gainToDecibels(v),1)+" dB";};gain.valueFromTextFunction=[](const juce::String& s){return s.containsIgnoreCase("inf")?0.:juce::Decibels::decibelsToGain(s.getDoubleValue());};attachment=std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state,"output",gain);getProperties().set("parameterIndex",param->getParameterIndex());}
 ~MasterPanel()override{attachment.reset();}
 void setJoinedHeader(bool joined){joinedHeader=joined;repaint();}
 void resized()override{gain.setBounds(0,0,58,44);}
 void paint(juce::Graphics& g)override{nd::Scope suitScope("meter");using namespace juce;if(!joinedHeader){g.fillAll(UiStyle::canvas());UiStyle::card(g,getLocalBounds().toFloat());}auto text=[&](String s,Rectangle<int> r,Colour c){g.setColour(nd::colour(c));g.setFont(FontOptions(9.f,Font::bold));g.drawText(s,r,Justification::centred,false);};text("MASTER",{1,44,56,11},nd::colour(Colour(0xffbecfdb)));text(p.value("output")<=0?String("-inf dB"):String(Decibels::gainToDecibels(p.value("output")),1)+" dB",{0,54,58,10},nd::colour(Colour(NEXUM_UI_ACCENT)));for(int c=0;c<2;++c){int x=72+c*39;float peak=p.masterMeters[c].load(std::memory_order_relaxed),db=Decibels::gainToDecibels(peak,-90.f);text(c==0?"L":"R",{x-7,0,24,10},nd::colour(Colour(0xff8ba0ae)));g.setColour(nd::colour(nd::colour(Colour(0xff0a141c))));g.fillRect(x,11,10,28);float height=jlimit(0.f,1.f,(db+60)/60)*28;g.setColour(nd::colour(db>=-.1f?nd::colour(Colour(0xffff745b)):Colour(NEXUM_UI_ACCENT)));g.fillRect((float)x,39-height,10.f,height);text(db<=-90?"-inf":String(db,1),{x-13,40,36,11},nd::colour(Colour(0xffbecfdb)));}}
};
}
