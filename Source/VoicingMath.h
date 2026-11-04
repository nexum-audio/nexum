#pragma once
#include <array>
#include <cmath>
#include <algorithm>
namespace noctrl {
inline float performanceCurve(const std::array<float,16>& points,float input){float at=std::clamp(input,0.f,1.f)*15;int i=std::min(14,(int)at);return std::clamp(points[i]+(at-i)*(points[i+1]-points[i]),0.f,1.f);}
inline float portamentoShape(float t,float curve){return std::pow(std::clamp(t,0.f,1.f),std::exp2(std::clamp(curve,-1.f,1.f)*3));}
struct PortamentoRamp {
 double from=440,target=440,progress=1,step=1;
 void begin(double start,double end,double seconds,bool scaled,double rate){from=std::max(1.,start);target=std::max(1.,end);double interval=std::abs(12*std::log2(target/from));if(scaled)seconds*=interval/12.;progress=seconds>0&&interval>1.e-8?0:1;step=seconds>0?1/std::max(1.,seconds*rate):1;}
 double next(float curve){if(progress>=1)return target;double result=from*std::pow(target/from,portamentoShape((float)progress,curve));progress=std::min(1.,progress+step);return result;}
};
}
