#pragma once
#include "PresetLibrary.h"
namespace noctrl {
inline juce::PopupMenu presetSaveMenu(SynthProcessor& processor){juce::PopupMenu menu;menu.addItem(1,"Create new preset");menu.addItem(2,juce::String::fromUTF8("Overwrite preset"),PresetLibrary::canOverwriteCurrent(processor));return menu;}
}
