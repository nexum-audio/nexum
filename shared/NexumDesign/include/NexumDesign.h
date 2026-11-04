#pragma once
#ifndef NEXUM_ACCENT
#define NEXUM_ACCENT 0xff48aaff
#endif
// Keep the supplied brand colour in the logo; lift the UI tint for dark surfaces.
#if NEXUM_ACCENT == 0xff394cff
#define NEXUM_UI_ACCENT 0xff8490ff
#else
#define NEXUM_UI_ACCENT NEXUM_ACCENT
#endif
#include <juce_gui_basics/juce_gui_basics.h>
#include <map>
#include <set>
#include <cmath>
namespace nexum { namespace design {
inline juce::String& currentScope(){static thread_local juce::String value="global";return value;}
struct Scope{juce::String previous;explicit Scope(juce::String id):previous(currentScope()){if(id=="curve"&&(previous=="waveform"||previous=="filter"||previous=="envelope"||previous=="lfo"||previous=="fxcurve"||previous=="pianoroll"))id=previous;currentScope()=std::move(id);}~Scope(){currentScope()=previous;}};
inline juce::File designFile(){auto test=juce::SystemStats::getEnvironmentVariable("NEXUM_DESIGN_FILE",{});if(test.isNotEmpty())return juce::File(test);return juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory).getChildFile("NEXUM/suit/design.json");}
inline const std::map<juce::String,juce::String>& defaultParents(){static const std::map<juce::String,juce::String> p={
 {"surfaces","global"},{"card","surfaces"},{"module","card"},{"header","card"},{"rack","surfaces"},{"library","surfaces"},{"controls","global"},{"button","controls"},{"tab","button"},{"icon","button"},{"knob","controls"},{"slider","controls"},{"numeric","controls"},{"combo","controls"},{"toggle","controls"},{"text","global"},{"curve","global"},{"waveform","curve"},{"filter","curve"},{"envelope","curve"},{"lfo","curve"},{"fxcurve","curve"},{"pianoroll","curve"},{"keyboard","controls"},{"meter","controls"}};return p;}
inline juce::var emptyDocument(){auto* root=new juce::DynamicObject;root->setProperty("schema",1);root->setProperty("revision",juce::Uuid().toString());auto* rules=new juce::DynamicObject;auto add=[&](juce::String id,juce::String parent){auto* node=new juce::DynamicObject;node->setProperty("parent",parent);node->setProperty("values",juce::var(new juce::DynamicObject));rules->setProperty(id,juce::var(node));};add("global",{});for(auto& p:defaultParents())add(p.first,p.second);root->setProperty("rules",juce::var(rules));return juce::var(root);}
inline bool valid(const juce::var& value,juce::String& error){
 auto* root=value.getDynamicObject();if(!root||int(value["schema"])!=1){error="Unbekanntes Designformat";return false;}auto* rules=value["rules"].getDynamicObject();if(!rules||!rules->hasProperty("global")||rules->getProperties().size()>512){error="Ungueltige Hierarchie";return false;}
 for(auto& item:rules->getProperties()){auto id=item.name.toString();auto* node=item.value.getDynamicObject();auto* props=item.value["values"].getDynamicObject();if(!node||!props){error="Ungueltiges Element: "+id;return false;}std::set<juce::String> seen;auto at=id;while(at.isNotEmpty()){if(!seen.insert(at).second){error="Hierarchy contains a cycle";return false;}auto parent=rules->getProperty(at)["parent"].toString();if(parent.isNotEmpty()&&!rules->hasProperty(parent)){error="Unbekanntes Elternelement";return false;}at=parent;}if(id!="global"&&!seen.count("global")){error="Element muss von NEXUM erben";return false;}
  for(auto& prop:props->getProperties()){auto key=prop.name.toString();if(key.startsWith("color.")){auto s=prop.value.toString();if(s.length()!=8||!s.containsOnly("0123456789abcdefABCDEF")){error="Farbe muss AARRGGBB sein";return false;}}else if(key.startsWith("number.")){double n=double(prop.value);if(!(prop.value.isDouble()||prop.value.isInt()||prop.value.isInt64())||!std::isfinite(n)||n<0||n>100){error="Ungueltiger Zahlenwert";return false;}}else {error="Unbekannte Eigenschaft";return false;}}
 }
 if(value["rules"]["global"]["parent"].toString().isNotEmpty()){error="NEXUM cannot have a parent";return false;}return true;
}
inline juce::var resolveValue(const juce::var& doc,juce::String id,const juce::String& key,juce::String* origin=nullptr){auto rules=doc["rules"];std::set<juce::String> seen;while(id.isNotEmpty()&&seen.insert(id).second){auto node=rules[juce::Identifier(id)];auto values=node["values"];if(auto* o=values.getDynamicObject())if(o->hasProperty(key)){if(origin)*origin=id;return o->getProperty(key);}auto next=node["parent"].toString();if(next.isEmpty()&&id!="global")next="global";id=next;}return {};}
class Store {
 juce::var document=emptyDocument();juce::String signature;juce::uint64 generation=1;double lastPoll=-1000;
public:
 static Store& instance(){static Store s;return s;}
 const juce::var& get()const{return document;}juce::uint64 version()const{return generation;}
 void preview(const juce::var& value){document=value.clone();++generation;}
 bool reload(){auto now=juce::Time::getMillisecondCounterHiRes();if(now-lastPoll<400)return false;lastPoll=now;auto file=designFile();if(!file.existsAsFile()||file.getSize()>1024*1024)return false;auto source=file.loadFileAsString();if(source==signature)return false;juce::var parsed;juce::String error;if(juce::JSON::parse(source,parsed).failed()||!valid(parsed,error))return false;signature=source;document=parsed;++generation;return true;}
 static juce::Result save(const juce::var& doc){juce::String error;if(!valid(doc,error))return juce::Result::fail(error);auto file=designFile();if(auto r=file.getParentDirectory().createDirectory();r.failed())return r;juce::TemporaryFile temp(file);if(!temp.getFile().replaceWithText(juce::JSON::toString(doc,true)))return juce::Result::fail("Could not write design");return temp.overwriteTargetFileWithTemporary()?juce::Result::ok():juce::Result::fail("Could not replace design atomically");}
};
inline juce::String colourKey(juce::uint32 c){switch(c){
 case 0xff48aaff:case 0xffcccccc:return "color.accent";
 case 0xff12191d:case 0xff15171b:return "color.canvas";
 case 0xff1e2b32:case 0xff20242b:return "color.surface";
 case 0xff243442:return "color.header";
 case 0xff344b5d:case 0xff505258:return "color.selected";
 case 0xff91a3b0:return "color.module";case 0xffa7b9c5:return "color.moduleHeader";case 0xff1b3040:return "color.moduleInk";
 case 0xffdce9f5:case 0xffe8e8e8:case 0xffeeeeee:return "color.text";
 case 0xff91a7b9:case 0xff899aa5:case 0xff9ba1aa:return "color.muted";
 case 0xff486074:case 0xff344650:case 0xff44454a:return "color.border";
 case 0xff0c1620:case 0xff13171d:return "color.plot";
 case 0xff15242c:return "color.plotTop";case 0xff0c171e:return "color.plotBottom";
 case 0xff233444:case 0xff2c323c:return "color.grid";
 case 0xffe8c77a:return "color.noteNear";case 0xffe99a58:return "color.noteMedium";case 0xffe06166:return "color.noteFar";
 default:return "color.palette."+juce::String::toHexString((int)c).paddedLeft('0',8);
 }}
inline juce::Colour resolved(juce::Colour fallback){auto key=colourKey(fallback.getARGB());auto value=resolveValue(Store::instance().get(),currentScope(),key);if(value.isString())return juce::Colour::fromString(value.toString());return fallback;}
inline juce::Colour colour(juce::Colour fallback){return currentScope()=="global"?fallback:resolved(fallback);}
inline juce::Colour colour(juce::uint32 fallback){return colour(juce::Colour(fallback));}
inline float number(const juce::String& key,float fallback){auto value=resolveValue(Store::instance().get(),currentScope(),"number."+key);if(value.isVoid())return fallback;float v=float(value);if(!std::isfinite(v))return fallback;if(key=="fontSize")return juce::jlimit(7.f,36.f,v);if(key=="radius")return juce::jlimit(0.f,18.f,v);if(key=="stroke")return juce::jlimit(.3f,6.f,v);if(key=="fillAlpha")return juce::jlimit(0.f,1.f,v);return v;}
inline void applyExplicit(juce::Component& c){const int ids[]={juce::TextButton::buttonColourId,juce::TextButton::buttonOnColourId,juce::TextButton::textColourOffId,juce::TextButton::textColourOnId,juce::Label::textColourId,juce::Label::backgroundColourId,juce::Label::outlineColourId,juce::Slider::thumbColourId,juce::Slider::trackColourId,juce::Slider::textBoxTextColourId,juce::Slider::textBoxBackgroundColourId,juce::Slider::textBoxOutlineColourId,juce::ComboBox::backgroundColourId,juce::ComboBox::textColourId,juce::ComboBox::outlineColourId,juce::TextEditor::backgroundColourId,juce::TextEditor::textColourId,juce::TextEditor::outlineColourId};for(int id:ids)if(c.isColourSpecified(id)){auto key="suit.original."+juce::String(id);auto& properties=c.getProperties();if(!properties.contains(key))properties.set(key,(int)c.findColour(id).getARGB());c.setColour(id,resolved(juce::Colour((juce::uint32)(int)properties[key])));}for(auto* child:c.getChildren())applyExplicit(*child);}
class LiveTheme:public juce::LookAndFeel_V4,private juce::Timer {
 std::map<int,juce::Colour> originals;std::vector<juce::Component::SafePointer<juce::Component>> roots;juce::uint64 observed=0;juce::String productName;
 void timerCallback()override{Store::instance().reload();if(observed==Store::instance().version())return;observed=Store::instance().version();for(auto& c:originals)juce::LookAndFeel_V4::setColour(c.first,resolved(c.second));std::function<void(juce::Component&)> visit=[&](juce::Component& c){if(&c.getLookAndFeel()==this){applyExplicit(c);c.sendLookAndFeelChange();c.repaint();}else for(auto* child:c.getChildren())visit(*child);};if(!roots.empty()){for(auto& root:roots)if(root)visit(*root);}else for(int i=0;i<juce::Desktop::getInstance().getNumComponents();++i)visit(*juce::Desktop::getInstance().getComponent(i));auto diagnostics=juce::SystemStats::getEnvironmentVariable("NEXUM_DESIGN_DIAGNOSTICS",{});if(productName.isNotEmpty()&&diagnostics.isNotEmpty()){juce::File folder(diagnostics);folder.createDirectory();auto* receipt=new juce::DynamicObject;receipt->setProperty("product",productName);receipt->setProperty("revision",Store::instance().get()["revision"]);receipt->setProperty("thumb",findColour(juce::Slider::thumbColourId).toString());receipt->setProperty("canvas",resolved(juce::Colour(0xff12191d)).toString());folder.getChildFile(juce::File::createLegalFileName(productName)+".json").replaceWithText(juce::JSON::toString(juce::var(receipt)));}}

public:
 void attach(juce::Component& root,juce::String product={}){if(product.isNotEmpty())productName=std::move(product);for(auto& p:roots)if(p.getComponent()==&root)return;roots.emplace_back(&root);observed=0;}
 LiveTheme(){startTimer(500);}~LiveTheme()override{stopTimer();}
 void drawLabel(juce::Graphics& g,juce::Label& label)override{Scope scope(dynamic_cast<juce::ComboBox*>(label.getParentComponent())?"combo":dynamic_cast<juce::Slider*>(label.getParentComponent())?"numeric":"text");auto& props=label.getProperties();if(!props.contains("suit.originalFont"))props.set("suit.originalFont",label.getFont().getHeight());label.setFont(label.getFont().withHeight(number("fontSize",float(props["suit.originalFont"]))));juce::LookAndFeel_V4::drawLabel(g,label);}
 juce::Font getTextButtonFont(juce::TextButton& b,int h)override{auto f=juce::LookAndFeel_V4::getTextButtonFont(b,h);return f.withHeight(number("fontSize",f.getHeight()));}
 juce::Font getComboBoxFont(juce::ComboBox& b)override{Scope scope("combo");auto f=juce::LookAndFeel_V4::getComboBoxFont(b);return f.withHeight(number("fontSize",f.getHeight()));}
 void drawButtonText(juce::Graphics& g,juce::TextButton& b,bool over,bool down)override{Scope scope("button");juce::LookAndFeel_V4::drawButtonText(g,b,over,down);}
 void drawToggleButton(juce::Graphics& g,juce::ToggleButton& b,bool over,bool down)override{Scope scope("toggle");juce::LookAndFeel_V4::drawToggleButton(g,b,over,down);}
 void drawRotarySlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float start,float end,juce::Slider& b)override{Scope scope("knob");juce::LookAndFeel_V4::drawRotarySlider(g,x,y,w,h,pos,start,end,b);}
 void drawLinearSlider(juce::Graphics& g,int x,int y,int w,int h,float pos,float min,float max,juce::Slider::SliderStyle style,juce::Slider& b)override{Scope scope("slider");juce::LookAndFeel_V4::drawLinearSlider(g,x,y,w,h,pos,min,max,style,b);}

 void setColour(int id,juce::Colour c){originals[id]=c;juce::LookAndFeel_V4::setColour(id,resolved(c));}
};
}}
namespace nd=nexum::design;
