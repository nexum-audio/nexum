#pragma once
#include "Processor.h"
namespace noctrl {
inline juce::StringArray noisePresetNames(){return {"White Air","Pink Silk","Brown Earth","Blue Spark","Violet Fizz","Soft Air","Bright Hiss","Deep Rumble","Storm Wind","Ocean Wash","Rainfall","Vinyl Crackle","Tape Hiss","Digital Dust","Dusty Static","Metal Shimmer","Steam Vent","Fire Crackle","Shaker Sand","Cyber Texture"};}
inline bool makeNoisePreset(int preset,juce::MemoryBlock& bytes,juce::String& error){
 using namespace juce;if(preset<0||preset>=20){error="Unknown noise preset.";return false;}constexpr int rate=48000,length=rate*3;AudioBuffer<float> audio(2,length);
 for(int channel=0;channel<2;++channel){Random random(0x4e6f4354+97*preset+channel*7919);float low=0,slow=0,last=0,previousHigh=0,brown=0,crackle=0,pink[3]={};
  for(int i=0;i<length;++i){float white=random.nextFloat()*2-1;low+=.035f*(white-low);slow+=.003f*(white-slow);brown=jlimit(-1.f,1.f,brown*.999f+white*.018f);float high=white-last,v=white;last=white;pink[0]=.998f*pink[0]+white*.025f;pink[1]=.97f*pink[1]+white*.08f;pink[2]=.7f*pink[2]+white*.2f;float pinkValue=(pink[0]+pink[1]+pink[2])*.6f;float t=i/(float)rate;
   switch(preset){case 0:v=white;break;case 1:v=pinkValue;break;case 2:v=brown;break;case 3:v=high*.5f;break;case 4:v=(high-previousHigh)*.25f;break;case 5:v=low*3;break;case 6:v=(white-low)*.7f;break;case 7:v=slow*7;break;case 8:v=(low*2+slow*4)*(.55f+.4f*std::sin(MathConstants<float>::twoPi*.7f*t));break;case 9:v=pinkValue*(.5f+.45f*std::sin(MathConstants<float>::twoPi*t/3));break;case 10:v=high*.22f+low*2.5f;break;
    case 11:crackle*=.91f;if(random.nextFloat()<.0013f)crackle=(random.nextFloat()*2-1)*1.3f;v=white*.035f+crackle;break;
    case 12:v=white*.35f+pinkValue*.15f;break;case 13:v=std::round(white*7)/7*.6f;break;case 14:crackle*=.72f;if(random.nextFloat()<.008f)crackle=white;v=low*.7f+crackle*.7f;break;
    case 15:v=high*(.4f+.3f*std::sin(MathConstants<float>::twoPi*1733*t))*.5f;break;case 16:v=(white-low)*(.45f+.18f*std::sin(MathConstants<float>::twoPi*4*t));break;
    case 17:crackle*=.97f;if(random.nextFloat()<.003f)crackle=white*1.6f;v=low*1.4f+crackle;break;case 18:v=high*.38f*std::exp(-12.f*std::fmod(t, .25f));break;case 19:v=(high*.3f+slow*2)*(.25f+.65f*(std::sin(MathConstants<float>::twoPi*11*t)>0?1.f:0.f));break;
   }previousHigh=high;audio.setSample(channel,i,v);
  }
 }
 // Remove DC, match useful peaks and soften the loop seam.
 float peak=0;for(int channel=0;channel<2;++channel){double mean=0;for(int i=0;i<length;++i)mean+=audio.getSample(channel,i);float dc=(float)(mean/length);for(int i=0;i<length;++i){float edge=jmin(1.f,jmin(i,(length-1)-i)/480.f);float v=(audio.getSample(channel,i)-dc)*edge;audio.setSample(channel,i,v);peak=jmax(peak,std::abs(v));}}if(peak>0)audio.applyGain(.85f/peak);
 bytes.reset();WavAudioFormat format;auto stream=std::make_unique<MemoryOutputStream>(bytes,false);std::unique_ptr<AudioFormatWriter> writer(format.createWriterFor(stream.release(),rate,2,24,{},0));if(!writer||!writer->writeFromAudioSampleBuffer(audio,0,length)){error="Could not create noise sample.";return false;}writer.reset();return true;
}
}
