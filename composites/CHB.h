
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
    for (int i = 0; i < 6; ++i) {
        float val = TBase::params[i + PARAM_H0].value;
        poly.setGain(i, val);
    }
    float input = SinOscillator<float, false>::run(sinState, sinParams);
    float output = poly.run(input);
    TBase::outputs[OUTPUT].value = output;
}

