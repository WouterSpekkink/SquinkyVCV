
#include "LowpassFilter.h"
#include "Analyzer.h"
#include "asserts.h"

template<typename T>
static void test0()
{
    LowpassFilterState<T> state;
    LowpassFilterParams<T> params;
    LowpassFilter<T>::setCutoff(params, T(.1));
    LowpassFilter<T>::run(0, state, params);
}

template<typename T>
static void test1()
{
    const float Fc = 100;
    const float sampleRate = 44100;

    LowpassFilterState<T> state;
    LowpassFilterParams<T> params;
    LowpassFilter<T>::setCutoff(params, Fc / sampleRate);
    
    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = LowpassFilter<T>::run(x, state, params);
        return float(y);
    };

    const int numSamples = 16*1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    int m = Analyzer::getMax(response);
    assert(m >= 0);
    auto x = Analyzer::getMaxAndShoulders(response, -3);
  

    const float cutoff = FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples);
    assert(m == 0);                 // lowpass, peak at 0hz
    assertEQ(std::get<0>(x), -1);   // no LF shoulder
    assertClose(cutoff, 100, 1);    // 3db down at Fc

    float slope = Analyzer::getSlope(response, 200, sampleRate);
    assertClose(slope, -6, .1);
   
}

template<typename T>
void _testLowpassFilter()
{
    test0<T>();
    test1<T>();
}

void testLowpassFilter()
{
    _testLowpassFilter<float>();
    _testLowpassFilter<double>();
}