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

    const float bw = .5;
    const float baseFreq = 100.f / 44100.f;
    for (int i = 0; i < numStages; ++i) {
        params[i].setMode(StateVariableFilterParams<float>::Mode::BandPass);
        params[i].setFreq(baseFreq * (i + 1));
    }
}

inline float GraphicEq::run(float input)
{
    float in = input;
    float out = 0;
    for (int i = 0; i < numStages; ++i) {
        out = StateVariableFilter<float>::run(in, states[i], params[i]);
        in = out;
    }
    return out;
}