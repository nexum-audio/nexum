#pragma once
#include <juce_audio_utils/juce_audio_utils.h>

namespace nexum::family {
// Family output controls use the synth header's knob / caption / value order.
class HeaderControl : public juce::Component {
    juce::Slider& slider;
    juce::Label caption, value;
public:
    HeaderControl(juce::Slider& control, juce::String name, juce::Colour accent) : slider(control) {
        setComponentID("header-" + name.toLowerCase());
        addAndMakeVisible(slider);
        addAndMakeVisible(caption);
        addAndMakeVisible(value);
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        slider.setRotaryParameters(juce::MathConstants<float>::pi * 1.24f,
                                   juce::MathConstants<float>::pi * 2.76f, true);
        for (auto* label : { &caption, &value }) {
            label->setFont(juce::FontOptions(9.f, juce::Font::bold));
            label->setJustificationType(juce::Justification::centred);
            label->setBorderSize(juce::BorderSize<int>(0));
            label->setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack);
        }
        caption.setText(name, juce::dontSendNotification);
        caption.setColour(juce::Label::textColourId, juce::Colour(0xffbecfdb));
        caption.setInterceptsMouseClicks(false, false);
        value.setColour(juce::Label::textColourId, accent);
        value.setColour(juce::Label::textWhenEditingColourId, accent);
        value.setColour(juce::Label::backgroundWhenEditingColourId, juce::Colour(0xff0a141c));
        value.setEditable(false, true);
        value.onTextChange = [this] {
            slider.setValue(slider.getValueFromText(value.getText()), juce::sendNotificationSync);
            refresh();
        };
        refresh();
    }
    void refresh() {
        if (!value.isBeingEdited())
            value.setText(slider.getTextFromValue(slider.getValue()), juce::dontSendNotification);
    }
    void resized() override {
        slider.setBounds(0, 0, getWidth(), 44);
        caption.setBounds(0, 44, getWidth(), 11);
        value.setBounds(0, 54, getWidth(), 10);
    }
};

// Stop and global bypass share the rack routing symbol and dark icon treatment.
class HeaderAction : public juce::Button {
    bool stop;
    juce::Colour accent;
public:
    HeaderAction(bool isStop, juce::Colour colour) : Button(isStop ? "Stop / Panic" : "Bypass"), stop(isStop), accent(colour) {
        setClickingTogglesState(!stop);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
        setTooltip(stop ? "Stop all voices / Panic" : "Bypass the complete effect chain");
    }
    void paintButton(juce::Graphics& g, bool over, bool down) override {
        using namespace juce;
        g.fillAll(Colour(0xff17212a));
        g.setColour(Colour(0xff344650));
        g.drawVerticalLine(0, 0.f, (float)getHeight());
        g.drawVerticalLine(getWidth()-1, 0.f, (float)getHeight());
        auto button=Rectangle<float>(getWidth()*.5f-14, getHeight()*.5f-14, 28, 28);
        g.setColour(Colour(down?0xff09121a:over?0xff182e3d:0xff0c1822));
        g.fillRoundedRectangle(button,2.f);
        float cx=button.getCentreX(),cy=button.getCentreY()+2;
        auto colour=(!stop&&getToggleState())||over?accent:Colour(0xff899aa5);
        if(stop){g.setColour(colour);g.fillRect(cx-5,cy-7,10.f,10.f);return;}
        Path route;route.startNewSubPath(cx-8,cy);route.lineTo(cx-3,cy);
        route.startNewSubPath(cx+3,cy);route.lineTo(cx+8,cy);
        route.startNewSubPath(cx-6,cy);route.cubicTo(cx-6,cy-8,cx+6,cy-8,cx+6,cy);
        route.startNewSubPath(cx+6,cy-2);route.lineTo(cx+8,cy);route.lineTo(cx+6,cy+2);
        route.addEllipse(cx-2,cy-2,4,4);
        if(getToggleState()){g.setColour(accent.withAlpha(.2f));g.strokePath(route,PathStrokeType(3.6f,PathStrokeType::curved,PathStrokeType::rounded));}
        g.setColour(colour);g.strokePath(route,PathStrokeType(1.4f,PathStrokeType::curved,PathStrokeType::rounded));
    }
};

inline void drawHeaderMeters(juce::Graphics& g, int x, const std::array<float, 2>& peaks,
                             juce::Colour accent, int height=66) {
    using namespace juce;
    for (int c = 0; c < 2; ++c) {
        const int left = x + c * 18;
        const float db = Decibels::gainToDecibels(peaks[c], -90.f);
        g.setColour(Colour(0xff0a141c));
        g.fillRect(left,0,17,height);
        const float level = jlimit(0.f, 1.f, (db + 60) / 60) * height;
        g.setColour(db >= -.1f ? Colour(0xffff745b) : accent);
        g.fillRect((float)left,height-level,17.f,level);
        g.setFont(FontOptions(7.f,Font::bold));
        g.setColour(Colour(0xffdce9f5));
        g.drawText(c == 0 ? "L" : "R",left,2,17,10,Justification::centred);
        g.setFont(FontOptions(6.5f,Font::bold));
        g.drawText(db <= -90 ? "-inf" : String(db,1),left,height-12,17,10,Justification::centred);
    }
}
}
