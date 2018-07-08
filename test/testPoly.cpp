
#include "asserts.h"
#include "poly.h"
#include "Analyzer.h"
#include "SinOscillator.h"

static void test0()
{
    Poly<11> poly;
    float x = poly.run(0);
    assertEQ(x, 0);

    x = poly.run(1);
    assertEQ(x, 0);

    poly.setGain(0, 1);
    x = poly.run(0);
    assertEQ(x, 0);
}

static void test1()
{
    Poly<11> poly;
    poly.setGain(0, 1);
    float x = poly.run(1);
    assertNE(x, 0);
}

// test that we get a good spectrum from synchronous sin
static void testSyncSin()
{
    float desiredFreq = 500.0f;
    int numSamples = 16 * 1024;
    const float sampleRate = 44100.0f;
    float actualFreq = Analyzer::makeEvenPeriod(desiredFreq, sampleRate, numSamples);

    SinOscillatorParams<float> sinParams;
    SinOscillatorState<float> sinState;
    SinOscillator<float, false>::setFrequency(sinParams, actualFreq / sampleRate);

    FFTDataCpx fftData(numSamples);
    Analyzer::getSpectrum(fftData, false, [&sinState, &sinParams]() {
        return SinOscillator<float, false>::run(sinState, sinParams);
        });

    auto data = Analyzer::getMaxAndShouldersFreq(fftData, -3, sampleRate);
    printf("max = %f shoulders=%f,%f\n", std::get<1>(data), std::get<0>(data), std::get<2>(data));

    assertClose(std::get<1>(data), actualFreq, 1);
    assertClose(std::get<0>(data), actualFreq, 5);
    assertClose(std::get<2>(data), actualFreq, 5);
}


void testPoly()
{
    test0();
    test1();

}