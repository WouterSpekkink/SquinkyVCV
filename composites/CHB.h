
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

    /** 
     * The waveshaper this is the heart of this module
     */
    Poly<11> poly;  

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
    SinOscillator<float, false>::setFrequency(sinParams, 500 * reciprocalSampleRate);
}

template <class TBase>
inline void CHB<TBase>::step()
{
    float input = SinOscillator<float, false>::run(sinState, sinParams);
    float output = poly.run(input);
    TBase::outputs[OUTPUT].value = output;
}

