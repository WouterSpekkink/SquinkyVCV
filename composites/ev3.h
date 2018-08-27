#pragma once

#include "MinBLEPVCO.h"
/**
*/
template <class TBase>
class EV3 : public TBase
{
public:
    EV3(struct Module * module) : TBase(module)
    {
    }
    
    EV3() : TBase()
    {
    }
    

    enum ParamIds
    {
        OCTAVE1_PARAM,
        SEMI1_PARAM,
        FINE1_PARAM,
        SYNC1_PARAM,

        OCTAVE2_PARAM,
        SEMI2_PARAM,
        FINE2_PARAM,
        SYNC2_PARAM,

        OCTAVE3_PARAM,
        SEMI3_PARAM,
        FINE3_PARAM,
        SYNC3_PARAM,

        NUM_PARAMS
    };
 
    enum InputIds
    {
        NUM_INPUTS
    };

    enum OutputIds
    {
        MIX_OUTPUT,
        NUM_OUTPUTS
    };

    enum LightIds
    {
        NUM_LIGHTS
    };

    void step() override;
private:
    void processPitchInputs();
    void processPitchInputs(int osc);

    MinBLEPVCO vcos[3];


};

template <class TBase>
inline void EV3<TBase>::step()
{
    processPitchInputs();
}


template <class TBase>
inline void EV3<TBase>::processPitchInputs()
{
    processPitchInputs(0);
    processPitchInputs(1);
    processPitchInputs(2);
}

template <class TBase>
inline void EV3<TBase>::processPitchInputs(int osc)
{
}

