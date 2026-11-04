#pragma once
#include <NexumDesign.h>
namespace nexum::family {
inline juce::Colour sidebarAccent(const juce::Component& c){return c.getProperties().contains("sidebarAccent")?juce::Colour((juce::uint32)(int)c.getProperties()["sidebarAccent"]):juce::Colour(NEXUM_UI_ACCENT);}
inline void drawSidebarTabText(juce::Graphics& g,juce::TextButton& button,bool over){using namespace juce;g.setColour((button.getToggleState()||over?sidebarAccent(button):Colour(0xff91a7b9)));g.setFont(FontOptions(11.5f,Font::bold));g.drawText(button.getButtonText(),button.getLocalBounds().withTrimmedLeft(18).withTrimmedRight(2),Justification::centredLeft,false);}
class SidebarTab:public juce::TextButton {
public:
 explicit SidebarTab(juce::String text):TextButton(text){}
 void paintButton(juce::Graphics& g,bool over,bool)override{using namespace juce;bool active=getToggleState();if(over){g.setColour(Colour(0xff20323e));g.fillAll();}if(active){g.setColour(sidebarAccent(*this));g.fillRect(0,7,3,getHeight()-14);}drawSidebarTabText(g,*this,over);g.setColour(Colour(0xff30414e));g.drawHorizontalLine(getHeight()-1,0.f,(float)getWidth());}
};
class SidebarAdd:public juce::Button {
public:
 SidebarAdd():Button("Add macro"){setComponentID("macro-add");setTooltip("Neues Makro hinzufuegen");}
 void paintButton(juce::Graphics& g,bool over,bool down)override{using namespace juce;if(over||down){g.setColour(sidebarAccent(*this).withAlpha(.1f));g.fillRect(getLocalBounds());}g.setColour(sidebarAccent(*this));float x=getWidth()*.5f,y=getHeight()*.5f;g.drawLine(x-6,y,x+6,y,1.8f);g.drawLine(x,y-6,x,y+6,1.8f);g.setColour(Colour(0xff30414e));g.drawHorizontalLine(getHeight()-1,0.f,(float)getWidth());}
};
class SidebarList:public juce::Component {
 void paint(juce::Graphics& g)override{g.setColour(juce::Colour(0xff30414e));for(int y=65;y<getHeight();y+=66)g.drawHorizontalLine(y,0.f,(float)getWidth());}
};
inline void paintMacroTile(juce::Graphics& g,int width,int height){g.setColour(juce::Colour(0xff0c1620));g.fillRect(0,height-15,width,15);g.setColour(juce::Colour(0xff30414e));g.drawVerticalLine(width-28,0.f,(float)height-15);}
class MacroKnobTheme:public juce::LookAndFeel_V4 {
public:
 void drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float value,float start,float end,juce::Slider& slider)override{nd::Scope suitScope("knob");using namespace juce;float r=15.f,cx=x+w*.5f,cy=y+h*.5f,k=r-4;auto accent=slider.getProperties().contains("macroColour")?Colour((juce::uint32)(int)slider.getProperties()["macroColour"]):slider.getProperties().contains("fxAccent")?Colour((juce::uint32)(int)slider.getProperties()["fxAccent"]):Colour(NEXUM_UI_ACCENT);if(!slider.isEnabled())accent=accent.withAlpha(.35f);
  Path track;track.addCentredArc(cx,cy,r,r,0,start,end,true);g.setColour(nd::colour(nd::colour(Colour(0xff0a1017))));g.strokePath(track,PathStrokeType(nd::number("stroke",3.5f)));Path active;active.addCentredArc(cx,cy,r,r,0,start,start+value*(end-start),true);g.setColour(nd::colour(accent));g.strokePath(active,PathStrokeType(nd::number("stroke",2.2f)));
  g.setColour(nd::colour(Colours::black.withAlpha(.45f)));g.fillEllipse(cx-k-1,cy-k+3,2*k+2,2*k+2);
  g.setGradientFill(ColourGradient(nd::colour(Colour(0xff8b9cab)),cx-k,cy-k,nd::colour(Colour(0xff0a1018)),cx+k,cy+k,false));g.fillEllipse(cx-k,cy-k,k*2,k*2);
  float face=jmax(1.f,k-2);g.setGradientFill(ColourGradient(nd::colour(Colour(0xff526779)),cx-face*.65f,cy-face,nd::colour(Colour(0xff17232f)),cx+face*.6f,cy+face,false));g.fillEllipse(cx-face,cy-face,face*2,face*2);
  Path shine;shine.addCentredArc(cx,cy,face-.5f,face-.5f,0,-1.1f,.75f,true);g.setColour(nd::colour(Colours::white.withAlpha(.22f)));g.strokePath(shine,PathStrokeType(nd::number("stroke",.8f)));
  float angle=start+value*(end-start);float sx=cx+std::sin(angle)*face*.36f,sy=cy-std::cos(angle)*face*.36f,ex=cx+std::sin(angle)*face*.83f,ey=cy-std::cos(angle)*face*.83f;g.setColour(nd::colour(Colours::black.withAlpha(.6f)));g.drawLine(sx+1,sy+1,ex+1,ey+1,2.5f);g.setColour(nd::colour(nd::colour(Colour(0xffe5f4ff))));g.drawLine(sx,sy,ex,ey,1.8f);
 }

};
class SidebarFrame:public juce::Component {
 juce::Image logo;juce::String suffix,version;int tabs;
protected:
 SidebarAdd add;juce::Viewport viewport;SidebarList list;
 int macroTop()const{return 67+tabs*38;}
public:
 SidebarFrame(int tabCount,juce::String productSuffix,juce::String productVersion):suffix(productSuffix),version(productVersion),tabs(tabCount){setComponentID("nexum-sidebar");addAndMakeVisible(viewport);viewport.setComponentID("macro-viewport");viewport.setViewedComponent(&list,false);viewport.setScrollBarsShown(true,false);viewport.setScrollBarThickness(5);list.addAndMakeVisible(add);}
 ~SidebarFrame()override{viewport.setViewedComponent(nullptr,false);}
 void setAccent(juce::Colour colour){getProperties().set("sidebarAccent",(int)colour.getARGB());add.getProperties().set("sidebarAccent",(int)colour.getARGB());repaint();}
 void setLogo(juce::Image image){logo=image;repaint();}
 virtual void refreshMacros(){}
 void resized()override{viewport.setBounds(0,macroTop(),getWidth(),juce::jmax(0,getHeight()-25-macroTop()));refreshMacros();}
 void paint(juce::Graphics& g)override{using namespace juce;g.fillAll(Colour(0xff17242d));g.setColour(Colour(0xff17242d));g.fillRect(0,0,getWidth(),66);g.setImageResamplingQuality(Graphics::highResamplingQuality);if(logo.isValid())g.drawImageWithin(logo,8,0,getWidth()-16,66,RectanglePlacement::centred);if(suffix.isNotEmpty()&&!getProperties().getWithDefault("completeBrandLogo",false)){g.setColour(Colour(0xffaebfc9));g.setFont(FontOptions(10.f,Font::bold));g.drawText(suffix,0,39,getWidth(),20,Justification::centred);}g.setColour(Colour(0xff344650));g.drawVerticalLine(getWidth()-1,0.f,(float)getHeight());g.drawHorizontalLine(66,0.f,(float)getWidth());g.setColour(Colour(0xff30414e));g.drawHorizontalLine(getHeight()-25,0.f,(float)getWidth());g.setColour(Colour(0xff8ba0ae));g.setFont(FontOptions(9.f));g.drawText("v"+version,0,getHeight()-24,getWidth()-1,23,Justification::centred);}
};
}

namespace nxf = nexum::family;
