#pragma once
#include <NexumDesign.h>
#include <NexumHeaderOutput.h>
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
 void paint(juce::Graphics& g)override{nd::Scope suitScope("meter");using namespace juce;if(!joinedHeader){g.fillAll(UiStyle::canvas());UiStyle::card(g,getLocalBounds().toFloat());}auto text=[&](String s,Rectangle<int> r,Colour c){g.setColour(nd::colour(c));g.setFont(FontOptions(9.f,Font::bold));g.drawText(s,r,Justification::centred,false);};text("MASTER",{1,44,56,11},nd::colour(Colour(0xffbecfdb)));text(p.value("output")<=0?String("-inf dB"):String(Decibels::gainToDecibels(p.value("output")),1)+" dB",{0,54,58,10},nd::colour(Colour(NEXUM_UI_ACCENT)));nexum::family::drawHeaderMeters(g,60,{p.masterMeters[0].load(),p.masterMeters[1].load()},Colour(NEXUM_UI_ACCENT),getHeight());}
};
}
