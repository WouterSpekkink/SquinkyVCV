#pragma once


#include "FunVCO.h"

template <class TBase>
class FunVCOComposite : public TBase
{
public:
    enum ParamIds
    {
        MODE_PARAM,
        SYNC_PARAM,
        FREQ_PARAM,
        FINE_PARAM,
        FM_PARAM,
        PW_PARAM,
        PWM_PARAM,
        NUM_PARAMS
    };
    enum InputIds
    {
        PITCH_INPUT,
        FM_INPUT,
        SYNC_INPUT,
        PW_INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        SIN_OUTPUT,
        TRI_OUTPUT,
        SAW_OUTPUT,
        SQR_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        PHASE_POS_LIGHT,
        PHASE_NEG_LIGHT,
        NUM_LIGHTS
    };


    void step() override;

    void setSampleRate(float rate)
    {
        oscillator.sampleTime = 1.f / rate;
    }

private:
    VoltageControlledOscillator<16,16> oscillator;
};

template <class TBase>
inline void FunVCOComposite<TBase>::step()
{
    oscillator.analog = TBase::params[MODE_PARAM].value > 0.0f;
    oscillator.soft = TBase::params[SYNC_PARAM].value <= 0.0f;

    float pitchFine = 3.0f * quadraticBipolar(TBase::params[FINE_PARAM].value);
    float pitchCv = 12.0f * TBase::inputs[PITCH_INPUT].value;
    if (TBase::inputs[FM_INPUT].active) {
        pitchCv += quadraticBipolar(TBase::params[FM_PARAM].value) * 12.0f * TBase::inputs[FM_INPUT].value;
    }
    oscillator.setPitch(TBase::params[FREQ_PARAM].value, pitchFine + pitchCv);
    oscillator.setPulseWidth(TBase::params[PW_PARAM].value + TBase::params[PWM_PARAM].value * TBase::inputs[PW_INPUT].value / 10.0f);
    oscillator.syncEnabled = TBase::inputs[SYNC_INPUT].active;

    oscillator.process(TBase::engineGetSampleTime(), TBase::inputs[SYNC_INPUT].value);

    // Set output
    if (TBase::outputs[SIN_OUTPUT].active)
        TBase::outputs[SIN_OUTPUT].value = 5.0f * oscillator.sin();
    if (TBase::outputs[TRI_OUTPUT].active)
        TBase::outputs[TRI_OUTPUT].value = 5.0f * oscillator.tri();
    if (TBase::outputs[SAW_OUTPUT].active)
        TBase::outputs[SAW_OUTPUT].value = 5.0f * oscillator.saw();
    if (TBase::outputs[SQR_OUTPUT].active)
        TBase::outputs[SQR_OUTPUT].value = 5.0f * oscillator.sqr();

    TBase::lights[PHASE_POS_LIGHT].setBrightnessSmooth(fmaxf(0.0f, oscillator.light()));
    TBase::lights[PHASE_NEG_LIGHT].setBrightnessSmooth(fmaxf(0.0f, -oscillator.light()));
}