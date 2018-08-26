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
