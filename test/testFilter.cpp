
#include <assert.h>

#include "FFT.h"
#include "StateVariableFilter.h"
#include "SinOscillator.h"
#include "Analyzer.h"
#include "asserts.h"

static void testPeak(std::function<float(float)> filter, float sampleRate, float expectedMax)
{
    const int numSamples = 16 * 1024;
    FFTDataCpx x(numSamples);
    Analyzer::getFreqResponse(x, filter);

    // fix this
    //Analyzer::getAndPrintFeatures(x, 3.0f, 44100);

    int maxBin = Analyzer::getMax(x);
    printf("max bin = %d, freq=%f expected =%f\n", maxBin, FFT::bin2Freq(maxBin, 44100, numSamples), expectedMax);
    
}


static void test0()
{
    SinOscillatorParams<float> params;
    SinOscillatorState<float> state;
    const float Fc = 100;
    const float sampleRate = 44100;

    SinOscillator<float, false>::setFrequency(params, Fc / sampleRate);
    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = SinOscillator<float, false>::run(state, params);
        // printf("filter(%f) ret (%f)\n", x, y);
        return y;
    };
    testPeak(filter, sampleRate, 100);
}


static void test1()
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
    testPeak(filter, sampleRate, 100);

   // Analyzer::getFreqResponse(x, filter);
#if 0
    // fix this
    //Analyzer::getAndPrintFeatures(x, 3.0f, 44100);

    int maxBin = Analyzer::getMax(x);
    printf("max bin = %d, freq=%f target = %f\n", maxBin, FFT::bin2Freq(maxBin, 44100, size), Fc);
    // TODO: tests something

    for (int i = 0; i < size / 2; ++i) {
        assertClose(std::abs(x.get(i)), 1, .0001);
    }
#endif
 
}
void testFilter()
{
    test0();
    test1();
}