
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

const float sampleRate = 44100;
template<typename T>
static void doLowpassTest( std::function<float(float)> filter, T Fc, T expectedSlope)
{
    const int numSamples = 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    auto x = Analyzer::getMaxAndShoulders(response, -3);

    const float cutoff = FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples);

    // Is the peak at zero? i.e. no ripple.
    if (std::get<1>(x) == 0) {
        assertEQ(std::get<0>(x), -1);   // no LF shoulder
    } else {
        // Some higher order filters have a tinny bit of ripple
        float peakMag = std::abs(response.get(std::get<1>(x)));
        float zeroMag = std::abs(response.get(0));
        assertClose(peakMag / zeroMag, 1, .0001);
    }

    assertClose(cutoff, Fc, 3);    // 3db down at Fc


    double slope = Analyzer::getSlope(response, (float) Fc * 2, sampleRate);
    assertClose(slope, expectedSlope, 1);          // to get accurate we need to go to higher freq, etc... this is fine

}


template<typename T>
static void test1()
{
    const float Fc = 100;

    LowpassFilterState<T> state;
    LowpassFilterParams<T> params;
    LowpassFilter<T>::setCutoff(params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        auto y = LowpassFilter<T>::run(x, state, params);
        return float(y);
    };
    doLowpassTest<T>(filter, Fc, -6);
}

template<typename T>
static void test2()
{
    const float Fc = 100;
    BiquadParams<T, 1> params;
    BiquadState<T, 1> state;

    ButterworthFilterDesigner<T>::designTwoPoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    doLowpassTest<T>(filter, Fc, -12);
}

template<typename T>
static void test3()
{
    const float Fc = 100;
    BiquadParams<T, 2> params;
    BiquadState<T, 2> state;

    ButterworthFilterDesigner<T>::designThreePoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    doLowpassTest<T>(filter, Fc, -18);
}

template<typename T>
static void test4()
{
    const float Fc = 100;
    BiquadParams<T, 2> params;
    BiquadState<T, 2> state;

    ButterworthFilterDesigner<T>::designFourPoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    doLowpassTest<T>(filter, Fc, -24);
}
template<typename T>
static void test5()
{
    const float Fc = 100;
    BiquadParams<T, 3> params;
    BiquadState<T, 3> state;

    ButterworthFilterDesigner<T>::designFivePoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    doLowpassTest<T>(filter, Fc, -30);
}

template<typename T>
static void test6()
{
    const float Fc = 100;
    BiquadParams<T, 3> params;
    BiquadState<T, 3> state;

    ButterworthFilterDesigner<T>::designSixPoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    doLowpassTest<T>(filter, Fc, -36);
}

/******************************************************************************************************/
/*
rolloff 0: 21db at 215 
*/

class results
{
public:
    float attenStopDb;
    float freq;
};

template<typename T>
static results doEllipticTest(std::function<float(float)> filter)
{
     const int numSamples = 16 * 1024;
   // const int numSamples = 1* 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);

    //auto x = Analyzer::getMaxAndShoulders(response, -3);

   // const float cutoff = FFT::bin2Freq(std::get<2>(x), sampleRate, numSamples);


    // Some higher order filters have a tinny bit of ripple
    //float peakMag = std::abs(response.get(std::get<1>(x)));
    float zeroMag = std::abs(response.get(0));
  //  printf("mag at zero hz = %f, peak mag = %f, -3db at %f\n ",
   //     zeroMag, peakMag, cutoff);

    float maxHighMag = 0;
    float fMax = 0;
    //Analyzer::getAndPrintFeatures(response, 3, sampleRate);
    for (int i = 0; i < numSamples; ++i) {
        float freq = FFT::bin2Freq(i, sampleRate, numSamples);
        if (freq > 150) {
            float mag = std::abs(response.get(i));
            if (mag > maxHighMag) {
                maxHighMag = mag;
                fMax = freq;
            }
        }

    }
  
    results r;
    r.attenStopDb = (float) AudioMath::db(maxHighMag);
    r.freq = fMax;
    return r;
}

//#define _DO4

template<typename T>
static results testElip1(double rolloff)
{
    const float Fc = 100;
#ifdef _DO4
    BiquadParams<T, 4> params;
    BiquadState<T, 4> state;
#else
    BiquadParams<T, 3> params;
    BiquadState<T, 3> state;
#endif

    T rippleDb = 3;
  //  T rolloff = 0;                // at 20 it dies
#ifdef _DO4
    ButterworthFilterDesigner<T>::designEightPoleElliptic(params, Fc / sampleRate, rippleDb, rolloff);
#else
    ButterworthFilterDesigner<T>::designSixPoleElliptic(params, Fc / sampleRate, rippleDb, rolloff);
#endif

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<T>::run(x, state, params);
        return x;
    };
    return doEllipticTest<T>(filter);
   
}

//float attenStopDb;
//float freq;



//best 8 pole: r = -0.47, max = -57.47 at 156.12
static void hunt()
{
    results best;
    best.attenStopDb = 100;
    double bestRolloff = 1000;
    for (double f = -2; f < -1; f += .01) {
        results x = testElip1<double>(f);
        printf("r = %.2f, max = %.2f at %.2f\n", f, x.attenStopDb, x.freq);
        if (x.attenStopDb < best.attenStopDb) {
            best = x;
            bestRolloff = f;
        }
    }
    testElip1<double>(-1.24);
    printf("best: r = %.2f, max = %.2f at %.2f\n", bestRolloff, best.attenStopDb, best.freq);
    printf("done\n");
}

template<typename T>
void _testLowpassFilter()
{
    hunt();
  //  testElip1<T>();
    test0<T>();
    test1<T>();
    test2<T>();
    test3<T>();
    test4<T>();
    test5<T>();
    test6<T>();
   // testElip1<T>();
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