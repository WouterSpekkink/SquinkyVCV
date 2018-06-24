
#pragma once


#include "ObjectCache.h"


/**
 */
template <class TBase>
class GMR : public TBase
{
public:
    GMR(struct Module * module) : TBase(module)
    {
    }
    GMR() : TBase()
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
        NUM_PARAMS
    };

    enum InputIds
    {
        NUM_INPUTS
    };

    enum OutputIds
    {
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

};



template <class TBase>
inline void GMR<TBase>::init()
{
  
}

template <class TBase>
inline void GMR<TBase>::step()
{
 
}

