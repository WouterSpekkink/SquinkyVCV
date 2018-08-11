/**
 * This is a modified version of the VCV Fundamental VCO.
 * See LICENSE-dist.txt for full license info.
 * This code has been modified extensively by Squinky Labs. Mainly modifications were:
 *      re-code hot-spots to lower CPU usage.
 *      Fix compiler warnings.
 *      Make it compile in Visual Studio
 */
#pragma once

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable: 4305 4244 4267)
#endif

#if !defined(M_PI)
#define M_PI 3.14159265358979323846264338327950288
#endif

#include "dsp/filter.hpp"
#include "dsp/decimator.hpp"
#include <random>

#include "BiquadFilter.h"
#include "BiquadParams.h"
#include "BiquadState.h"
#include "ButterworthFilterDesigner.h"
#include "ObjectCache.h"

extern float sawTable[2048];
extern float triTable[2048];

// When this is defined, will use Squinky Labs anti-aliasing decimators,
// rather than  rack::Decimator<>
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

    // Use interpolating lookups for these transcendentals
    std::shared_ptr<LookupTableParams<float>> sinLookup;
    std::function<float(float)> expLookup;

    void init()
    {
        sinLookup = ObjectCache<float>::getSinLookup();
        expLookup = ObjectCache<float>::getExp2Ex();

        // Set anti-alias 3-db down point an octave below nyquist: .25   
        float cutoff = .25f / float(OVERSAMPLE);

        sinDecimator.setCutoff(cutoff);
        sawDecimator.setCutoff(cutoff);
        sqrDecimator.setCutoff(cutoff);
        triDecimator.setCutoff(cutoff);
    }

    // Use the standard c++ library for random generation
    std::default_random_engine generator{99};
    std::normal_distribution<double> distribution{0, 1.0};
    float noise()
    {
        return (float) distribution(generator);
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
       // freq = 261.626f * powf(2.0f, pitch / 12.0f);
        const float q = float(log2(261.626));       // move up to pitch range up
        pitch = (pitch / 12.0f) + q;
        freq = expLookup(pitch);
    }

    void setPulseWidth(float pulseWidth)
    {
        const float pwMin = 0.01f;
        pw = clamp(pulseWidth, pwMin, 1.0f - pwMin);
    }

    void process(float deltaTime, float syncValue)
    {
        assert(sinLookup);
        assert(sampleTime > 0);
        if (analog) {
            // Adjust pitch slew
            if (++pitchSlewIndex > 64) {
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

        if (sqEnabled) {
            sqrFilter.setCutoff(40.0f * deltaTime);
        }

        for (int i = 0; i < OVERSAMPLE; i++) {
            if (syncIndex == i) {
                if (soft) {
                    syncDirection = !syncDirection;
                    deltaPhaseOver *= -1.0f;
                } else {
                    // phase = syncCrossing * deltaPhase / OVERSAMPLE;
                    phase = 0.0f;
                }
            }

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
            // don't do that expensive mod
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
#if 0
    float light()
    {
        return sinf(2 * M_PI * phase);
    }
#endif
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

    std::default_random_engine generator{99};
    std::normal_distribution<double> distribution{-1.0, 1.0};
    float noise()
    {
        return (float) distribution(generator);
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

    void init()
    {
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

#if defined(_MSC_VER)
#pragma warning(pop)
#endif