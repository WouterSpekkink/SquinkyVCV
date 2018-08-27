#pragma once

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
};
