
#pragma once

#include "ButterworthFilterDesigner.h"
#include "Decimator.h"
#include "GraphicEq.h"
#include "LowpassFilter.h"
#include "BiquadParams.h"
#include "BiquadState.h"
#include "BiquadFilter.h"
#include "ObjectCache.h"

#define _GEQ2
/**
 */
template <class TBase>
class LFN : public TBase
{
public:
#ifdef _GEQ2
    LFN(struct Module * module) : TBase(module)
    {
    }
    LFN() : TBase()
    {
    }
#else
    const int numEqStages=5;
    LFN(struct Module * module) : TBase(module), geq(numEqStages, .6f)
    {
    }
    LFN() : TBase(), geq(numEqStages, .6f)
    {
    }
#endif

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
   // LowpassFilterState<float> lpfState;
  //  LowpassFilterParams<float> lpfParams;

#ifdef _GEQ2
    GraphicEq2<5> geq;
#else
    GraphicEq geq;
#endif
    BiquadParams<float, 2> lpfParams;
    BiquadState<float, 2> lpfState;

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
    decimator.setDecimationRate(100);

    
    ButterworthFilterDesigner<float>::designThreePoleLowpass(
        lpfParams, float(1.0 / (44 * 100.0)));
   // LowpassFilter<float>::setCutoff(lpfParams, 50 * reciprocalSampleRate);
}

template <class TBase>
inline void LFN<TBase>::step()
{
    /*
     EQ0_PARAM,
        EQ1_PARAM,
        EQ2_PARAM,
        EQ3_PARAM,
        EQ4_PARAM,
        */
    const int numEqStages = geq.getNumStages();
    for (int i=0; i<numEqStages; ++i) {
        auto paramNum = i + EQ0_PARAM;
        const float gain = TBase::params[paramNum].value;
        geq.setGain(i, gain);
    }


    bool needsData;
    float x = decimator.clock(needsData);
   // printf("need = %d\n", needsData);
 //   x = LowpassFilter<float>::run(x, lpfState, lpfParams);
    x = BiquadFilter<float>::run(x, lpfState, lpfParams);
    if (needsData) {
        float z = noise();
        z = geq.run(z);
        decimator.acceptData(z);
    }

    //x = geq.run(x);

    TBase::outputs[OUTPUT].value = x;
 
}

