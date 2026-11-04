#pragma once
#include <NexumDesign.h>
#include "Processor.h"
#include "Expansion.h"
namespace noctrl {
struct MixCableRouting {
 struct Edge {int from,to;bool send=false;};
 static juce::String sourceKey(int channel){return channel==0?"Sub":channel==4?"Noise":"Osc"+juce::String(channel);}
 static juce::String routeKey(int channel){return channel==0?"subRouting":channel==4?"noiseRouting":channel<4?"osc"+juce::String(channel)+"routing":channel<7?"filter"+juce::String(channel-4)+"route":"mixBus"+juce::String(channel-6)+"Route";}
 static bool allowed(int from,int to){return from>=0&&from<9&&to>=5&&to<11&&from!=to&&(from<7||to>=7);}
 static bool connect(SynthProcessor& p,int from,int to,bool add=false){
  if(!allowed(from,to))return false;auto id=routeKey(from);
  if(add&&to>=7&&to<=8&&from<7){if(from<5){int old=(int)p.value(id)-(from==0||from==4?1:0);if(old==to-1)return true;if(old==5)return connect(p,from,to,false);}p.set("mix"+(from<5?sourceKey(from):"Filter"+juce::String(from-4))+"Bus"+juce::String(to-6),1);return true;}
  if(from<5){int route=to==5?0:to==6?1:to==7?6:to==8?7:to==9?3:4;int offset=from==0||from==4?1:0;int old=(int)p.value(id)-offset;if(old<0){bool first=false,second=false;for(auto edge:edges(p))if(edge.from==from&&!edge.send){first|=edge.to==5;second|=edge.to==6;}old=first&&second?2:first?0:second?1:3;}if(add&&to<7&&(old==0||old==1||old==2)&&old!=route){route=2;p.set("mix"+sourceKey(from)+"Balance",.5f);}p.set(id,(float)(route+offset));}
  else if(from<7){if(to==5||to==6){p.set(routeKey(to),1);p.set(id,3);}else p.set(id,to==7?4.f:to==8?5.f:to==9?1.f:2.f);}
  else{if(to==7||to==8){p.set(routeKey(to),0);p.set(id,2);}else p.set(id,to==9?0.f:1.f);}
  return true;
 }
 static void disconnect(SynthProcessor& p,int channel){if(channel<0||channel>=7)return;p.set(routeKey(channel),channel==0||channel==4?6.f:channel<4?5.f:6.f);auto key=channel<5?sourceKey(channel):"Filter"+juce::String(channel-4);for(int bus=1;bus<=2;++bus)p.set("mix"+key+"Bus"+juce::String(bus),0);}
 static std::vector<Edge> edges(SynthProcessor& p){
  std::vector<Edge> result;auto assets=std::atomic_load(&p.importedAssets);
  for(int from=0;from<5;++from){int route=(int)p.value(routeKey(from))-(from==0||from==4?1:0);auto add=[&](int to){result.push_back({from,to,false});};
   if(route<0){int source=from==0?4:3;float f0=1,f1=0;if(assets&&assets->expansion&&!assets->userSource){f0=assets->expansion->filterRouting[0][source];f1=assets->expansion->filterRouting[1][source];}if(f0>0)add(5);if(f1>0)add(6);if(f0+f1<.999f)add(9);}
   else if(route==0)add(5);else if(route==1)add(6);else if(route==2){float balance=p.value("mix"+sourceKey(from)+"Balance");if(balance<1)add(5);if(balance>0)add(6);}else if(route!=5)add(route==3?9:route==4?10:route==6?7:8);
   if(route==5)continue;for(int bus=1;bus<=2;++bus)if(p.value("mix"+sourceKey(from)+"Bus"+juce::String(bus))>.0001f)result.push_back({from,6+bus,true});
  }
  int filters[2]={(int)p.value("filter1route"),(int)p.value("filter2route")};if(filters[0]==0)filters[0]=p.value("filterSerial")>.5f?3:1;if(filters[1]==0)filters[1]=1;if(filters[0]==3&&filters[1]==3)filters[1]=1;
  for(int f=0;f<2;++f){int route=filters[f];if(route!=6)result.push_back({5+f,route==3?6-f:route==4?7:route==5?8:route==2?10:9,false});for(int bus=1;bus<=2;++bus)if(p.value("mixFilter"+juce::String(f+1)+"Bus"+juce::String(bus))>.0001f)result.push_back({5+f,6+bus,true});}
  int buses[2]={(int)p.value("mixBus1Route"),(int)p.value("mixBus2Route")};if(buses[0]==2&&buses[1]==2)buses[1]=0;for(int bus=0;bus<2;++bus)result.push_back({7+bus,buses[bus]==2?8-bus:buses[bus]==1?10:9,false});return result;
 }
};
class MixCableCanvas:public juce::Component,public juce::SettableTooltipClient {
 SynthProcessor& processor;int dragging=-1,hover=-1;juce::Point<float> cursor;bool additive=false;
 juce::Point<float> out(int channel)const{return {channel*110.f+(channel<5?54.f:35.f),6.f};}
 juce::Point<float> in(int channel)const{return {channel*110.f+(channel>=9?54.f:76.f),6.f};}
 int target(juce::Point<float> point)const{if(point.y< -4||point.y>getHeight()+6)return -1;int channel=(int)(point.x/110);return channel>=5&&channel<11?channel:-1;}
 void cable(juce::Graphics& g,juce::Point<float> a,juce::Point<float> b,juce::Colour colour,float thickness){juce::Path path;path.startNewSubPath(a);float depth=juce::jmin((float)getHeight()-2,18.f+std::abs(a.x-b.x)*.012f);path.cubicTo(a.x,depth,b.x,depth,b.x,b.y);g.setColour(nd::colour(juce::Colours::black.withAlpha(.6f)));g.strokePath(path,juce::PathStrokeType(thickness+2));g.setColour(nd::colour(colour));g.strokePath(path,juce::PathStrokeType(thickness));}
public:
 explicit MixCableCanvas(SynthProcessor& p):processor(p){setComponentID("mix-cable-canvas");setMouseCursor(juce::MouseCursor::PointingHandCursor);setTooltip("Drag an OUT dot to an IN dot to route audio. Shift-drag: add a bus send / second filter. Right-click OUT: disconnect.");}
 void paint(juce::Graphics& g)override{nd::Scope suitScope("controls");using namespace juce;g.fillAll(nd::colour(Colour(0xff121c25)));for(auto& edge:MixCableRouting::edges(processor)){bool active=dragging==edge.from||hover==edge.from||hover==edge.to;cable(g,out(edge.from),in(edge.to),nd::colour(Colour(edge.send?0xff80c5ff:NEXUM_UI_ACCENT)).withAlpha(active?1.f:.55f),active?2.4f:1.5f);}
  if(dragging>=0)cable(g,out(dragging),cursor,nd::colour(Colour(MixCableRouting::allowed(dragging,target(cursor))?0xff80d4ff:0xffffaa55)),2.2f);
  for(int channel=0;channel<11;++channel){if(channel<9){auto pt=out(channel);g.setColour(nd::colour(nd::colour(Colour(NEXUM_UI_ACCENT))));g.fillEllipse(pt.x-4,pt.y-4,8,8);{g.setFont(FontOptions(7.f,Font::bold));g.drawText("OUT",(int)pt.x-20,0,16,10,Justification::centredRight);}}
   if(channel>=5){auto pt=in(channel);g.setColour(nd::colour(nd::colour(Colour(dragging>=0&&MixCableRouting::allowed(dragging,channel)?0xff80d4ff:0xff8aabbd))));g.drawEllipse(pt.x-4,pt.y-4,8,8,1.2f);{g.setFont(FontOptions(7.f,Font::bold));g.drawText("IN",(int)pt.x+7,0,15,10,Justification::centredLeft);}}}
 }
 void mouseMove(const juce::MouseEvent& e)override{hover=(int)(e.position.x/110);repaint();}
 void mouseExit(const juce::MouseEvent&)override{hover=-1;repaint();}
 void mouseDown(const juce::MouseEvent& e)override{int channel=(int)(e.position.x/110);if(channel<0||channel>=9||e.position.getDistanceFrom(out(channel))>13)return;if(e.mods.isPopupMenu()){if(channel>=7)return;juce::PopupMenu menu;menu.addItem(1,"Disconnect output and sends");auto safe=juce::Component::SafePointer<MixCableCanvas>(this);menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),[safe,channel](int selected){if(safe&&selected==1){MixCableRouting::disconnect(safe->processor,channel);safe->repaint();}});return;}dragging=channel;cursor=e.position;additive=e.mods.isShiftDown();repaint();}
 void mouseDrag(const juce::MouseEvent& e)override{if(dragging<0||e.mods.isPopupMenu())return;cursor=e.position;additive=e.mods.isShiftDown();repaint();}
 void mouseUp(const juce::MouseEvent& e)override{if(dragging>=0&&!e.mods.isPopupMenu())MixCableRouting::connect(processor,dragging,target(e.position),additive);dragging=-1;repaint();}
};
}
