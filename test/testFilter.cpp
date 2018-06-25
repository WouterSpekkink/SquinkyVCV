
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


static void test3()
{
    const float bw = .7f;
    const float Fc = 200;
    printf("one stage, fc=%f bw=%f\n", Fc, bw);
    StateVariableFilterState<float> state;
    StateVariableFilterParams<float> params;

   
    const float sampleRate = 44100;

    params.setMode(params.Mode::BandPass);
    params.setFreq(Fc / sampleRate);
    params.setNormalizedBandwidth(bw);

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

    auto x = Analyzer::getMaxAndShoulders(response, -3);
    printf("lf 3db at %f, high at %f, center at %f\n",
        FFT::bin2Freq(std::get<0>(x), sampleRate, numSamples),
        FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples),
        FFT::bin2Freq(std::get<1>(x), sampleRate, numSamples)
    );

}

static void _test4(int stages, float bw)
{
    printf("\ntest geq %d sages bw=%f\n", stages, bw);
    GraphicEq geq(stages, bw);
    std::function<float(float)> filter = [&geq](float x) {
        auto y = geq.run(x);
        // printf("filter(%f) ret (%f)\n", x, y);
        return y;
    };

    const int numSamples = 64 * 1024;
    const int sampleRate = 44100;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    auto x = Analyzer::getMaxAndShoulders(response, -2);
    printf("geq: lf 3db at %f, high at %f, center at %f\n",
        FFT::bin2Freq(std::get<0>(x), sampleRate, numSamples),
        FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples),
        FFT::bin2Freq(std::get<1>(x), sampleRate, numSamples)
    );
}

static void test4()
{
    printf("\n");
   // _test4(2, .8f);
    _test4(2, .7f);
  //  _test4(3, 1.1f);
  //  _test4(3, 1.0f);
 //   _test4(3, .9f);
 //   _test4(3, .8f);
    _test4(3, .7f);
   // _test4(3, .6f);
   // _test4(4, .7f);
    _test4(4, .7f);

}

void testFilter()
{
    test0();
    test1();
    test2();
    test3();
    test4();
}