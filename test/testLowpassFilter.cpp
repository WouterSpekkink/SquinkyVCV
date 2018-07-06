
#include "LowpassFilter.h"
#include "Decimator.h"
#include "Analyzer.h"
#include "asserts.h"
#include "LFN.h"
#include "TestComposite.h"

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

    const int numSamples = 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    auto x = Analyzer::getMaxAndShoulders(response, -3);

    const float cutoff = FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples);
    assert(std::get<1>(x) == 0);                 // low pass, peak at 0hz
    assertEQ(std::get<0>(x), -1);   // no LF shoulder
    assertClose(cutoff, 100, 1);    // 3db down at Fc

    float slope = Analyzer::getSlope(response, 200, sampleRate);
    assertClose(slope, -6, 1);          // to get accurate we need to go to higher freq, etc... this is fine
}

template<typename T>
void _testLowpassFilter()
{
    test0<T>();
    test1<T>();
}

/************ also test decimator here
 */

static void decimate0()
{
    Decimator d;
    d.setDecimationRate(2);
    d.acceptData(5);
    bool b = true;
    auto x = d.clock(b);
    assert(!b);
    assertEQ(x, 5);
}


static void decimate1()
{
    Decimator d;
    d.setDecimationRate(2);
    d.acceptData(5);
    bool b = true;
    auto x = d.clock(b);
    assert(!b);
    assertEQ(x, 5);

    x = d.clock(b);
    assert(b);
    assertEQ(x, 5);
}

void testLowpassFilter()
{
    _testLowpassFilter<float>();
    _testLowpassFilter<double>();
    decimate0();
    decimate1();
}