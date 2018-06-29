
#pragma once

#include "Decimator.h"
#include "LowpassFilter.h"
#include "ObjectCache.h"


/**
 */
template <class TBase>
class LFN : public TBase
{
public:
    LFN(struct Module * module) : TBase(module)
    {
    }
    LFN() : TBase()
    {
    }

    void setSampleRate(float rate)
    {
        reciprocalSampleRate = 1 / rate;
    }

    // must be called after setSampleRate
    void init();

    enum ParamIds
    {
        EQ0_PARAM,
        EQ1_PARAM,
        EQ2_PARAM,
        EQ3_PARAM,
        EQ4_PARAM,
        NUM_PARAMS
    };

    enum InputIds
    {
        NUM_INPUTS
    };

    enum OutputIds
    {
        OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds
    {
        NUM_LIGHTS
    };

    /**
     * Main processing entry point. Called every sample
     */
    void step();

private:
    float reciprocalSampleRate = 0;

    Decimator decimator;
    LowpassFilterState<float> lpfState;
    LowpassFilterParams<float> lpfParams;

    float noise()
    {
        float x = (float) rand() / float(RAND_MAX);
        x -= .5;
        x *= 10;
        return x;
    }
};



template <class TBase>
inline void LFN<TBase>::init()
{
    assert(reciprocalSampleRate > 0);
    // TODO: make sample rate aware
    decimator.setDecimationRate(100 * reciprocalSampleRate);
    LowpassFilter<float>::setCutoff(lpfParams, 50 * reciprocalSampleRate);
}

template <class TBase>
inline void LFN<TBase>::step()
{
    bool needsData;
    float x = decimator.clock(needsData);
    x = LowpassFilter<float>::run(x, lpfState, lpfParams);
    if (needsData) {
        decimator.acceptData(noise());
    }

    TBase::outputs[OUTPUT].value = x;
 
}

