
#pragma once

#include <algorithm>
#include "poly.h"
#include "ObjectCache.h"
#include "SinOscillator.h"

// until c++17
namespace std {
    inline float clamp(float v, float lo, float hi)
    {
        assert(lo < hi);
        return std::min(hi, std::max(v, lo));
    }
}
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

#if 0
    void setSampleTime(float time)
    {
        reciprocalSampleRate = time;
        internalUpdate();
    }
#endif

    // must be called after setSampleRate
 //   void init();
    

    enum ParamIds
    {
        PARAM_TUNE,
        PARAM_OCTAVE,
        PARAM_EXTGAIN,
        PARAM_PITCH_MOD_TRIM,
        PARAM_LINEAR_FM_TRIM,
        PARAM_FOLD,
        PARAM_SLOPE,
        PARAM_MAG_EVEN,
        PARAM_MAG_ODD,
        PARAM_H0,
        PARAM_H1,
        PARAM_H2,
        PARAM_H3,
        PARAM_H4,
        PARAM_H5,
        PARAM_H6,
        PARAM_H7,
        PARAM_H8,
        PARAM_H9,
        PARAM_H10,

        NUM_PARAMS
    };

    enum InputIds
    {
        CV_INPUT,
        PITCH_MOD_INPUT,
        LINEAR_FM_INPUT,
        ENV_INPUT,
        AUDIO_INPUT,
        SLOPE_INPUT,
        H0_INPUT,
        H1_INPUT,
        H2_INPUT,
        H3_INPUT,
        H4_INPUT,
        H5_INPUT,
        H6_INPUT,
        H7_INPUT,
        H8_INPUT,
        H9_INPUT,
        H10_INPUT,
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

    /**
     * Main processing entry point. Called every sample
     */
    void step() override;

    float _freq=0;

private:


  //  float reciprocalSampleRate = 0;

    /** 
     * The waveshaper this is the heart of this module
     */
    Poly<double, 11> poly;  

    /**
     * Internal sine wave oscillator to drive the waveshaper
     */        
    SinOscillatorParams<float> sinParams;
    SinOscillatorState<float> sinState;

    // just maps 0..1 to 0..1
    std::shared_ptr<LookupTableParams<float>> audioTaper = {ObjectCache<float>::getAudioTaper()};

    // TODO: use more accurate lookup
   // std::shared_ptr<LookupTableParams<float>> pitchExp = {ObjectCache<float>::getExp2()};
    std::function<float(float)> expLookup  = ObjectCache<float>::getExp2Ex();

    /**
    * Audio taper for the slope.
    */
    AudioMath::ScaleFun<float> slopeScale =
        {AudioMath::makeLinearScaler<float>(-18, 0)};

    /**
     * do one-time calculations when sample rate changes
     */
    void internalUpdate();


    /**
     * Do all the processing to get the input waveform
     * that will be fed to the polynomials
     */
    float getInput();

    void calcVolumes(float *);

    float taper(float raw)
    {
       return LookupTable<float>::lookup(*audioTaper, raw, false);
    }

};


#if 0
template <class TBase>
inline void CHB<TBase>::init()
{
    internalUpdate();
}

template <class TBase>
inline void CHB<TBase>::internalUpdate()
{
    // for now, just run at 150 hz
    SinOscillator<float, false>::setFrequency(sinParams, 150 * reciprocalSampleRate);
}
#endif

template <class TBase>
inline float CHB<TBase>::getInput()
{

    assert(TBase::engineGetSampleTime() > 0);
    /*
        float pitch = 1.0f + roundf(vco.params[(int) CH::OCTAVE_PARAM].value) + vco.params[(int) CH::TUNE_PARAM].value / 12.0f;
    pitch += vco.inputs[(int) EVCO::PITCH_INPUT].value;
    pitch += vco.inputs[(int) EVCO::PITCH_MOD_INPUT].value / 4.0f;

    */
    // Get the frequency from the inputs.


    float pitch = 1.0f + roundf(TBase::params[PARAM_OCTAVE].value) + TBase::params[PARAM_TUNE].value / 12.0f;
    pitch += TBase::inputs[CV_INPUT].value;
    pitch += .25f * TBase::inputs[PITCH_MOD_INPUT].value *
        taper(TBase::params[PARAM_PITCH_MOD_TRIM].value);

    // TODO: smarter limiting
  //  pitch = std::max(-5.0f, pitch);
  //  pitch = std::min(5.0f, pitch);

 //   pitch += 5;     // push it up to reasonable range
                    // lookup give abs Hz, so div by sample rate for normalized freq
    const float q = float(log2(261.626));       // move up to pitch range of even vco
    pitch += q;
 //   _freq = LookupTable<float>::lookup(*pitchExp, pitch);
    _freq = expLookup(pitch);

    // Multiply in the Linear FM contribution
    _freq *= 1.0f + TBase::inputs[LINEAR_FM_INPUT].value * taper(TBase::params[PARAM_LINEAR_FM_TRIM].value);

    float time = std::clamp(_freq * TBase::engineGetSampleTime(), 1e-6f, 0.5f);
    
  //  float time = _freq * engineGetSampleTime();
  //  time = std::min(time, .4999f);


    SinOscillator<float, false>::setFrequency(sinParams, time);

    // Get the gain from the envelope generator in
    float EGgain = TBase::inputs[ENV_INPUT].active ? TBase::inputs[ENV_INPUT].value : 10.f;
    EGgain *= 0.1f;

    const bool isExternalAudio = TBase::inputs[AUDIO_INPUT].active;

    const float gainKnobValue = TBase::params[PARAM_EXTGAIN].value;
    float knobGain = 1;
    if (isExternalAudio) {
        knobGain = gainKnobValue;           // TODO: taper
    } else {
        // if using internal sin, never attenuate
        if (gainKnobValue > 0) {
            knobGain = 1 + gainKnobValue;
        }
    }

    // printf("pitch = %f freq = %f gain = %f\n", pitch, frequency, gain);
    float input = EGgain * knobGain * (isExternalAudio ?
        TBase::inputs[AUDIO_INPUT].value :
        SinOscillator<float, false>::run(sinState, sinParams));

    // Now clip or fold to keep in -1...+1
    if (TBase::params[PARAM_FOLD].value > .5) {
        input = AudioMath::fold(input);
    } else {
        input = std::max(input, -1.f);
        input = std::min(input, 1.f);
    }
    return input;
}

template <class TBase>
inline void CHB<TBase>::calcVolumes(float * volumes)
{
    // first get the harmonics knobs, and scale them
    for (int i = 0; i < 11; ++i) {
       // float rawVal = TBase::params[i + PARAM_H0].value;
#if 0
        float val = taper(TBase::params[i + PARAM_H0].value);
#else
        const float val = TBase::inputs[i + H0_INPUT].active ?
            TBase::inputs[i + H0_INPUT].value * .1f :
            taper(TBase::params[i + PARAM_H0].value);
#endif

        volumes[i] = val;
    }
   // printf("knob 0=%f, 3 = %f 4=%f 10=%f\n", volumes[0], volumes[3], volumes[4], volumes[10]);

    // Second: apply the even and odd knobs
    {
        const float even = taper(TBase::params[PARAM_MAG_EVEN].value);
        const float odd = taper(TBase::params[PARAM_MAG_ODD].value);
      //  printf("even = %f odd  = %f\n", even, odd);
        for (int i = 1; i < 11; ++i) {
            const float mul = (i & 1) ? even : odd;     // 0 = fundamental, 1=even, 2=odd....
            volumes[i] *= mul;
        }
    }

    // Third: slope

    float octave[11];
    for (int i = 0; i < 11; ++i) {
        octave[i] = log2(float(i+1));
    }

    {
#if 0
        const float slopeRaw = TBase::params[PARAM_SLOPE].value;
        assert(slopeRaw >= 0 && slopeRaw <= 1);
        const float slope = - slopeRaw * 18;
      //  printf("slope raw = %f slope = %f\n", slopeRaw, slope);
#endif
        // TODO: add attenuverter, or make a simple linear scale
        const float slope = slopeScale(TBase::params[PARAM_SLOPE].value, TBase::inputs[SLOPE_INPUT].value, 1);
      
        for (int i = 0; i < 11; ++i) {
            float slopeAttenDb = slope * octave[i];
            float slopeAtten = (float) AudioMath::gainFromDb(slopeAttenDb);
            volumes[i] *= slopeAtten;
          //  printf("i=%d, oct=%.2f slopedb=%.2f atten=%.2f\n", i, octave[i], slopeAttenDb, slopeAtten)
        }
    }
}

template <class TBase>
inline void CHB<TBase>::step()
{
    // do all the processing to get the carrier signal
    const float input = getInput();

    float volume[11];
    calcVolumes(volume);
  //  printf("vol 0=%f, 3 = %f 4=%f 10=%f\n", volume[0], volume[3], volume[4], volume[10]);

    for (int i = 0; i < 11; ++i) {
       // float val = TBase::params[i + PARAM_H0].value;
      //  poly.setGain(i, val);
        poly.setGain(i, volume[i]);
    }
   
    float output = poly.run(input);
    TBase::outputs[MIX_OUTPUT].value = output;
}

