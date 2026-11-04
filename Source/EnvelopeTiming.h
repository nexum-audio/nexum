#pragma once
#include <juce_core/juce_core.h>
#include <array>
#include <cmath>
namespace noctrl {
inline const juce::StringArray& envelopeBeatNames(){static const juce::StringArray names{"0","1/64T","1/64","1/32T","1/64D","1/32","1/16T","1/32D","1/16","1/8T","1/16D","1/8","1/4T","1/8D","1/4","1/2T","1/4D","1/2","1/1T","1/2D","1 bar","2 bars","4 bars"};return names;}
inline constexpr std::array<float,23> envelopeBeats{0,1.f/24,1.f/16,1.f/12,3.f/32,1.f/8,1.f/6,3.f/16,1.f/4,1.f/3,3.f/8,.5f,2.f/3,.75f,1,4.f/3,1.5f,2,8.f/3,3,4,8,16};
inline int nearestEnvelopeBeat(float beats){int best=0;float distance=1.e20f;for(int i=0;i<(int)envelopeBeats.size();++i){float d=std::abs(beats-envelopeBeats[i]);if(d<distance){best=i;distance=d;}}return best;}
inline float envelopeBeatSeconds(int index,double tempo){return envelopeBeats[juce::jlimit(0,22,index)]*60.f/(float)juce::jlimit(20.,400.,tempo);}
}
