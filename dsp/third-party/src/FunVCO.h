#pragma once


#pragma warning (push)
#pragma warning (disable: 4305)
#pragma warning (disable: 4244)

#define M_PI 3.14159265358979323846264338327950288

#include "dsp/filter.hpp"
#include "dsp/decimator.hpp"
#include <random>


extern float sawTable[2048];
extern float triTable[2048];

/**
 * CPU = 554, all outputs hooked up.
 * 392 no output hooked up
 * 91 no outputs hooked up, short circuit sin
 * 89 no outputs hooked up, short circuit sin, sq
 * 48 all the above, and smarter phase advance 
 * 47 all the above, saw turned off
 * 41 all the above, tri turned off
 * 38 no exp
 * 37 remove outer pitch proc
 * 28 remove all outer proc
 * 14 fake oversample down to 4
 * 41 (back to no crazy stuff
 * 16 gut the do nothing part of high sample rate inner loop
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
                    sinBuffer[i] = sinf(2.f*M_PI * phase);
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