
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

    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, [&sinState, &sinParams]() {
        return SinOscillator<float, false>::run(sinState, sinParams);
        });

    Analyzer::assertSingleFreq(spectrum, actualFreq, sampleRate);

}

static void testPureTerm(int term)
{
    float desiredFreq = 500.0f;
    int numSamples = 16 * 1024;
    const float sampleRate = 44100.0f;
    float actualFreq = Analyzer::makeEvenPeriod(desiredFreq, sampleRate, numSamples);

    SinOscillatorParams<float> sinParams;
    SinOscillatorState<float> sinState;
    SinOscillator<float, false>::setFrequency(sinParams, actualFreq / sampleRate);

    Poly<11> poly;
    poly.setGain(term, 1);

    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, [&sinState, &sinParams, &poly]() {
        float sin = SinOscillator<float, false>::run(sinState, sinParams);
        const float x = poly.run(sin);
        return x;
        });

    const int order = term + 1;
    Analyzer::assertSingleFreq(spectrum, actualFreq * order, sampleRate);
}

static void testTerms()
{
    for (int i = 0; i < 10; ++i) {
        testPureTerm(i);
   }    
}

void testPoly()
{
    test0();
    test1();
    testTerms();

}