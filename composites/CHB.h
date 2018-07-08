
#pragma once

#include "poly.h"
#include "ObjectCache.h"


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
    Poly<11> poly;

};



template <class TBase>
inline void CHB<TBase>::init()
{

}

template <class TBase>
inline void CHB<TBase>::step()
{

}

