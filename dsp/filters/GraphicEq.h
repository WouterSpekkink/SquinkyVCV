#pragma once

#include "StateVariableFilter.h"

class GraphicEq
{
public:

    GraphicEq(int stages, float bw);

    float run(float);
private:
    StateVariableFilterParams<float> params[6];
    StateVariableFilterState<float> states[6];
    const int _stages;

};

// todo: refactor
inline GraphicEq::GraphicEq(int stages, float bw) : _stages(stages)
{
    assert(stages < 6);
    // .5, 1 stage is 78..128. 2stage 164 273 / 
    // .8  67..148 /  63..314  / 72..456
    const float baseFreq = 100.f / 44100.f;
    float freq = baseFreq;
    for (int i = 0; i < stages; ++i) {
        params[i].setMode(StateVariableFilterParams<float>::Mode::BandPass);
        params[i].setFreq(freq);
        params[i].setNormalizedBandwidth(bw);
        freq *= 2.f;

    }
}

inline float GraphicEq::run(float input)
{
    float out = 0;
    for (int i = 0; i < _stages; ++i) {
        out += StateVariableFilter<float>::run(input, states[i], params[i]);
    }
    return out;
}