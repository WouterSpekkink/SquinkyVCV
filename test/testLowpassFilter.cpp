
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

// temp - remove later
using tLFN = LFN<TestComposite>;
static void testLFN()
{
    tLFN lfn;
    lfn.setSampleRate(44100.0f);
    lfn.params[tLFN::FREQ_RANGE_PARAM].value = 5;
    lfn.step();
    lfn.init();
    for (int i = tLFN::EQ0_PARAM; i <= tLFN::EQ4_PARAM; ++i) {
        lfn.params[i].value = 1;
    }


    std::function<float(float)> filter = [&lfn](float x) {
        lfn.step();
        float ret = lfn.outputs[tLFN::OUTPUT].value;
        //printf("noise: %f\n", ret);
        return ret;
    };

    const int numSamples = 64 * 64 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);
    for (int i = 0; i < numSamples; ++i) {
        //printf("mag[%d] = %f\n", i, std::abs(response.get(i)));
    }

   // auto x = Analyzer::getMaxAndShoulders(response, -3);
    Analyzer::getAndPrintFeatures(response, 3, 44100);

    printf("did it (LFN)\n");
}




#include <random>

static std::default_random_engine generator(12345);
std::normal_distribution<double> distribution(-1.0, 1.0);

#if 0
static float noise2()
{
    static bool b = false;
    static float last = 0;

    b = !b;
    if (b) {
        last = (float) distribution(generator);
    }
    return last;

}
#endif

template <typename TButter>
static void testButter(float fc)
{
    printf("butter w/noise\n");
    int seed = 57;
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(-1.0, 1.0);

    BiquadParams<TButter, 3> params;
    BiquadState<TButter, 3> state;

    const float ff = fc / 44100.0f;
    printf("butter at fc %f is %f\n", fc, ff);
    ButterworthFilterDesigner<TButter>::designSixPoleLowpass(params, fc / 44100.0f);

    std::function<float(float)> filter = [&params, &state, &distribution, &generator](float x) {
        return (float) BiquadFilter<TButter>::run(x, state, params);
       // return  (float) BiquadFilter<TButter>::run(noise2(), state, params);
    };

    const int numSamples = 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);
    for (int i = 0; i < numSamples; ++i) {
        //printf("mag[%d] = %f\n", i, std::abs(response.get(i)));
    }

    // auto x = Analyzer::getMaxAndShoulders(response, -3);
    Analyzer::getAndPrintFeatures(response, 3, 44100);

    printf("did it (butterworth)\n");
}



static void testButterWNoise(float fc)
{
    printf("butter w/noise\n");
    int seed = 57;
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(-1.0, 1.0);

    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;

    const float ff = fc / 44100.0f;
    printf("butter at fc %f is %f\n", fc, ff);
    ButterworthFilterDesigner<float>::designSixPoleLowpass(params, fc / 44100.0f);

    std::function<float()> filter = [&params, &state, &distribution, &generator]() {
        return  (float) BiquadFilter<float>::run(
            (float) distribution(generator),
            state,
            params);
    };

    const int numSamples = 64 * 64 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getSpectrum(response, filter);

    Analyzer::getAndPrintFeatures(response, 6, 44100);

    printf("did it (butterworth w/noise)\n");
}


static void testButter()
{
    testButter<float>(64.f);
}



static void testNoise()
{

    int seed = 57;
    std::default_random_engine generator(seed);
    std::normal_distribution<double> distribution(-1.0, 1.0);

    std::function<float()> filter = [&generator, &distribution]() {
        //return (float) BiquadFilter<TButter>::run(x, state, params);
       // return 1000.0f * (float) BiquadFilter<TButter>::run(noise(), state, params);
        return (float) distribution(generator);
    };

    const int numSamples = 256;
    FFTDataCpx response(numSamples);
    Analyzer::getSpectrum(response, filter);
    for (int i = 0; i < numSamples; ++i) {
        //printf("mag[%d] = %f\n", i, std::abs(response.get(i)));
    }

    // auto x = Analyzer::getMaxAndShoulders(response, -3);
    Analyzer::getAndPrintFeatures(response, 3, 44100);

    printf("did it (noise)\n");
}

#include "SinOscillator.h"
static void testSin()
{
    SinOscillatorParams<float> params;
    SinOscillatorState<float> state;

    SinOscillator<float, true>::setFrequency(params, 32.f / 44100.f);
   // T x = SinOscillator<T, true>::run(s, p);

    std::function<float()> filter = [&params, &state]() {
        //return (float) BiquadFilter<TButter>::run(x, state, params);
      //  // return 1000.0f * (float) BiquadFilter<TButter>::run(noise(), state, params);
       // return (float) distribution(generator);
        return  SinOscillator<float, true>::run(state, params);
    };

    const int numSamples = 16 * 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getSpectrum(response, filter);
    for (int i = 0; i < numSamples; ++i) {
        //printf("mag[%d] = %f\n", i, std::abs(response.get(i)));
    }

    // auto x = Analyzer::getMaxAndShoulders(response, -3);
    Analyzer::getAndPrintFeatures(response, 3, 44100);

    printf("did it (sin)\n");
}

void testLowpassFilter()
{
    _testLowpassFilter<float>();
    _testLowpassFilter<double>();
    decimate0();
    decimate1();
   // testLFN();
    testButter();
  //  testButterWNoise(64);
   // testNoise();
   // testSin();
}