#pragma once

#include "IIRUpsampler.h"
#include "IIRDecimator.h"

template <class TBase>
class Shaper : public TBase
{
public:
    Shaper(struct Module * module) : TBase(module)
    {
       init();
    }
    Shaper() : TBase()
    {
       init();
    }

    enum ParamIds
    {
        NUM_PARAMS
    };
  
    enum InputIds
    {
        INPUT_AUDIO,
        NUM_INPUTS
    };

    enum OutputIds
    {
        OUTPUT_AUDIO,
        NUM_OUTPUTS
    };

    enum LightIds
    {

        NUM_LIGHTS
    };

    /**
     * Main processing entry point. Called every sample
     */
    void step() override;

private:

    const static int oversample = 16;
    void init();
    IIRUpsampler<oversample> up;
    IIRDecimator<oversample> dec;
  
};


template <class TBase>
void  Shaper<TBase>::init()
{
   // clock.setMultiplier(1); // no mult
   float fc = .25 / float(oversample);
   up.setCutoff(fc);
   dec.setCutoff(fc);
}

template <class TBase>
void  Shaper<TBase>::step()
{
    float buffer[oversample];
    const float input = TBase::inputs[INPUT_AUDIO].value;
    up.process(buffer, input);


    for (int i=0; i<oversample; ++i) {
        float x = buffer[i];
        x = std::min(1.f, x);
        x = std::max(-1.f, x);
        buffer[i] = x;
    }

    const float output = dec.process(buffer);
    TBase::outputs[OUTPUT_AUDIO].value = output;
} 
