
#pragma once

#include "poly.h"
#include "ObjectCache.h"
#include "SinOscillator.h"


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
        internalUpdate();
    }

    // must be called after setSampleRate
    void init();
    

    enum ParamIds
    {
        PARAM_PITCH,
        PARAM_EXTGAIN,
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
        ENV_INPUT,
        AUDIO_INPUT,
        SLOPE_INPUT,
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
    void step() override;

private:


    float reciprocalSampleRate = 0;

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
    std::shared_ptr<LookupTableParams<float>> pitchExp = {ObjectCache<float>::getExp2()};

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


template <class TBase>
inline float CHB<TBase>::getInput()
{
    // Get the frequency from the inputs.
    float pitch = TBase::params[PARAM_PITCH].value + TBase::inputs[CV_INPUT].value;
    pitch = std::max(-5.0f, pitch);
    pitch = std::min(5.0f, pitch);

    pitch += 5;     // push it up to reasonable range
                    // lookup give abs Hz, so div by sample rate for normalized freq
    float frequency = LookupTable<float>::lookup(*pitchExp, pitch) * reciprocalSampleRate;
    SinOscillator<float, false>::setFrequency(sinParams, frequency);

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
        float val = taper(TBase::params[i + PARAM_H0].value);
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
    TBase::outputs[OUTPUT].value = output;
}

