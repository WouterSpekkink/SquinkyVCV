
#include <assert.h>

#include "FFT.h"
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
   // printf("max bin = %d, freq=%f expected =%f\n", maxBin, maxFreq, expectedMax);

    const float delta = expectedMax * percentTolerance / 100.f ;      // One percent accuracy
    assertClose(maxFreq, expectedMax, delta);
    
}

/**
 * Really a test of the analyzer.
 * make as sin wave and see if the peak is where it should be.
 */
static void test0()
{
    SinOscillatorParams<float> params;
    SinOscillatorState<float> state;
    const float Fc = 100;
    const float sampleRate = 44100;

    SinOscillator<float, false>::setFrequency(params, Fc / sampleRate);
    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = SinOscillator<float, false>::run(state, params);
        return y;
    };
    testPeak(filter, sampleRate, 100, 1);
}


static void testBandpass(float Fc, float tolerancePercent)
{
    StateVariableFilterState<float> state;
    StateVariableFilterParams<float> params;

   // const float Fc = 100;
    const float sampleRate = 44100;

    params.setMode(params.Mode::BandPass);
    params.setFreq(Fc / sampleRate);
    params.setNormalizedBandwidth(.2f);

    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = StateVariableFilter<float>::run(x, state, params);
       // printf("filter(%f) ret (%f)\n", x, y);
        return y;
    };
    testPeak(filter, sampleRate, Fc, tolerancePercent);
}

static void test1()
{
    testBandpass(10, 10);
    testBandpass(100, 1);
    testBandpass(1000, 1);
}


static void test2()
{
    StateVariableFilterState<float> state;
    StateVariableFilterParams<float> params;

    const float Fc = 100;
    const float sampleRate = 44100;

    params.setMode(params.Mode::BandPass);
    params.setFreq(Fc / sampleRate);
    params.setNormalizedBandwidth(.2f);

    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = StateVariableFilter<float>::run(x, state, params);
        // printf("filter(%f) ret (%f)\n", x, y);
        return y;
    };
    //testPeak(filter, sampleRate, Fc, tolerancePercent);

    printf("test2: bp resp:\n");
    const int numSamples = 64 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    int maxBin = Analyzer::getMax(response);
    printf("max bin %d f = %f gain = %f\n", maxBin,
        FFT::bin2Freq(maxBin, sampleRate, numSamples),
        std::abs(response.get(maxBin)));

    Analyzer::getAndPrintFeatures(response, 3, sampleRate);

}


void testFilter()
{
    test0();
    test1();
    test2();
}