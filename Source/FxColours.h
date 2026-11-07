#pragma once
#include <NexumDesign.h>
namespace noctrl {
inline juce::Colour fxAccent(const juce::String& name){using juce::Colour;if(name=="Trancegate")return nd::colour(Colour(0xff49d8cf));if(name=="Hyper / Dimension")return nd::colour(Colour(0xff4a9dff));if(name=="Distortion")return nd::colour(Colour(0xfffa6a43));if(name=="Flanger")return nd::colour(Colour(0xff9e6ae8));if(name=="Phaser")return nd::colour(Colour(0xff43e4e7));if(name=="Chorus")return nd::colour(Colour(0xffcf83f1));if(name=="Delay")return nd::colour(Colour(0xff65e648));if(name=="Compressor")return nd::colour(Colour(0xffffca58));if(name=="Reverb")return nd::colour(Colour(0xff53bfff));if(name=="Equalizer")return nd::colour(Colour(0xffb6e859));if(name=="Filter")return nd::colour(Colour(0xff33d6c9));return nd::colour(Colour(NEXUM_UI_ACCENT));}
}
