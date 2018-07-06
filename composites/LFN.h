
#pragma once

#include "ButterworthFilterDesigner.h"
#include "Decimator.h"
#include "GraphicEq.h"
#include "LowpassFilter.h"
#include "BiquadParams.h"
#include "BiquadState.h"
#include "BiquadFilter.h"
#include "ObjectCache.h"
#include <random>

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
 *
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
        // TODO: just use rate everywhere.
        reciprocalSampleRate = 1 / rate;
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
        EQ0_INPUT,
        EQ1_INPUT,
        EQ2_INPUT,
        EQ3_INPUT,
        EQ4_INPUT,
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

    float getBaseFrequency() const
    {
        return baseFrequency;
    }
private:
    float reciprocalSampleRate = 0;

    Decimator decimator;

    GraphicEq2<5> geq;
    using TButter = double;

    BiquadParams<TButter, 2> lpfParams;
    BiquadState<TButter, 2> lpfState;

    float baseFrequency = 1;
    float lastBaseFrequencyParamValue = -100;

    std::default_random_engine generator{57};  // 12345
    std::normal_distribution<double> distribution{-1.0, 1.0};
    float noise()
    {
        return  (float) distribution(generator);
    }

    std::function<double(double)> rangeFunc;
    AudioMath::SimpleScaleFun<float> gainScale = {AudioMath::makeSimpleScalerAudioTaper(0, 1)};
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
    const float decimationDivider = float(100.0 / baseFrequency);
    decimator.setDecimationRate(decimationDivider);

    // Imaging filter fc = 3.2khz / decimation-divider
    // fc/fs = 3200 * (reciprocal sr) / decimation-divider.
    const float lpFc = 3200 * reciprocalSampleRate / decimationDivider;
    ButterworthFilterDesigner<TButter>::designThreePoleLowpass(
        lpfParams, lpFc);

}

template <class TBase>
inline void LFN<TBase>::step()
{
    // TODO: this will pop. consider doing something better.
    if (lastBaseFrequencyParamValue != TBase::params[FREQ_RANGE_PARAM].value) {
        lastBaseFrequencyParamValue = TBase::params[FREQ_RANGE_PARAM].value;
        baseFrequency = float(rangeFunc(lastBaseFrequencyParamValue));
        init();
    }
    const int numEqStages = geq.getNumStages();
    for (int i = 0; i < numEqStages; ++i) {
        auto paramNum = i + EQ0_PARAM;
        auto cvNum = i + EQ0_INPUT;
        const float gainParamKnob = TBase::params[paramNum].value;
        const float gainParamCV = TBase::inputs[cvNum].value;
        const float gain = gainScale(gainParamKnob, gainParamCV);
        //printf("gain[%d]=%f (%f,%f)\n", i, gain, gainParamKnob, gainParamCV);
        geq.setGain(i, gain);
    }

#if 1 // normal way (with decimator)
    bool needsData;
    TButter x = decimator.clock(needsData);
    x = BiquadFilter<TButter>::run(x, lpfState, lpfParams);
    if (needsData) {
        const float z = geq.run(noise());
        decimator.acceptData(z);
}
#else
    TButter x = BiquadFilter<TButter>::run(noise(), lpfState, lpfParams);
#endif
    TBase::outputs[OUTPUT].value = (float) x;
}

