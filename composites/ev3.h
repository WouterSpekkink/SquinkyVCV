#pragma once

#include "MinBLEPVCO.h"
#include "ObjectCache.h"

/**
*/
template <class TBase>
class EV3 : public TBase
{
public:
    EV3(struct Module * module) : TBase(module)
    {
        init();
    }
    
    EV3() : TBase()
    {
        init();
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
    void stepVCOs();
    void init();

    MinBLEPVCO vcos[3];
    std::function<float(float)> expLookup = ObjectCache<float>::getExp2Ex();
};

template <class TBase>
inline void EV3<TBase>::step()
{
    processPitchInputs();
    stepVCOs();
}

template <class TBase>
inline void EV3<TBase>::stepVCOs()
{
    for (int i = 0; i < 3; ++i) {
        vcos[i].step();
    }
    // route waveform outputs?
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
    const int delta = osc * (OCTAVE2_PARAM - OCTAVE1_PARAM);

    float pitch = 1.0f + roundf(TBase::params[OCTAVE1_PARAM + delta].value) +
        TBase::params[SEMI1_PARAM + delta].value / 12.0f +
        TBase::params[FINE1_PARAM + delta].value;

#if 0
    pitch += TBase::inputs[CV_INPUT].value;
    pitch += .25f * TBase::inputs[PITCH_MOD_INPUT].value *
        taper(TBase::params[PARAM_PITCH_MOD_TRIM].value);
#endif

    const float q = float(log2(261.626));       // move up to pitch range of even vco
    pitch += q;
    const float freq = expLookup(pitch);
    vcos[osc].setNormalizedFreq(engineGetSampleTime() * freq);
}

template <class TBase>
inline void EV3<TBase>::init()
{
    for (int i = 0; i < 3; ++i) {
        vcos[i].enableWaveform(MinBLEPVCO::Waveform::Saw, true);
        vcos[i].enableWaveform(MinBLEPVCO::Waveform::Tri, false);
        vcos[i].enableWaveform(MinBLEPVCO::Waveform::Sin, false);
        vcos[i].enableWaveform(MinBLEPVCO::Waveform::Square, false);
        vcos[i].enableWaveform(MinBLEPVCO::Waveform::Even, false);
    }
}

