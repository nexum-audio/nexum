#pragma once
#include "AssetBrowser.h"
namespace noctrl {
class WavetableBrowser:public AssetBrowser {
public:
 WavetableBrowser(SynthProcessor& p,int osc,juce::File root=WavetableLibrary::root()):AssetBrowser(p,osc,AssetKind::Wavetable,root){}
 static juce::DialogWindow* open(SynthProcessor& p,int osc,juce::Component* parent){return AssetBrowser::open(p,osc,AssetKind::Wavetable,parent);}
};
class MultisampleBrowser:public AssetBrowser {
public:
 MultisampleBrowser(SynthProcessor& p,int osc,juce::File root={}):AssetBrowser(p,osc,AssetKind::Multisample,root){}
 static juce::DialogWindow* open(SynthProcessor& p,int osc,juce::Component* parent){return AssetBrowser::open(p,osc,AssetKind::Multisample,parent);}
};
}
