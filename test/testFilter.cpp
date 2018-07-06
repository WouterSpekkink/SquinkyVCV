
#include <assert.h>

#include "FFT.h"
#include "GraphicEq.h"
#include "StateVariableFilter.h"
#include "SinOscillator.h"
#include "Analyzer.h"
#include "asserts.h"

static void testPeak(std::function<float(float)> filter, float sampleRate, float expectedMax, float percentTolerance)
{

    const int numSamples = 64 * 1024;
    FFTDataCpx x(numSamples);
    Analyzer::getFreqResponse(x, filter);

    int maxBin = Analyzer::getMax(x);
    float maxFreq = (FFT::bin2Freq(maxBin, 44100, numSamples) +
        FFT::bin2Freq(maxBin + 1, 44100, numSamples)) / 2;

    const float delta = expectedMax * percentTolerance / 100.f ;      // One percent accuracy

    assertClose(maxFreq, expectedMax, delta);

}

template <typename T>
static void testBandpass(float Fc, float tolerancePercent)
{
    StateVariableFilterState<T> state;
    StateVariableFilterParams<T> params;

    const float sampleRate = 44100;

    params.setMode(StateVariableFilterParams<T>::Mode::BandPass);
    params.setFreq(Fc / sampleRate);
    params.setQ(3);

    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = StateVariableFilter<T>::run(x, state, params);
        return float(y);
    };
    testPeak(filter, sampleRate, Fc, tolerancePercent);
}

template <typename T>
static void test1()
{
    testBandpass<T>(10, 10);
    testBandpass<T>(100, 2);
    testBandpass<T>(1000, 3);
    testBandpass<T>(3000, 12);
    testBandpass<T>(6000, 35);      // This filter just gets inaccurate at high freq - don't know why
}


void testFilter()
{
    test1<float>();
}