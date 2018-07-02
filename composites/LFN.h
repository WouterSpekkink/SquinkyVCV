
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
 * Noise generator feeding a graphic equalizer.
 * Calculated at very low sample rate, then resampled
 * up to audio rate.
 * 
 * Below assumes 44k SR. TODO: other rates.
 * 
 * We first design the EQ around bands of 100, 200, 400, 800,
 * 1600. EQ gets noise.
 * 
 * Then output of EQ is resampled up by a factor of 100 
 * to bring the first band down to 1hz.
 * or : decimation factor = 100 * (fs) / 44100.
 * 
 * A butterworth lowpass then removes the resmapling artifacts.
 * Otherwise these images bring in high frequencies that we
 * don't want.
 * 
 * Cutoff for the filter can be as low as the top of the eq,
 * which is 3.2khz. 44k/3.2k is about 10,
 * so fc/fs can be 1/1000.
 * 
 * or :   fc = (fs / 44100) / 1000;
 * 
 * (had been using  fc/fs = float(1.0 / (44 * 100.0)));)
 * 
 * Design for R = root freq (was 1 hz, above)
 * EQ first band at E (was 100 hz, above)
 * 
 * Decimation divider = E / R
 * 
 * Imaging filter fc = 3.2khz / decimation-divider
 * fc/fs = 3200 * (reciprocal sr) / decimation-divider.
 * 
 * Experiment: let's use those values and compare to what we had been using.
 * result: not too far off.
 * 
 * make a range/base control. map -5 to +5 into 1/10 hz to 2 hz rate. Can use regular
 * functions, since we won't calc that often.
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
        //reciprocalSampleRate = 1 / rate;
        reciprocalSampleRate = engineGetSampleTime();
        init();
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
        FREQ_RANGE_PARAM,
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

#ifdef _GEQ2
    GraphicEq2<5> geq;
#else
    GraphicEq geq;
#endif
    BiquadParams<float, 2> lpfParams;
    BiquadState<float, 2> lpfState;
    float baseFrequency = 1;
    float lastBaseFrequencyParamValue = -100;

 //   static std::function<double(double)> makeFunc_Exp(double xMin, double xMax, double yMin, double yMax);
    
    std::function<double(double)> rangeFunc;




    float noise()
    {
        float x = (float) rand() / float(RAND_MAX);
        x -= .5;
        x *= 10;
        return x;
    }
};


/**
 * re-calc everything that changes with sample
 * rate. Also everything that depends on baseFrequency.
 * 
 * TODO: break up into one time, and onBaseFreq()
 */
template <class TBase>
inline void LFN<TBase>::init()
{
    assert(reciprocalSampleRate > 0);

    // map knob range from .1 Hz to 2.0 Hz
    rangeFunc = AudioMath::makeFunc_Exp(-5, 5, .1, 2);
    
 
    // decimation must be 100hz (what our eq is designed at)
    // divided by base.
    const float decimationDivider = 100.0 / baseFrequency;
    decimator.setDecimationRate(decimationDivider);

    // Imaging filter fc = 3.2khz / decimation-divider
    // fc/fs = 3200 * (reciprocal sr) / decimation-divider.
    const float lpFc = 3200 * reciprocalSampleRate / decimationDivider; 
    ButterworthFilterDesigner<float>::designThreePoleLowpass(
        lpfParams, lpFc);

    printf("\n**** setting lpf to %f, used to be %f\n", lpFc, float(1.0 / (44 * 100.0))); 
        //
       // lpfParams, float(1.0 / (44 * 100.0)));
 
}

template <class TBase>
inline void LFN<TBase>::step()
{
    if (lastBaseFrequencyParamValue != TBase::params[FREQ_RANGE_PARAM].value) {
        lastBaseFrequencyParamValue = TBase::params[FREQ_RANGE_PARAM].value;
        baseFrequency = rangeFunc(lastBaseFrequencyParamValue);
        init();
    }
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

