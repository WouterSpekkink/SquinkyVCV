
#pragma once

#include "poly.h"
#include "ObjectCache.h"
#include "SinOscillator.h"


/**
 */
template <class TBase>
class CHB : public TBase
{
public:
    CHB(struct Module * module) : TBase(module)
    {
    }
    CHB() : TBase()
    {
    }

    void setSampleTime(float time)
    {
        reciprocalSampleRate = time;
        internalUpdate();
    }

    // must be called after setSampleRate
    void init();
    

    enum ParamIds
    {
        PARAM_PITCH,
        PARAM_H0,
        PARAM_H1,
        PARAM_H2,
        PARAM_H3,
        PARAM_H4,
        PARAM_H5,

        NUM_PARAMS
    };

    enum InputIds
    {
        CV_INPUT,
        ENV_INPUT,
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

    /** 
     * The waveshaper this is the heart of this module
     */
    Poly<double, 11> poly;  

    /**
     * Internal sine wave oscillator to drive the waveshaper
     */        
    SinOscillatorParams<float> sinParams;
    SinOscillatorState<float> sinState;

    std::shared_ptr<LookupTableParams<float>> pitchExp = {ObjectCache<float>::getExp2()};

    void internalUpdate();

};



template <class TBase>
inline void CHB<TBase>::init()
{
    internalUpdate();
}

template <class TBase>
inline void CHB<TBase>::internalUpdate()
{
    // for now, just run at 500 hz
    SinOscillator<float, false>::setFrequency(sinParams, 150 * reciprocalSampleRate);
}

template <class TBase>
inline void CHB<TBase>::step()
{
    // Get the frequency from the inputs.
    float pitch = TBase::params[PARAM_PITCH].value + TBase::inputs[CV_INPUT].value;
    pitch = std::max(-5.0f, pitch);
    pitch = std::min(5.0f, pitch);

    pitch += 5;     // push it up to reasonable range
    // lookup give abs Hz, so div by sample rate for normalized freq
    float frequency = LookupTable<float>::lookup(*pitchExp, pitch) * reciprocalSampleRate;
    SinOscillator<float, false>::setFrequency(sinParams, frequency);

    // Get the gain from the inputs.
    float gain = TBase::inputs[ENV_INPUT].active ? TBase::inputs[ENV_INPUT].value : 10.f;
    gain *= 0.1f;

   // printf("pitch = %f freq = %f gain = %f\n", pitch, frequency, gain);

    for (int i = 0; i < 6; ++i) {
        float val = TBase::params[i + PARAM_H0].value;
        poly.setGain(i, val);
    }
   
    float input = SinOscillator<float, false>::run(sinState, sinParams);
    float output = poly.run(input * gain);
    TBase::outputs[OUTPUT].value = output;
}

