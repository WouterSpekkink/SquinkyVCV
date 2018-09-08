#pragma once

template <class TBase>
class Gray : public TBase
{
public:
    Gray(struct Module * module) : TBase(module)
    {
       // init();
    }
    Gray() : TBase()
    {
       // init();
    }

    enum ParamIds
    {
        PARAM_CLOCK_MUL,
        NUM_PARAMS
    };
  
    enum InputIds
    {
        INPUT_CLOCK
        NUM_INPUTS
    };

    enum OutputIds
    {
        OUTPUT_0,
        OUTPUT_1,
        OUTPUT_2,
        OUTPUT_3,
        OUTPUT_4,
        OUTPUT_5,
        OUTPUT_6,
        OUTPUT_7,

        NUM_OUTPUTS
    };

    enum LightIds
    {
        LIGHT_0,
        LIGHT_1,
        LIGHT_2,
        LIGHT_3,
        LIGHT_4,
        LIGHT_5,
        LIGHT_6,
        LIGHT_7,
        NUM_LIGHTS
    };

    /**
     * Main processing entry point. Called every sample
     */
    void step() override;

private:
    int counterValue = 0;
  
};


template <class TBase>
void  Gray<TBase>::step()
{

} 
