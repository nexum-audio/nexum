#pragma once
#include <NexumDesign.h>
#include <juce_gui_basics/juce_gui_basics.h>
namespace noctrl {
// Shared visual language for every page. No DSP or parameter mapping lives here.
struct UiStyle {
 static constexpr float radius=4.f;
 static juce::Colour canvas(){return nd::colour(juce::Colour(0xff12191d));}
 static juce::Colour surface(){return nd::colour(juce::Colour(0xff1e2b32));}
 static juce::Colour headerColour(){return nd::colour(juce::Colour(0xff243442));}
 static juce::Colour blue(){return nd::colour(juce::Colour(NEXUM_UI_ACCENT));}
 static juce::Colour moduleSurface(){return nd::colour(juce::Colour(0xff91a3b0));}
 static juce::Colour moduleHeader(){return nd::colour(juce::Colour(0xffa7b9c5));}
 static juce::Colour filterSurface(){return nd::colour(juce::Colour(0xff25313a));}
 static juce::Colour filterHeader(){return nd::colour(juce::Colour(0xff303f49));}
 static juce::Colour moduleInk(){return nd::colour(juce::Colour(0xff1b3040));}
 static void moduleCard(juce::Graphics& g,juce::Rectangle<float> r){nd::Scope suitScope("module");g.setColour(nd::colour(moduleSurface()));g.fillRoundedRectangle(r,nd::number("radius",radius));g.setColour(nd::colour(nd::colour(juce::Colour(0xff657d8e))));g.drawRoundedRectangle(r.reduced(.5f),nd::number("radius",radius-.5f),.7f);}
 static void header(juce::Graphics& g,juce::Rectangle<float> r,juce::String title={},bool selected=false){nd::Scope suitScope("header");juce::Path path;path.addRoundedRectangle(r.getX(),r.getY(),r.getWidth(),r.getHeight(),nd::number("radius",radius),nd::number("radius",radius),true,true,false,false);g.setColour(nd::colour(nd::colour(juce::Colour(selected?0xff344b5d:0xff243442))));g.fillPath(path);g.setColour(nd::colour(nd::colour(juce::Colour(0xff486074))));g.drawHorizontalLine(juce::roundToInt(r.getBottom()-1),r.getX(),r.getRight());if(title.isNotEmpty()){g.setColour(nd::colour(blue()));g.setFont(juce::FontOptions(nd::number("fontSize",11.5f),juce::Font::bold));g.drawText(title,r.reduced(12,0),juce::Justification::centredLeft,false);}}
 static void card(juce::Graphics& g,juce::Rectangle<float> r,juce::String title={},float heading=28){nd::Scope suitScope("card");g.setColour(nd::colour(surface()));g.fillRoundedRectangle(r,nd::number("radius",radius));if(title.isNotEmpty())header(g,r.withHeight(heading),title);g.setColour(nd::colour(nd::colour(juce::Colour(0xff344650)).withAlpha(.65f)));g.drawRoundedRectangle(r.reduced(.5f),nd::number("radius",radius-.5f),.7f);}
 static void maskCorners(juce::Graphics& g,juce::Rectangle<float> r,juce::Colour outside=canvas()){juce::Path path;path.setUsingNonZeroWinding(false);path.addRectangle(r);path.addRoundedRectangle(r,nd::number("radius",radius));g.setColour(nd::colour(outside));g.fillPath(path);}
 static void plot(juce::Graphics& g,juce::Rectangle<float> r){nd::Scope suitScope("curve");g.setGradientFill(juce::ColourGradient(nd::colour(juce::Colour(0xff15242c)),r.getCentreX(),r.getY(),nd::colour(juce::Colour(0xff0c171e)),r.getCentreX(),r.getBottom(),false));g.fillRect(r);}
 static void fillCurve(juce::Graphics& g,const juce::Path& line,juce::Rectangle<float> r,float baseline,juce::Colour colour=blue()){nd::Scope suitScope("curve");if(line.isEmpty())return;auto area=line;area.lineTo(line.getBounds().getRight(),baseline);area.lineTo(line.getBounds().getX(),baseline);area.closeSubPath();g.setGradientFill(juce::ColourGradient(colour.withAlpha(nd::number("fillAlpha",.27f)),r.getCentreX(),r.getY(),colour.withAlpha(.09f),r.getCentreX(),r.getBottom(),false));g.fillPath(area);}
};
}
