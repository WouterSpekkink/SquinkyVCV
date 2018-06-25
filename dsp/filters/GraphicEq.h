#pragma once

#include "StateVariableFilter.h"

class GraphicEq
{
public:
    const static int numStages = 3;
    GraphicEq();

    float run(float);
private:
    StateVariableFilterParams<float> params[numStages];
    StateVariableFilterState<float> states[numStages];

};

// todo: refactor
inline GraphicEq::GraphicEq()
{
    // .5, 1 stage is 78..128. 2stage 164 273 / 
    // .8  67..148 /  63..314  / 72..456
    const float bw = .8f;
    const float baseFreq = 100.f / 44100.f;
    for (int i = 0; i < numStages; ++i) {
        params[i].setMode(StateVariableFilterParams<float>::Mode::BandPass);
        params[i].setFreq(baseFreq * (i + 1));
        params[i].setNormalizedBandwidth(bw);
    }
}

inline float GraphicEq::run(float input)
{
    float out = 0;
    for (int i = 0; i < numStages; ++i) {
        out += StateVariableFilter<float>::run(input, states[i], params[i]);
    }
    return out;
}