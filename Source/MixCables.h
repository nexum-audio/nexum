#pragma once
#include <NexumDesign.h>
#include "Processor.h"
#include "Expansion.h"
#include "UiStyle.h"
#include "FxColours.h"
namespace noctrl {
// Presentation order is independent of saved channel/routing indices.
struct MixStripLayout {
 static constexpr int width=104;
 static int visualIndex(int channel){return channel==0?3:channel<4?channel-1:channel;}
 static int x(int channel){int v=visualIndex(channel);return v*108+(v>=5?8:0)+(v>=7?8:0)+(v>=9?8:0);}
 static juce::Point<float> input(int channel){return {(float)x(channel)+18,20};}
 static juce::Point<float> output(int channel){return {(float)x(channel)+86,20};}
 static int channelAt(float xPos){for(int c=0;c<11;++c)if(xPos>=x(c)&&xPos<x(c)+width)return c;return -1;}
};
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
 SynthProcessor& processor;int dragging=-1,hover=-1;juce::Point<float> cursor;bool additive=false;int hoverCable=-1;
 struct CableHit {juce::Path path;MixCableRouting::Edge edge;juce::Rectangle<float> badge;};std::vector<CableHit> cableHits;
 int cableAt(juce::Point<float> pt)const{for(int i=(int)cableHits.size()-1;i>=0;--i)if(!cableHits[(size_t)i].badge.isEmpty()&&cableHits[(size_t)i].badge.contains(pt))return i;for(int i=(int)cableHits.size()-1;i>=0;--i){auto& hit=cableHits[(size_t)i];juce::Path stroke;juce::PathStrokeType(14).createStrokedPath(stroke,hit.path);if(stroke.contains(pt))return i;}return -1;}

 static juce::Colour defaultCableColour(bool send){return nd::colour(juce::Colour(send?0xff80c5ff:NEXUM_UI_ACCENT));}
 juce::Colour edgeColour(const MixCableRouting::Edge& edge)const{auto rack=processor.rack();auto it=rack->cableColours.find(cableColourKey(edge.from,edge.to,edge.send));return it==rack->cableColours.end()?defaultCableColour(edge.send):juce::Colour(it->second);}
 void customColour(MixCableRouting::Edge edge){
  class Picker:public juce::Component {juce::ColourSelector selector;juce::TextButton apply{"Apply colour"};std::function<void(juce::Colour)> commit;
  public:Picker(juce::Colour colour,std::function<void(juce::Colour)> callback):selector(juce::ColourSelector::showColourAtTop|juce::ColourSelector::showSliders|juce::ColourSelector::showColourspace),commit(std::move(callback)){addAndMakeVisible(selector);selector.setCurrentColour(colour);addAndMakeVisible(apply);apply.onClick=[this]{commit(selector.getCurrentColour().withAlpha(1.f));if(auto* box=findParentComponentOfClass<juce::CallOutBox>())box->dismiss();};setSize(280,300);}
   void resized()override{selector.setBounds(4,4,getWidth()-8,getHeight()-42);apply.setBounds(8,getHeight()-34,getWidth()-16,26);}
  };
  auto safe=juce::Component::SafePointer<MixCableCanvas>(this);auto picker=std::make_unique<Picker>(edgeColour(edge),[safe,edge](juce::Colour colour){if(safe){safe->processor.setCableColour(edge.from,edge.to,edge.send,colour.getARGB());safe->repaint();}});juce::CallOutBox::launchAsynchronously(std::move(picker),getScreenBounds().withSizeKeepingCentre(1,1),nullptr);
 }
 void rackMenu(int index,juce::Point<int> screen){if(index<0||index>=(int)cableHits.size())return;auto edge=cableHits[(size_t)index].edge;int bus=connectionRackBus(edge.from,edge.to);auto rack=processor.rack();bool exists=bus>=3&&rack->connections[bus-3];juce::PopupMenu menu;menu.addSectionHeader(bus>=3?connectionRackName(bus):"Patch cable");if(bus>=3){menu.addItem(1,"Add FX rack",!exists);menu.addItem(2,"Open FX rack",exists);menu.addItem(3,"Remove FX rack",exists);menu.addSeparator();}
  static const juce::uint32 colours[]={NEXUM_UI_ACCENT,0xff68c8ff,0xff63d8b5,0xffffd84d,0xffffa45b,0xffff6d75,0xffe991ff,0xffe4e6df};const char* names[]={"Nexum","Ice blue","Mint","Yellow","Orange","Red","Pink","Ivory"};juce::PopupMenu colourMenu;auto selected=rack->cableColours.find(cableColourKey(edge.from,edge.to,edge.send));colourMenu.addItem(99,"Automatic",true,selected==rack->cableColours.end());for(int i=0;i<8;++i)colourMenu.addColouredItem(100+i,names[i],juce::Colour(colours[i]),true,selected!=rack->cableColours.end()&&selected->second==colours[i]);colourMenu.addSeparator();colourMenu.addItem(120,"Custom colour...");menu.addSubMenu("Cable colour",colourMenu);auto safe=juce::Component::SafePointer<MixCableCanvas>(this);
  menu.showMenuAsync(juce::PopupMenu::Options().withTargetScreenArea({screen.x,screen.y,1,1}),[safe,edge,bus](int selected){if(!safe)return;if(selected==1){int created=safe->processor.addConnectionRack(edge.from,edge.to);if(created>=3&&safe->onOpenRack)safe->onOpenRack(created);}else if(selected==2){if(safe->onOpenRack)safe->onOpenRack(bus);}else if(selected==3)safe->processor.removeConnectionRack(bus);else if(selected==99)safe->processor.setCableColour(edge.from,edge.to,edge.send,0);else if(selected>=100&&selected<108)safe->processor.setCableColour(edge.from,edge.to,edge.send,colours[selected-100]);else if(selected==120)safe->customColour(edge);safe->repaint();});
 }

 juce::Point<float> out(int channel)const{return MixStripLayout::output(channel);}
 juce::Point<float> in(int channel)const{return MixStripLayout::input(channel);}
 int target(juce::Point<float> point)const{int channel=MixStripLayout::channelAt(point.x);return channel>=5&&channel<11&&point.getDistanceFrom(in(channel))<=15?channel:-1;}
 juce::Path cable(juce::Graphics& g,juce::Point<float> a,juce::Point<float> b,juce::Colour colour,float thickness,float sag,bool loose=false){a+=juce::Point<float>(0,16);if(!loose)b+=juce::Point<float>(0,16);juce::Path path;path.startNewSubPath(a);float depth=juce::jmax(juce::jmax(a.y,b.y)+8,(sag-(a.y+b.y)*.125f)*4.f/3.f);path.cubicTo(a.x,depth,b.x,depth,b.x,b.y);g.setColour(juce::Colours::black.withAlpha(.85f));g.strokePath(path,juce::PathStrokeType(thickness+3.f,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));g.setColour(colour);g.strokePath(path,juce::PathStrokeType(thickness,juce::PathStrokeType::curved,juce::PathStrokeType::rounded));return path;}
 void socket(juce::Graphics& g,juce::Point<float> pt,bool input,bool connected,juce::Colour colour){using namespace juce;
  g.setColour(Colours::black.withAlpha(.55f));g.fillEllipse(pt.x-13,pt.y-12,26,26);Path nut;for(int i=0;i<6;++i){float angle=MathConstants<float>::twoPi*i/6+MathConstants<float>::pi/6;auto p=pt+Point<float>(std::cos(angle)*11,std::sin(angle)*11);if(i==0)nut.startNewSubPath(p);else nut.lineTo(p);}nut.closeSubPath();g.setGradientFill(ColourGradient(Colour(0xffa9b3bb),pt.x-9,pt.y-10,Colour(0xff45525d),pt.x+10,pt.y+11,false));g.fillPath(nut);g.setColour(Colour(0xffcbd3d9).withAlpha(.7f));g.strokePath(nut,PathStrokeType(.7f));g.setColour(Colour(0xff10171d));g.fillEllipse(pt.x-8,pt.y-8,16,16);g.setColour(Colour(0xff768894));g.drawEllipse(pt.x-8,pt.y-8,16,16,1);g.setColour(Colours::black);g.fillEllipse(pt.x-5.5f,pt.y-5.5f,11,11);
  g.setColour(Colour(0xffd9e3e9));g.setFont(FontOptions(7.f,Font::bold));g.drawText(input?"IN":"OUT",(int)pt.x-13,0,26,8,Justification::centred);
  if(!connected)return;g.setColour(Colour(0xff080e13));g.fillRoundedRectangle(pt.x-7,pt.y-2,14,20,4);g.setGradientFill(ColourGradient(Colour(0xff485963),pt.x-5,pt.y,Colour(0xff1a252e),pt.x+6,pt.y,false));g.fillRoundedRectangle(pt.x-5.5f,pt.y-1,11,18,3);g.setGradientFill(ColourGradient(Colour(0xffc0cad0),pt.x-4,pt.y-4,Colour(0xff596873),pt.x+4,pt.y+3,false));g.fillRoundedRectangle(pt.x-4,pt.y-4,8,7,1.5f);g.setColour(colour);g.fillRoundedRectangle(pt.x-5,pt.y+5,10,4,1);g.setColour(Colour(0xff101a22));for(int i=0;i<2;++i)g.drawHorizontalLine((int)pt.y+11+i*3,pt.x-4,pt.x+4);

 }

public:
 std::function<void(int)> onOpenRack;
 explicit MixCableCanvas(SynthProcessor& p):processor(p){setComponentID("mix-cable-canvas");setMouseCursor(juce::MouseCursor::PointingHandCursor);setTooltip("Drag an OUT jack to an IN jack to route audio. Shift-drag: add a bus send / second filter. Right-click OUT: disconnect. Click any cable to change its colour; generator-to-filter cables also support FX racks.");}
 void paint(juce::Graphics& g)override{nd::Scope suitScope("controls");using namespace juce;g.setColour(nd::colour(Colour(0xff121c25)));g.fillRect(0,38,getWidth(),getHeight()-38);for(int channel=0;channel<11;++channel){int x=MixStripLayout::x(channel);g.setGradientFill(ColourGradient(Colours::black.withAlpha(.05f),(float)x,0,Colours::black.withAlpha(.23f),(float)x,37,false));g.fillRect(x,0,MixStripLayout::width,37);g.setColour(Colours::white.withAlpha(.13f));g.drawHorizontalLine(1,(float)x+4,(float)x+MixStripLayout::width-4);}auto edges=MixCableRouting::edges(processor);cableHits.clear();auto rack=processor.rack();int lane=0;for(auto& edge:edges){float top=juce::jmin(64.f,(float)getHeight()-12.f),bottom=(float)getHeight()-12.f;float sag=edges.size()>1?top+(bottom-top)*lane/(float)(edges.size()-1):(top+bottom)*.5f;++lane;bool active=dragging==edge.from||hover==edge.from||hover==edge.to||hoverCable==lane-1;auto path=cable(g,out(edge.from),in(edge.to),edgeColour(edge).withAlpha(active?1.f:.85f),active?4.8f:3.4f,sag);cableHits.push_back({path,edge,{}});}
  std::vector<Rectangle<float>> badges;for(auto& hit:cableHits){int bus=connectionRackBus(hit.edge.from,hit.edge.to);if(bus<3||!rack->connections[bus-3])continue;
   std::vector<const FxEntry*> effects;for(auto& entry:rack->entries)if(entry.bus==bus)effects.push_back(&entry);int shown=jmin((int)effects.size(),effects.size()>7?6:7),remaining=(int)effects.size()-shown;int lines=jmax(1,shown+(remaining>0?1:0));float height=22.f+lines*14.f;
   auto middle=hit.path.getPointAlongPath(hit.path.getLength()*.5f);Rectangle<float> best;float distance=std::numeric_limits<float>::max();for(float y=40;y+height<=getHeight()-4;y+=6)for(int col=0;col<7;++col){Rectangle<float> candidate(8.f+col*171.f,y,162,height);bool overlap=false;for(auto existing:badges)overlap|=candidate.expanded(1).intersects(existing);float d=candidate.getCentre().getDistanceFrom(middle);if(!overlap&&d<distance){distance=d;best=candidate;}}if(best.isEmpty())best={jlimit(4.f,(float)getWidth()-166,middle.x-81),jlimit(40.f,(float)getHeight()-height-4,middle.y-height*.5f),162,height};
   hit.badge=best;badges.push_back(best);auto accent=edgeColour(hit.edge);g.setColour(nd::colour(Colour(0xff1b2935)));g.fillRoundedRectangle(best,4);g.setColour(accent.withAlpha(.85f));g.drawRoundedRectangle(best.reduced(.5f),4,1);g.setColour(accent);g.setFont(FontOptions(9.f,Font::bold));g.drawText(connectionRackName(bus),best.reduced(6,2).withHeight(14),Justification::centredLeft,false);g.setColour(accent.withAlpha(.25f));g.drawHorizontalLine((int)best.getY()+17,best.getX()+5,best.getRight()-5);
   g.setFont(FontOptions(10.f,Font::bold));for(int row=0;row<shown;++row){auto& effect=*effects[(size_t)row];const auto& name=fxDefinitions()[(size_t)effect.type].name;auto colour=processor.value(fxId(effect.slot,"enabled"))>.5f?fxAccent(name):nd::colour(Colour(0xff64717b));float y=best.getY()+19+row*14;g.setColour(colour);g.fillEllipse(best.getX()+7,y+5,4,4);g.drawText(String(row+1)+"  "+name,Rectangle<float>(best.getX()+16,y,best.getWidth()-22,14),Justification::centredLeft,true);}
   if(effects.empty()||remaining>0){g.setColour(nd::colour(Colour(0xffa5b7c3)));g.setFont(FontOptions(9.f));g.drawText(effects.empty()?"Empty rack / click to edit":"+ "+String(remaining)+" more / open rack",Rectangle<float>(best.getX()+7,best.getY()+19+shown*14,best.getWidth()-14,14),Justification::centredLeft,true);}
  }

  if(dragging>=0)cable(g,out(dragging),cursor,nd::colour(Colour(MixCableRouting::allowed(dragging,target(cursor))?0xff80d4ff:0xffffaa55)),4.8f,(float)getHeight()*.85f,true);
  for(int channel=0;channel<11;++channel){if(channel<9){bool connected=dragging==channel;Colour colour=defaultCableColour(false);for(auto& edge:edges)if(edge.from==channel){connected=true;colour=edgeColour(edge);break;}socket(g,out(channel),false,connected,colour);}if(channel>=5){bool connected=false;Colour colour=defaultCableColour(false);for(auto& edge:edges)if(edge.to==channel){connected=true;colour=edgeColour(edge);break;}if(dragging>=0&&target(cursor)==channel)colour=Colour(0xff80d4ff);socket(g,in(channel),true,connected,colour);}}

 }
 void mouseMove(const juce::MouseEvent& e)override{hoverCable=cableAt(e.position);hover=MixStripLayout::channelAt(e.position.x);repaint();}
 void mouseExit(const juce::MouseEvent&)override{hover=-1;hoverCable=-1;repaint();}
 void mouseDown(const juce::MouseEvent& e)override{int channel=MixStripLayout::channelAt(e.position.x);if(channel<0||channel>=9||e.position.getDistanceFrom(out(channel))>13){rackMenu(cableAt(e.position),e.getScreenPosition());return;}if(e.mods.isPopupMenu()){if(channel>=7)return;juce::PopupMenu menu;menu.addItem(1,"Disconnect output and sends");auto safe=juce::Component::SafePointer<MixCableCanvas>(this);menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(this),[safe,channel](int selected){if(safe&&selected==1){MixCableRouting::disconnect(safe->processor,channel);safe->repaint();}});return;}dragging=channel;cursor=e.position;additive=e.mods.isShiftDown();repaint();}
 void mouseDrag(const juce::MouseEvent& e)override{if(dragging<0||e.mods.isPopupMenu())return;cursor=e.position;additive=e.mods.isShiftDown();repaint();}
 void mouseUp(const juce::MouseEvent& e)override{if(dragging>=0&&!e.mods.isPopupMenu())MixCableRouting::connect(processor,dragging,target(e.position),additive);dragging=-1;repaint();}
};
}
