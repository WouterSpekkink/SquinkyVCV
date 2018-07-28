#pragma once


#pragma warning (push)
#pragma warning (disable: 4305)
#pragma warning (disable: 4244)

#define M_PI 3.14159265358979323846264338327950288

#include "dsp/filter.hpp"
#include "dsp/decimator.hpp"
#include <random>

#include "ObjectCache.h"

extern float sawTable[2048];
extern float triTable[2048];

#define _USEIIR

template <int OVERSAMPLE>
class IIRDecimator
{
public:
    float process(const float * input)
    {
        float x = 0;
        for (int i = 0; i < OVERSAMPLE; ++i) {
            x = BiquadFilter<float>::run(input[i], state, params);
        }
        return x;
    }

    /**
     * cutoff is normalized freq (.5 = nyquist)
     */
    void setCutoff(float cutoff)
    {
        assert(cutoff > 0 && cutoff < .5f);
        ButterworthFilterDesigner<float>::designSixPoleLowpass(params, cutoff);
    }
private:
    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;

};

/**
 * orig, all outputs off - 394
 * orig, all outputs on - 809
 * orig, saw only = 492
 *
 * mine 7/27
 * all outputs off - 47
 * all outputs on - 475
 * saw only - 149
 * sin only - 154
 * sq only - 153;
 *
 * why does all off take so long?
 * remove inner loop, all off - 6!!
 * no inner loop, saw only 205
 * inner loop, no decimator saw - 158

 * no decimator, no pitch calc.
 * saw - 142
 * all off - 36
 * all on - 624
 *
 * dedicated saw path (used for all
 * saw - 236, 233 optimized
 *
 * IIR Decimators
 * all outputs off - 42
 * all outputs on - 214
 * saw only - 80
 * sin only - 88
 * sq only - 92
 */
template <int OVERSAMPLE, int QUALITY>
struct VoltageControlledOscillator
{
    float sampleTime = 0;
    bool analog = false;
    bool soft = false;
    float lastSyncValue = 0.0f;
    float phase = 0.0f;
    float freq;
    float pw = 0.5f;
    float pitch;
    bool syncEnabled = false;
    bool syncDirection = false;

    /**
     * flags to help decide not to do redundant work
     */
    bool sinEnabled = false;
    bool sqEnabled = false;
    bool sawEnabled = false;
    bool triEnabled = false;

#ifdef _USEIIR
    IIRDecimator<OVERSAMPLE> sinDecimator;
    IIRDecimator<OVERSAMPLE> triDecimator;
    IIRDecimator<OVERSAMPLE> sawDecimator;
    IIRDecimator<OVERSAMPLE> sqrDecimator;
#else
    rack::Decimator<OVERSAMPLE, QUALITY> sinDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> triDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> sawDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> sqrDecimator;
#endif
    RCFilter sqrFilter;



    // For analog detuning effect
    float pitchSlew = 0.0f;
    int pitchSlewIndex = 0;

    float sinBuffer[OVERSAMPLE] = {};
    float triBuffer[OVERSAMPLE] = {};
    float sawBuffer[OVERSAMPLE] = {};
    float sqrBuffer[OVERSAMPLE] = {};

    std::shared_ptr<LookupTableParams<float>> sinLookup;

    void init()
    {
        sinLookup = ObjectCache<float>::getSinLookup();

        // first guess - an octave below nyquist
        float cutoff = .25f / float(OVERSAMPLE);
        sinDecimator.setCutoff(1.f / 32.f);
        sawDecimator.setCutoff(1.f / 32.f);
        sqrDecimator.setCutoff(1.f / 32.f);
        triDecimator.setCutoff(1.f / 32.f);
       
    }

    // TODO: what is the range of the one in VCV?
    std::default_random_engine generator{99};
    std::normal_distribution<double> distribution{-1.0, 1.0};
    float noise()
    {
        return  (float) distribution(generator);
    }

    void setPitch(float pitchKnob, float pitchCv)
    {
        // Compute frequency
        pitch = pitchKnob;
        if (analog) {
            // Apply pitch slew
            const float pitchSlewAmount = 3.0f;
            pitch += pitchSlew * pitchSlewAmount;
        } else {
            // Quantize coarse knob if digital mode
            pitch = roundf(pitch);
        }
        pitch += pitchCv;
        // Note C4
         freq = 261.626f * powf(2.0f, pitch / 12.0f);
    }
    void setPulseWidth(float pulseWidth)
    {
        const float pwMin = 0.01f;
        pw = clamp(pulseWidth, pwMin, 1.0f - pwMin);
    }

    void processSaw(float deltaTime, float syncValue)
    {
        assert(sampleTime > 0);
        if (analog) {
            // Adjust pitch slew
            if (++pitchSlewIndex > 32) {
                const float pitchSlewTau = 100.0f; // Time constant for leaky integrator in seconds
                pitchSlew += (noise() - pitchSlew / pitchSlewTau) *sampleTime;
                pitchSlewIndex = 0;
            }
        }

        // Advance phase
        float deltaPhaseOver = clamp(freq * deltaTime, 1e-6, 0.5f) * (1.0f / OVERSAMPLE);

        // Detect sync
        int syncIndex = -1; // Index in the oversample loop where sync occurs [0, OVERSAMPLE)
        float syncCrossing = 0.0f; // Offset that sync occurs [0.0f, 1.0f)
        if (syncEnabled) {
            syncValue -= 0.01f;
            if (syncValue > 0.0f && lastSyncValue <= 0.0f) {
                float deltaSync = syncValue - lastSyncValue;
                syncCrossing = 1.0f - syncValue / deltaSync;
                syncCrossing *= OVERSAMPLE;
                syncIndex = (int) syncCrossing;
                syncCrossing -= syncIndex;
            }
            lastSyncValue = syncValue;
        }

        if (syncDirection)
            deltaPhaseOver *= -1.0f;

     

        for (int i = 0; i < OVERSAMPLE; i++) {
#if 0
            // if (syncEnabled) {
            if (syncIndex == i) {
                if (soft) {
                    syncDirection = !syncDirection;
                    deltaPhaseOver *= -1.0f;
                } else {
                    // phase = syncCrossing * deltaPhase / OVERSAMPLE;
                    phase = 0.0f;
                }
            }
            // }


#endif


        //    if (analog) {
        //        sawBuffer[i] = 1.66f * interpolateLinear(sawTable, phase * 2047.f);
        //    } else {
                if (phase < 0.5f)
                    sawBuffer[i] = 2.f * phase;
                else
                    sawBuffer[i] = -2.f + 2.f * phase;
        //    }


            // don't divide by oversample every time.
            // don't do that expensive mod (TODO: is this correct

            // Advance phase ( I think this could be a normal phase advance
            //  phase += deltaPhase / OVERSAMPLE;
            // phase = eucmod(phase, 1.0f);
            phase += deltaPhaseOver;
            while (phase > 1.0f) {
                phase -= 1.0f;
            }
            while (phase < 0) {
                phase += 1.0f;
            }
        }
    }

    void process(float deltaTime, float syncValue)
    {
       // processSaw(deltaTime, syncValue);
        assert(sinLookup);
#if 1
        assert(sampleTime > 0);
        if (analog) {
            // Adjust pitch slew
            if (++pitchSlewIndex > 32) {
                const float pitchSlewTau = 100.0f; // Time constant for leaky integrator in seconds
                pitchSlew += (noise() - pitchSlew / pitchSlewTau) *sampleTime;
                pitchSlewIndex = 0;
            }
        }

        // Advance phase
        float deltaPhaseOver = clamp(freq * deltaTime, 1e-6, 0.5f) * (1.0f / OVERSAMPLE);

        // Detect sync
        int syncIndex = -1; // Index in the oversample loop where sync occurs [0, OVERSAMPLE)
        float syncCrossing = 0.0f; // Offset that sync occurs [0.0f, 1.0f)
        if (syncEnabled) {
            syncValue -= 0.01f;
            if (syncValue > 0.0f && lastSyncValue <= 0.0f) {
                float deltaSync = syncValue - lastSyncValue;
                syncCrossing = 1.0f - syncValue / deltaSync;
                syncCrossing *= OVERSAMPLE;
                syncIndex = (int) syncCrossing;
                syncCrossing -= syncIndex;
            }
            lastSyncValue = syncValue;
        }

        if (syncDirection)
            deltaPhaseOver *= -1.0f;

        // this does a divide
        if (sqEnabled) {
            sqrFilter.setCutoff(40.0f * deltaTime);
        }


        for (int i = 0; i < OVERSAMPLE; i++) {

           // if (syncEnabled) {
                if (syncIndex == i) {
                    if (soft) {
                        syncDirection = !syncDirection;
                        deltaPhaseOver *= -1.0f;
                    } else {
                        // phase = syncCrossing * deltaPhase / OVERSAMPLE;
                        phase = 0.0f;
                    }
                }
           // }

            if (sinEnabled) {
                if (analog) {
                    // Quadratic approximation of sine, slightly richer harmonics
                    if (phase < 0.5f)
                        sinBuffer[i] = 1.f - 16.f * powf(phase - 0.25f, 2);
                    else
                        sinBuffer[i] = -1.f + 16.f * powf(phase - 0.75f, 2);
                    sinBuffer[i] *= 1.08f;
                } else {
                   // sinBuffer[i] = sinf(2.f*M_PI * phase);
                    sinBuffer[i] = LookupTable<float>::lookup(*sinLookup, phase, true);
                }
            }

  
            if (triEnabled) {
                if (analog) {
                    triBuffer[i] = 1.25f * interpolateLinear(triTable, phase * 2047.f);
                } else {
                    if (phase < 0.25f)
                        triBuffer[i] = 4.f * phase;
                    else if (phase < 0.75f)
                        triBuffer[i] = 2.f - 4.f * phase;
                    else
                        triBuffer[i] = -4.f + 4.f * phase;
                }
            }

            if (sawEnabled) {
                if (analog) {
                    sawBuffer[i] = 1.66f * interpolateLinear(sawTable, phase * 2047.f);
                } else {
                    if (phase < 0.5f)
                        sawBuffer[i] = 2.f * phase;
                    else
                        sawBuffer[i] = -2.f + 2.f * phase;
                }
            }
     

            if (sqEnabled) {
                sqrBuffer[i] = (phase < pw) ? 1.f : -1.f;
                if (analog) {
                    // Simply filter here
                    sqrFilter.process(sqrBuffer[i]);
                    sqrBuffer[i] = 0.71f * sqrFilter.highpass();
                }
            }

            // don't divide by oversample every time.
            // don't do that expensive mod (TODO: is this correct

            // Advance phase ( I think this could be a normal phase advance
            //  phase += deltaPhase / OVERSAMPLE;
           // phase = eucmod(phase, 1.0f);
            phase += deltaPhaseOver;
            while (phase > 1.0f) {
                phase -= 1.0f;
            }
            while (phase < 0) {
                phase += 1.0f;
            }
        }
#endif
    }

    float sin()
    {
        return sinDecimator.process(sinBuffer);
    }
    float tri()
    {
        return triDecimator.process(triBuffer);
    }
    float saw()
    {
       return sawDecimator.process(sawBuffer);
    }
    float sqr()
    {
        return sqrDecimator.process(sqrBuffer);
    }
    float light()
    {
        return sinf(2 * M_PI * phase);
    }
};


template <int OVERSAMPLE, int QUALITY>
struct VoltageControlledOscillatorOrig
{
    float sampleTime = 0;
    bool analog = false;
    bool soft = false;
    float lastSyncValue = 0.0f;
    float phase = 0.0f;
    float freq;
    float pw = 0.5f;
    float pitch;
    bool syncEnabled = false;
    bool syncDirection = false;

    rack::Decimator<OVERSAMPLE, QUALITY> sinDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> triDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> sawDecimator;
    rack::Decimator<OVERSAMPLE, QUALITY> sqrDecimator;
    RCFilter sqrFilter;

    // For analog detuning effect
    float pitchSlew = 0.0f;
    int pitchSlewIndex = 0;

    float sinBuffer[OVERSAMPLE] = {};
    float triBuffer[OVERSAMPLE] = {};
    float sawBuffer[OVERSAMPLE] = {};
    float sqrBuffer[OVERSAMPLE] = {};

    // TODO: what is the range of the one in VCV?
    std::default_random_engine generator{99};
    std::normal_distribution<double> distribution{-1.0, 1.0};
    float noise()
    {
        return  (float) distribution(generator);
    }

    void setPitch(float pitchKnob, float pitchCv)
    {
        // Compute frequency
        pitch = pitchKnob;
        if (analog) {
            // Apply pitch slew
            const float pitchSlewAmount = 3.0f;
            pitch += pitchSlew * pitchSlewAmount;
        } else {
            // Quantize coarse knob if digital mode
            pitch = roundf(pitch);
        }
        pitch += pitchCv;
        // Note C4
        freq = 261.626f * powf(2.0f, pitch / 12.0f);
    }
    void setPulseWidth(float pulseWidth)
    {
        const float pwMin = 0.01f;
        pw = clamp(pulseWidth, pwMin, 1.0f - pwMin);
    }

    void process(float deltaTime, float syncValue)
    {
        assert(sampleTime > 0);
        if (analog) {
            // Adjust pitch slew
            if (++pitchSlewIndex > 32) {
                const float pitchSlewTau = 100.0f; // Time constant for leaky integrator in seconds
               // pitchSlew += (randomNormal() - pitchSlew / pitchSlewTau) * sampleTime;
                pitchSlew += (noise() - pitchSlew / pitchSlewTau) *sampleTime;
                pitchSlewIndex = 0;
            }
        }

        // Advance phase
        float deltaPhase = clamp(freq * deltaTime, 1e-6, 0.5f);

        // Detect sync
        int syncIndex = -1; // Index in the oversample loop where sync occurs [0, OVERSAMPLE)
        float syncCrossing = 0.0f; // Offset that sync occurs [0.0f, 1.0f)
        if (syncEnabled) {
            syncValue -= 0.01f;
            if (syncValue > 0.0f && lastSyncValue <= 0.0f) {
                float deltaSync = syncValue - lastSyncValue;
                syncCrossing = 1.0f - syncValue / deltaSync;
                syncCrossing *= OVERSAMPLE;
                syncIndex = (int) syncCrossing;
                syncCrossing -= syncIndex;
            }
            lastSyncValue = syncValue;
        }

        if (syncDirection)
            deltaPhase *= -1.0f;

        sqrFilter.setCutoff(40.0f * deltaTime);

        for (int i = 0; i < OVERSAMPLE; i++) {
            if (syncIndex == i) {
                if (soft) {
                    syncDirection = !syncDirection;
                    deltaPhase *= -1.0f;
                } else {
                    // phase = syncCrossing * deltaPhase / OVERSAMPLE;
                    phase = 0.0f;
                }
            }

            if (analog) {
                // Quadratic approximation of sine, slightly richer harmonics
                if (phase < 0.5f)
                    sinBuffer[i] = 1.f - 16.f * powf(phase - 0.25f, 2);
                else
                    sinBuffer[i] = -1.f + 16.f * powf(phase - 0.75f, 2);
                sinBuffer[i] *= 1.08f;
            } else {
                sinBuffer[i] = sinf(2.f*M_PI * phase);
            }
            if (analog) {
                triBuffer[i] = 1.25f * interpolateLinear(triTable, phase * 2047.f);
            } else {
                if (phase < 0.25f)
                    triBuffer[i] = 4.f * phase;
                else if (phase < 0.75f)
                    triBuffer[i] = 2.f - 4.f * phase;
                else
                    triBuffer[i] = -4.f + 4.f * phase;
            }
            if (analog) {
                sawBuffer[i] = 1.66f * interpolateLinear(sawTable, phase * 2047.f);
            } else {
                if (phase < 0.5f)
                    sawBuffer[i] = 2.f * phase;
                else
                    sawBuffer[i] = -2.f + 2.f * phase;
            }
            sqrBuffer[i] = (phase < pw) ? 1.f : -1.f;
            if (analog) {
                // Simply filter here
                sqrFilter.process(sqrBuffer[i]);
                sqrBuffer[i] = 0.71f * sqrFilter.highpass();
            }

            // Advance phase
            phase += deltaPhase / OVERSAMPLE;
            phase = eucmod(phase, 1.0f);
        }
    }

    float sin()
    {
        return sinDecimator.process(sinBuffer);
    }
    float tri()
    {
        return triDecimator.process(triBuffer);
    }
    float saw()
    {
        return sawDecimator.process(sawBuffer);
    }
    float sqr()
    {
        return sqrDecimator.process(sqrBuffer);
    }
    float light()
    {
        return sinf(2 * M_PI * phase);
    }
};


#pragma warning(pop)