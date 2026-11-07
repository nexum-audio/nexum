#include "MatrixPage.h"
#include <NexumPresetHeader.h>
#include "SynthSidebar.h"
#include <NexumDesign.h>
#include "PresetSaveMenu.h"

#include "Expansion.h"

#include "OscPage.h"

#include "MixPage.h"

#include "FxPage.h"

#include "AutomationSlider.h"

#include "PackBrowser.h"

#include "MasterPanel.h"

#include "KeyboardPanel.h"

#include "SequencerPage.h"

#include "ExternalPresetImport.h"

#include "Processor.h"

#include <BinaryData.h>

using namespace juce;

namespace noctrl {

static Colour accent(NEXUM_UI_ACCENT), background(0xff12191d), panel(0xff1e2b32), ink(0xffdce9f5), muted(0xff91a7b9);

class Theme:public OscModernTheme {

public:

 Theme(){setColour(Slider::thumbColourId,accent);setColour(Slider::trackColourId,accent);setColour(Slider::textBoxTextColourId,ink);setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);setColour(Slider::textBoxBackgroundColourId,Colours::transparentBlack);setColour(ComboBox::backgroundColourId,nd::colour(Colour(0xff15212d)));setColour(ComboBox::textColourId,ink);setColour(ComboBox::outlineColourId,nd::colour(Colour(0xff304151)));setColour(PopupMenu::backgroundColourId,panel);setColour(PopupMenu::textColourId,ink);setColour(PopupMenu::highlightedBackgroundColourId,accent);setColour(PopupMenu::highlightedTextColourId,Colours::black);setColour(TextButton::buttonColourId,nd::colour(Colour(0xff202f3e)));setColour(TextButton::textColourOffId,ink);setColour(TextButton::buttonOnColourId,accent);setColour(TextButton::textColourOnId,Colours::black);}



 void drawComboBox(Graphics& g,int w,int h,bool down,int x,int y,int bw,int bh,ComboBox& box)override{nd::Scope suitScope("combo");if(!(bool)box.getProperties()["presetHeader"]){OscModernTheme::drawComboBox(g,w,h,down,x,y,bw,bh,box);return;}g.fillAll(nd::colour(Colour(0xff17212a)));Path arrow;arrow.addTriangle(w-17.f,h*.5f-3,w-7.f,h*.5f-3,w-12.f,h*.5f+3);g.setColour(nd::colour(nd::colour(Colour(0xff91a7b9))));g.fillPath(arrow);}

 Font getTextButtonFont(TextButton& button,int height)override{return (bool)button.getProperties()["compactHeaderAction"]?Font(FontOptions(nd::number("fontSize",10.5f),Font::bold)):OscModernTheme::getTextButtonFont(button,height);}

 Font getComboBoxFont(ComboBox& box)override{return (bool)box.getProperties()["presetHeader"]?Font(FontOptions(nd::number("fontSize",13),Font::bold)):OscModernTheme::getComboBoxFont(box);}

 void drawButtonBackground(Graphics& g,Button& button,const Colour& colour,bool over,bool down)override{nd::Scope suitScope("button");if((bool)button.getProperties()["sidebarTab"]){if(over||down){g.setColour(nd::colour(Colour(NEXUM_UI_ACCENT)).withAlpha(down?.14f:.07f));g.fillRect(button.getLocalBounds());}if(button.getToggleState()){g.setColour(accent);g.fillRect(0,7,3,button.getHeight()-14);}g.setColour(nd::colour(Colour(0xff30414e)));g.drawHorizontalLine(button.getHeight()-1,0.f,(float)button.getWidth());return;}if(!(bool)button.getProperties()["presetHeader"]){OscModernTheme::drawButtonBackground(g,button,colour,over,down);return;}g.fillAll(nd::colour(Colour(down?0xff294455:over?0xff233543:0xff17212a)));g.setColour(nd::colour(nd::colour(Colour(0xff34444f))));g.drawVerticalLine(button.getWidth()-1,7.f,(float)button.getHeight()-7);}

 void drawButtonText(Graphics& g,TextButton& button,bool over,bool down)override{nd::Scope suitScope("button");if((bool)button.getProperties()["sidebarTab"]){nxf::drawSidebarTabText(g,button,over);return;}if(!(bool)button.getProperties()["presetHeader"]){OscModernTheme::drawButtonText(g,button,over,down);return;}g.setColour(nd::colour(over?accent:Colour(0xffa9becd)));float cx=button.getWidth()*.5f,cy=button.getHeight()*.5f;Path shape;if((bool)button.getProperties()["presetSave"]){g.drawRoundedRectangle(cx-6,cy-6,12,12,1,1.3f);g.drawRect(Rectangle<float>(cx-3.5f,cy-6,6,5),1.1f);g.drawRect(Rectangle<float>(cx-3.5f,cy+1,7,5),1.1f);}else{float dir=(float)(int)button.getProperties()["presetDirection"];shape.startNewSubPath(cx-dir*2,cy-4);shape.lineTo(cx+dir*2,cy);shape.lineTo(cx-dir*2,cy+4);g.strokePath(shape,PathStrokeType(nd::number("stroke",1.8f),PathStrokeType::curved,PathStrokeType::rounded));}}



};

class Knob:public Component {

public:

 int macroIndex=-1;std::unique_ptr<MacroDragHandle> macroHandle;AutomationSlider slider;Label label;std::unique_ptr<AudioProcessorValueTreeState::SliderAttachment> attachment;

 Knob(SynthProcessor& p,String id,String name){addAndMakeVisible(slider);addAndMakeVisible(label);label.setInterceptsMouseClicks(false,false);label.setText(name,dontSendNotification);label.setJustificationType(Justification::centred);label.setFont(FontOptions(nd::number("fontSize",10),Font::bold));label.setColour(Label::textColourId,muted);slider.setSliderStyle(Slider::RotaryHorizontalVerticalDrag);slider.setTextBoxStyle(Slider::TextBoxBelow,false,78,17);slider.setRotaryParameters(MathConstants<float>::pi*1.24f,MathConstants<float>::pi*2.76f,true);slider.setTooltip(name+juce::String::fromUTF8(" — drag to change; double-click to reset"));attachment=std::make_unique<AudioProcessorValueTreeState::SliderAttachment>(p.state,id,slider);auto* param=dynamic_cast<RangedAudioParameter*>(p.state.getParameter(id));slider.bindAutomation(param,&p);getProperties().set("parameterIndex",param->getParameterIndex());slider.setDoubleClickReturnValue(true,param->convertFrom0to1(param->getDefaultValue()));

 slider.textFromValueFunction=[id](double v){if(id.endsWith("cut"))return v>=1000?String(v/1000,2)+" kHz":String(roundToInt(v))+" Hz";if(id.contains("env")&&(id.endsWith("a")||id.endsWith("d")||id.endsWith("r")))return v<1?String(roundToInt(v*1000))+" ms":String(v,2)+" s";if(id.endsWith("rate"))return String(v,2)+" Hz";if(id.endsWith("voices")||id.endsWith("oct"))return String((int)v);return String((int)std::round(v*100))+"%";};slider.valueFromTextFunction=[id](const String& text){double v=text.getDoubleValue();if(id.endsWith("cut"))return v*(text.containsIgnoreCase("k")?1000.0:1.0);if(id.contains("env")&&(id.endsWith("a")||id.endsWith("d")||id.endsWith("r")))return v/(text.containsIgnoreCase("ms")?1000.0:1.0);if(id.endsWith("rate")||id.endsWith("voices")||id.endsWith("oct"))return v;return v/100.0;};slider.updateText();if(id.startsWith("macro")){macroIndex=id.getTrailingIntValue()-1;slider.setTextBoxStyle(Slider::NoTextBox,false,0,0);slider.getProperties().set("macroTile",true);macroHandle=std::make_unique<MacroDragHandle>(p,slider,macroIndex);addAndMakeVisible(*macroHandle);}slider.setColour(Slider::textBoxOutlineColourId,Colours::transparentBlack);slider.setColour(Slider::thumbColourId,accent);setColour(Slider::trackColourId,accent);setColour(Slider::textBoxTextColourId,ink);

 }

 void mouseDown(const MouseEvent& e)override{if(e.mods.isShiftDown())slider.assignSelectedMacro();else if(e.mods.isPopupMenu())slider.showAutomationMenu(slider.getLocalPoint(this,e.getPosition()));else slider.selectMacro();}

 void paint(Graphics& g)override{nd::Scope suitScope("controls");if(macroIndex<0)return;g.setColour(nd::colour(nd::colour(Colour(0xff344650))));g.drawVerticalLine(getWidth()-1,3.f,(float)getHeight()-3);}

 void resized()override{if(macroIndex>=0){label.setBounds(1,getHeight()-18,getWidth()-2,17);slider.setBounds(1,1,getWidth()-30,getHeight()-20);macroHandle->setBounds(getWidth()-28,1,27,getHeight()-20);}else{label.setBounds(0,0,getWidth(),15);slider.setBounds(0,15,getWidth(),getHeight()-15);}}

};

struct Choice:Component {

 ComboBox box;std::unique_ptr<AudioProcessorValueTreeState::ComboBoxAttachment> attach;

 Choice(SynthProcessor& p,String id){addAndMakeVisible(box);auto* param=dynamic_cast<AudioParameterChoice*>(p.state.getParameter(id));box.addItemList(param->choices,1);attach=std::make_unique<AudioProcessorValueTreeState::ComboBoxAttachment>(p.state,id,box);box.setTooltip(param->name);getProperties().set("parameterIndex",param->getParameterIndex());}

 void resized()override{box.setBounds(getLocalBounds());}

};

class PackWorker:public Thread {

 std::function<void()> work;

public:

 explicit PackWorker(std::function<void()> fn):Thread("NoCTRL Pack Import"),work(std::move(fn)){}

 void run()override{work();}

};

class SynthEditor:public AudioProcessorEditor,public FileDragAndDropTarget,public DragAndDropContainer,private Timer,private AsyncUpdater {

 SynthProcessor& p;Theme theme;nxf::PresetHeaderStyle presetStyle{Colour(NEXUM_UI_ACCENT)};SynthSidebar sidebar{p};OwnedArray<Knob> knobs;OwnedArray<Choice> choices;std::vector<Component*> modulationWidgets,shapeWidgets;

 nxf::HeaderAction panic{true,Colour(NEXUM_UI_ACCENT)};MasterPanel masterPanel{p};KeyboardPanel keyboard{p};ComboBox presets;Label presetArtist,presetDescription;String headerMetadataText;PresetMetadata headerMetadata;TextButton previous{"<"},next{">"},save{"SAVE"},load{"LOAD"},synthTab{"OSC"},mixTab{"MIX"},fxTab{"FX"},matrixTab{"MATRIX"},packsTab{"PRESETS"},pack{"IMPORT"},importInfo{"IMPORT INFO"},assetFolder{"ASSET FOLDER"},arpToggle{"ARP"};Array<File> libraryFiles;std::unique_ptr<PackWorker> packWorker;CriticalSection packResultLock;PackResult pendingPack;

 std::unique_ptr<MatrixPage> matrixPage;std::unique_ptr<SequencerPage> sequencePage;std::unique_ptr<FxPage> fxPage;std::unique_ptr<MixPage> mixPage;std::unique_ptr<OscPage> oscPage;std::unique_ptr<PackBrowser> browser;int activePage=0;

 Image logo;std::unique_ptr<FileChooser> fileChooser;TooltipWindow tooltip{this,600};bool matrix=false;

 void text(Graphics& g,String t,Rectangle<float> r,float size,Colour c,Justification j=Justification::centredLeft){g.setColour(nd::colour(c));g.setFont(FontOptions(nd::number("fontSize",size),Font::bold));g.drawText(t,r,j);}

 void box(Graphics& g,Rectangle<float> r,String title){UiStyle::card(g,r,title);}

 Knob* knob(String id,String title,int x,int y,int w=86,int h=84,bool lower=false){auto* k=knobs.add(new Knob(p,id,title));addAndMakeVisible(k);k->setBounds(x,y,w,h);if(lower)shapeWidgets.push_back(k);return k;}

 Choice* choice(String id,int x,int y,int w,int h=25,bool lower=false){auto* c=choices.add(new Choice(p,id));addAndMakeVisible(c);c->setBounds(x,y,w,h);if(lower)shapeWidgets.push_back(c);return c;}

 void setPage(int page){activePage=page;if(sequencePage){sequencePage->setVisible(page==5||page==6);if(page==5||page==6){sequencePage->setMode(page==6);sequencePage->toFront(false);}}matrix=page==1;for(auto* c:knobs)c->setVisible(false);for(auto* c:choices)c->setVisible(false);if(matrixPage)matrixPage->setVisible(page==1);for(auto* c:shapeWidgets)c->setVisible(false);for(auto* c:modulationWidgets)c->setVisible(false);for(auto* c:{&importInfo,&assetFolder})c->setVisible(false);arpToggle.setVisible(false);load.setVisible(false);pack.setVisible(false);if(oscPage){oscPage->setVisible(page==0||page==3||page==4);if(page==0||page==3||page==4)oscPage->toFront(false);}if(browser){browser->setVisible(page==2);if(page==2){browser->refresh();browser->toFront(false);}}if(oscPage)oscPage->showGenerators(page==0);if(mixPage){mixPage->setVisible(page==3);if(page==3)mixPage->toFront(false);}if(fxPage){fxPage->setVisible(page==4);if(page==4)fxPage->toFront(false);}fxTab.setToggleState(page==4,dontSendNotification);mixTab.setToggleState(page==3,dontSendNotification);synthTab.setToggleState(page==0,dontSendNotification);matrixTab.setToggleState(page==1,dontSendNotification);packsTab.setToggleState(page==2,dontSendNotification);repaint();}

 void setMatrix(bool show){setPage(show?1:0);}

 void refreshLibrary(){presets.clear(dontSendNotification);libraryFiles=ExternalPresetImport::defaultLibraryRoot().findChildFiles(File::findFiles,true,"*.SerumPreset;*.ncsynth");libraryFiles.addArray(ExternalPresetImport::defaultLibraryRoot().getParentDirectory().getChildFile("User Packs").findChildFiles(File::findFiles,true,"*.ncsynth"));libraryFiles.addArray(ExternalPresetImport::defaultLibraryRoot().getParentDirectory().getChildFile("Factory Packs").findChildFiles(File::findFiles,true,"*.ncsynth"));libraryFiles.sort();for(int i=0;i<libraryFiles.size();++i)presets.addItem((libraryFiles[i].getFullPathName().contains("Factory Packs")?"FACTORY / ":libraryFiles[i].hasFileExtension("ncsynth")?"USER / ":"PACK / ")+libraryFiles[i].getFileNameWithoutExtension(),1000+i);if(browser)browser->refresh();}

 void loadExternal(File file){if(file.hasFileExtension("ncsynth")){openFile(file);return;}String error;if(!p.loadSerumPreset(file,file.getParentDirectory(),error))AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon,"Preset import failed",error);repaint();}

 void handleAsyncUpdate()override{PackResult result;{const ScopedLock lock(packResultLock);result=pendingPack;}pack.setEnabled(true);refreshLibrary();if(result.ok){auto files=result.root.findChildFiles(File::findFiles,true,"*.SerumPreset");files.sort();if(files.size())loadExternal(files[0]);AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,"Pack imported",String(result.presets)+" presets / "+String(result.assets)+" WAV assets.\n\nPresets are approximate conversions. Select them in the preset menu. IMPORT INFO shows limitations for the current preset.\n\nReport: "+result.report.getFullPathName());}else AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon,"Pack import failed",result.error);}

 void beginPackImport(File file){if(packWorker&&packWorker->isThreadRunning())return;packWorker.reset();pack.setEnabled(false);packWorker=std::make_unique<PackWorker>([this,file]{auto result=ExternalPresetImport::importPack(file,ExternalPresetImport::defaultLibraryRoot());{const ScopedLock lock(packResultLock);pendingPack=std::move(result);}triggerAsyncUpdate();});packWorker->startThread();}

 void openFile(File file){if(file.hasFileExtension("SerumPack"))beginPackImport(file);else if(file.hasFileExtension("SerumPreset"))loadExternal(file);else{MemoryBlock bytes;if(file.loadFileAsData(bytes)){auto xml=AudioProcessor::getXmlFromBinary(bytes.getData(),(int)bytes.getSize());if(xml&&noctrl::isSynthState(*xml)){p.loadSoundState(bytes.getData(),(int)bytes.getSize());p.state.state.setProperty("currentPresetFile",file.getFullPathName(),nullptr);}}}}

 void chooseFile(bool writing){fileChooser=std::make_unique<FileChooser>(writing?"Save NoCTRL preset":"Load NoCTRL preset",File::getSpecialLocation(File::userDocumentsDirectory),writing?"*.ncsynth":"*.ncsynth;*.SerumPreset;*.SerumPack");auto safe=Component::SafePointer<SynthEditor>(this);fileChooser->launchAsync((writing?FileBrowserComponent::saveMode|FileBrowserComponent::warnAboutOverwriting:FileBrowserComponent::openMode)|FileBrowserComponent::canSelectFiles,[safe,writing](const FileChooser& chooser){if(!safe)return;auto file=chooser.getResult();if(file==File())return;if(writing){MemoryBlock data;safe->p.getStateInformation(data);if(!file.withFileExtension("ncsynth").replaceWithData(data.getData(),data.getSize()))AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon,"Save failed","Could not write this preset.");}else{safe->openFile(file);}});}

 void updatePresetHeader(){String metadata=p.state.state.getProperty("libraryMetadata").toString();if(metadata!=headerMetadataText){headerMetadataText=metadata;headerMetadata=PresetMetadata::fromJson(JSON::parse(metadata));}String name,author,description;auto assets=std::atomic_load(&p.importedAssets);auto ownName=p.state.state.getProperty("userPresetName").toString();if(ownName.isNotEmpty()){name=ownName;author=headerMetadata.author;description=headerMetadata.description;}else if(assets){name=assets->name;author=assets->author;description=assets->description;}else{name=SynthProcessor::presetNames()[0];author="NoCTRL";}if(presets.getText()!=name)presets.setText(name,dontSendNotification);presets.setTooltip(name);auto dash=String::charToString(0x2014);presetArtist.setText(author.isEmpty()?dash:author,dontSendNotification);presetDescription.setText(description.isEmpty()?dash:description,dontSendNotification);presetArtist.setTooltip(author);presetDescription.setTooltip(description);}

 void timerCallback()override{sidebar.refreshMacros();for(auto* k:knobs){auto id=k->slider.getComponentID().fromFirstOccurrenceOf("parameter:",false,false);if(id.startsWith("macro")){int m=id.getTrailingIntValue()-1;auto state=p.macros();k->label.setText(state->names[m].isEmpty()?"MACRO "+String(m+1):state->names[m],dontSendNotification);k->label.setColour(Label::textColourId,Colour(state->colours[m]));}}arpToggle.setToggleState(p.sequenceArpEnabled(),dontSendNotification);updatePresetHeader();masterPanel.repaint();repaint();}

public:

 SynthEditor(SynthProcessor& proc):AudioProcessorEditor(proc),p(proc){setLookAndFeel(&theme);theme.attach(*this,"Nexum");setSize(1332,961);addAndMakeVisible(masterPanel);masterPanel.setJoinedHeader(true);masterPanel.setBounds(1144,0,96,66);logo=ImageCache::getFromMemory(BinaryData::NEXUM_SYNTH_png,BinaryData::NEXUM_SYNTH_pngSize);

  addAndMakeVisible(presets);presets.setSelectedId(p.currentPreset+1,dontSendNotification);presets.setBounds(36,1,992,34);presets.getProperties().set("presetHeader",true);presets.setColour(ComboBox::textColourId,accent);presets.setComponentID("preset-name");for(auto* label:{&presetArtist,&presetDescription}){addAndMakeVisible(label);label->setFont(FontOptions(nd::number("fontSize",11.5f)));label->setColour(Label::textColourId,nd::colour(Colour(0xffbecfdb)));label->setInterceptsMouseClicks(false,false);}presetArtist.setBounds(53,39,135,23);presetDescription.setBounds(238,39,844,23);presetArtist.setComponentID("preset-artist");presetDescription.setComponentID("preset-description");presets.onChange=[this]{int id=presets.getSelectedId();if(id>=1000&&id-1000<libraryFiles.size())loadExternal(libraryFiles[id-1000]);};refreshLibrary();updatePresetHeader();

  for(auto* b:std::initializer_list<Button*>{&previous,&next,&save,&load,&synthTab,&mixTab,&fxTab,&matrixTab,&packsTab,&panic,&pack,&importInfo,&assetFolder,&arpToggle})addAndMakeVisible(b);for(auto* tab:{&synthTab,&mixTab,&fxTab,&matrixTab,&packsTab}){OscModernTheme::styleTab(*tab);tab->getProperties().set("squareMainTab",true);}for(auto* button:{&save,&previous,&next})button->getProperties().set("presetHeader",true);save.getProperties().set("presetSave",true);save.setComponentID("preset-save-menu");previous.getProperties().set("presetDirection",-1);next.getProperties().set("presetDirection",1);for(auto* button:{&load,&pack})button->getProperties().set("compactHeaderAction",true);save.setTooltip(juce::String::fromUTF8("Save as: Create new preset / Overwrite preset"));previous.setTooltip("Previous preset");next.setTooltip("Next preset");save.setBounds(1,1,33,34);previous.setBounds(1030,1,28,34);next.setBounds(1060,1,28,34);load.setBounds(1080,3,42,27);synthTab.setBounds(211,2,64,64);mixTab.setBounds(277,2,64,64);fxTab.setBounds(343,2,64,64);matrixTab.setBounds(409,2,64,64);packsTab.setBounds(475,2,64,64);panic.setBounds(1106,0,38,66);panic.setComponentID("master-panic");pack.setBounds(1126,3,52,27);importInfo.setBounds(1050,735,164,23);assetFolder.setBounds(408,735,146,23);arpToggle.setBounds(565,735,76,23);arpToggle.onClick=[this]{p.setSequenceArpEnabled(!p.sequenceArpEnabled());};assetFolder.onClick=[this]{fileChooser=std::make_unique<FileChooser>("Choose your Serum preset library (Tables, Samples, Multisamples)",File::getSpecialLocation(File::userDocumentsDirectory));auto safe=Component::SafePointer<SynthEditor>(this);fileChooser->launchAsync(FileBrowserComponent::openMode|FileBrowserComponent::canSelectDirectories,[safe](const FileChooser& fc){if(!safe||!fc.getResult().isDirectory())return;auto config=ExternalPresetImport::defaultLibraryRoot().getChildFile("AssetSearchPaths.txt");config.getParentDirectory().createDirectory();auto paths=StringArray::fromLines(config.loadFileAsString());paths.addIfNotAlreadyThere(fc.getResult().getFullPathName());config.replaceWithText(paths.joinIntoString("\n"));if(auto a=std::atomic_load(&safe->p.importedAssets))if(File(a->sourcePath).existsAsFile())safe->loadExternal(File(a->sourcePath));});};

  pack.onClick=[this]{fileChooser=std::make_unique<FileChooser>("Import SerumPack",File::getSpecialLocation(File::userDesktopDirectory),"*.SerumPack");auto safe=Component::SafePointer<SynthEditor>(this);fileChooser->launchAsync(FileBrowserComponent::openMode|FileBrowserComponent::canSelectFiles,[safe](const FileChooser& fc){if(safe&&fc.getResult()!=File())safe->beginPackImport(fc.getResult());});};importInfo.onClick=[this]{auto a=std::atomic_load(&p.importedAssets);AlertWindow::showMessageBoxAsync(MessageBoxIconType::InfoIcon,"Serum preset import",a?a->name+" / "+a->author+"\n\nApproximate import:\n"+a->warnings.joinIntoString("\n"):"Load a .SerumPreset with LOAD, or import a .SerumPack with PACK. Presets from packs appear in the preset selector.");};previous.onClick=[this]{int count=presets.getNumItems();if(count>0)presets.setSelectedItemIndex((presets.getSelectedItemIndex()+count-1)%count,sendNotificationSync);};next.onClick=[this]{int count=presets.getNumItems();if(count>0)presets.setSelectedItemIndex((presets.getSelectedItemIndex()+1)%count,sendNotificationSync);};save.onClick=[this]{auto menu=presetSaveMenu(p);auto safe=Component::SafePointer<SynthEditor>(this);menu.showMenuAsync(PopupMenu::Options().withTargetComponent(&save),[safe](int selected){if(!safe)return;if(selected==1){safe->setPage(2);safe->browser->showSaveCurrent();}else if(selected==2){auto result=safe->browser->overwriteCurrent();if(result.failed())AlertWindow::showMessageBoxAsync(MessageBoxIconType::WarningIcon,"Save preset",result.getErrorMessage());else safe->save.setTooltip("Saved: "+PresetLibrary::currentPresetFile(safe->p).getFileName());}});};load.onClick=[this]{chooseFile(false);};synthTab.onClick=[this]{setMatrix(false);};fxTab.onClick=[this]{setPage(4);};mixTab.onClick=[this]{setPage(3);};matrixTab.onClick=[this]{setMatrix(true);};packsTab.onClick=[this]{setPage(2);};panic.onClick=[this]{p.keyboard.allNotesOff(0);p.panicRequested.store(true);};

  matrixPage=std::make_unique<MatrixPage>(p);addAndMakeVisible(*matrixPage);matrixPage->setBounds(16,78,1208,791);addAndMakeVisible(keyboard);keyboard.setBounds(16,875,1208,82);keyboard.onSequenceRequested=[this](bool arp){setPage(arp?6:5);};sequencePage=std::make_unique<SequencerPage>(p);addAndMakeVisible(*sequencePage);sequencePage->setBounds(16,78,1208,680);oscPage=std::make_unique<OscPage>(p);addAndMakeVisible(*oscPage);oscPage->setBounds(16,78,1208,791);oscPage->onImportInfo=[this]{importInfo.triggerClick();};browser=std::make_unique<PackBrowser>(p);addAndMakeVisible(*browser);browser->setBounds(16,78,1208,680);browser->onImportRequested=[this]{pack.onClick();};browser->onLoadFileRequested=[this]{chooseFile(false);};browser->onAssetsRequested=[this]{assetFolder.onClick();};browser->onInfoRequested=[this]{importInfo.onClick();};browser->onLibraryChanged=[this]{refreshLibrary();};mixPage=std::make_unique<MixPage>(p);addAndMakeVisible(*mixPage);mixPage->setBounds(16,78,1208,534);fxPage=std::make_unique<FxPage>(p);addAndMakeVisible(*fxPage);fxPage->setBounds(16,78,1208,534);mixPage->onOpenRack=[this](int bus){setPage(4);fxPage->selectConnectionRack(bus);};setMatrix(false);auto contentChildren=getChildren();for(auto* child:contentChildren){if(child==&synthTab||child==&mixTab||child==&fxTab||child==&matrixTab||child==&packsTab)continue;child->setTopLeftPosition(child->getX()+92,child->getY());}addAndMakeVisible(sidebar);sidebar.setComponentID("nexum-sidebar");sidebar.setBounds(0,0,92,961);sidebar.setLogo(logo);sidebar.setAccent(Colour(NEXUM_UI_ACCENT));sidebar.toBack();int nav=0;for(auto* tab:{&synthTab,&mixTab,&fxTab,&matrixTab,&packsTab}){tab->getProperties().set("sidebarTab",true);tab->setBounds(0,67+nav++*38,92,38);}oscPage->placeMacrosInSidebar(sidebar);presetStyle.attach(presets,save,previous,next,presetArtist,presetDescription);nxf::layoutPresetHeader(92,1106,presets,save,previous,next,presetArtist,presetDescription);sidebar.refreshMacros();startTimerHz(24);

 }

 int getControlParameterIndex(Component& control)override{for(auto* component=&control;component&&component!=this;component=component->getParentComponent())if(component->getProperties().contains("parameterIndex"))return (int)component->getProperties()["parameterIndex"];return -1;}

 ~SynthEditor()override{stopTimer();if(packWorker)packWorker->stopThread(-1);cancelPendingUpdate();matrixPage.reset();sequencePage.reset();fxPage.reset();mixPage.reset();oscPage.reset();browser.reset();setLookAndFeel(nullptr);}

 bool isInterestedInFileDrag(const StringArray& paths)override{for(const auto& path:paths)if(path.endsWithIgnoreCase(".SerumPack")||path.endsWithIgnoreCase(".SerumPreset")||path.endsWithIgnoreCase(".ncsynth"))return true;return false;}

 void filesDropped(const StringArray& paths,int,int)override{if(paths.size())openFile(File(paths[0]));}

 void paint(Graphics& g)override{nd::Scope suitScope("controls");

  Graphics::ScopedSaveState contentState(g);g.reduceClipRegion(92,0,1240,getHeight());g.setOrigin(92,0);auto imported=std::atomic_load(&p.importedAssets);g.fillAll(nd::colour(background));g.setColour(nd::colour(UiStyle::canvas()));g.fillRect(0,0,getWidth(),67);

  nxf::paintPresetHeader(g,0,1106);


 }

};

juce::AudioProcessorEditor* SynthProcessor::createEditor(){return new SynthEditor(*this);}

}



















