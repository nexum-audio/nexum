#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
namespace nexum::family {
class PresetHeaderStyle : public juce::LookAndFeel_V4 {
    juce::Colour accent;
public:
    explicit PresetHeaderStyle(juce::Colour colour):accent(colour){}
    void drawComboBox(juce::Graphics& g,int w,int h,bool,int,int,int,int,juce::ComboBox&)override {
        g.fillAll(juce::Colour(0xff17212a));juce::Path arrow;
        arrow.addTriangle(w-17.f,h*.5f-3,w-7.f,h*.5f-3,w-12.f,h*.5f+3);
        g.setColour(juce::Colour(0xff91a7b9));g.fillPath(arrow);
    }
    juce::Font getComboBoxFont(juce::ComboBox&)override{return juce::FontOptions(13.f,juce::Font::bold);}
    void drawButtonBackground(juce::Graphics& g,juce::Button& b,const juce::Colour&,bool over,bool down)override{
        g.fillAll(juce::Colour(down?0xff294455:over?0xff233543:0xff17212a));
        g.setColour(juce::Colour(0xff34444f));g.drawVerticalLine(b.getWidth()-1,7.f,(float)b.getHeight()-7);
    }
    void drawButtonText(juce::Graphics& g,juce::TextButton& b,bool over,bool)override{
        g.setColour(over?accent:juce::Colour(0xffa9becd));float cx=b.getWidth()*.5f,cy=b.getHeight()*.5f;
        if((bool)b.getProperties()["presetSave"]){g.drawRoundedRectangle(cx-6,cy-6,12,12,1,1.3f);g.drawRect(juce::Rectangle<float>(cx-3.5f,cy-6,6,5),1.1f);g.drawRect(juce::Rectangle<float>(cx-3.5f,cy+1,7,5),1.1f);}
        else{float dir=(float)(int)b.getProperties()["presetDirection"];juce::Path path;path.startNewSubPath(cx-dir*2,cy-4);path.lineTo(cx+dir*2,cy);path.lineTo(cx-dir*2,cy+4);g.strokePath(path,juce::PathStrokeType(1.8f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));}
    }
    void attach(juce::ComboBox& names,juce::TextButton& save,juce::TextButton& previous,juce::TextButton& next,juce::Label& artist,juce::Label& description){
        names.setLookAndFeel(this);names.setColour(juce::ComboBox::textColourId,accent);
        for(auto* b:{&save,&previous,&next})b->setLookAndFeel(this);
        save.getProperties().set("presetSave",true);previous.getProperties().set("presetDirection",-1);next.getProperties().set("presetDirection",1);
        save.setTooltip("Save as: Create new preset / Overwrite preset");previous.setTooltip("Previous preset");next.setTooltip("Next preset");
        save.setComponentID("preset-save-menu");previous.setComponentID("preset-previous");next.setComponentID("preset-next");
        for(auto* label:{&artist,&description}){label->setFont(juce::FontOptions(11.5f));label->setColour(juce::Label::textColourId,juce::Colour(0xffbecfdb));label->setInterceptsMouseClicks(false,false);}
    }
};
inline void layoutPresetHeader(int x,int width,juce::ComboBox& names,juce::TextButton& save,juce::TextButton& previous,juce::TextButton& next,juce::Label& artist,juce::Label& description){
    save.setBounds(x+1,1,33,34);names.setBounds(x+36,1,width-96,34);previous.setBounds(x+width-58,1,28,34);next.setBounds(x+width-28,1,28,34);
    artist.setBounds(x+53,39,135,23);description.setBounds(x+238,39,width-248,23);
}
inline void paintPresetHeader(juce::Graphics& g,int x,int width){
    using namespace juce;g.setColour(Colour(0xff17212a));g.fillRect(x,0,width,66);g.setColour(Colour(0xff1d2a34));g.fillRect(x,37,width,29);
    g.setColour(Colour(0xff080e14));g.drawHorizontalLine(36,(float)x,(float)(x+width));g.setColour(Colour(0xff344650));g.drawHorizontalLine(66,(float)x,(float)(x+width));
    g.setColour(Colour(0xff3d505e));g.drawVerticalLine(x+194,43.f,60.f);g.setColour(Colour(0xff7e94a4));g.setFont(FontOptions(10.f,Font::bold));g.drawText("ARTIST:",x+9,42,43,16,Justification::centredLeft);g.drawText("DESC:",x+205,42,33,16,Justification::centredLeft);
}
}
