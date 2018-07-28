
#include <assert.h>

#include "Analyzer.h"
#include "asserts.h"
#include "EvenVCO.h"
#include "SawOscillator.h"
#include "TestComposite.h"


using EVCO = EvenVCO <TestComposite>;

#if 0
// Compute frequency, pitch is 1V/oct
float pitch = 1.0 + roundf(TBase::params[OCTAVE_PARAM].value) + TBase::params[TUNE_PARAM].value / 12.0;
pitch += TBase::inputs[PITCH1_INPUT].value + TBase::inputs[PITCH2_INPUT].value;
pitch += TBase::inputs[FM_INPUT].value / 4.0;


// float freq = 261.626 * powf(2.0, pitch);
#endif

float desiredPitch(const EVCO& vco)
{
    float pitch = 1.0f + roundf(vco.params[(int) EVCO::OCTAVE_PARAM].value) + vco.params[(int) EVCO::TUNE_PARAM].value / 12.0f;
    pitch += vco.inputs[(int) EVCO::PITCH1_INPUT].value + vco.inputs[(int) EVCO::PITCH2_INPUT].value;
    pitch += vco.inputs[(int) EVCO::FM_INPUT].value / 4.0f;

    float freq = 261.626f * powf(2.0f, pitch);
    printf("theirs: pitch = %f exp = %f\n", pitch, freq);
    return freq;
}


static void testx(float octave, float tune = 0, float pitch1 = 0, float pitch2 = 0, float fm = 0)
{
    EVCO vco;

    vco.params[(int) EVCO::OCTAVE_PARAM].value = octave;
    vco.params[(int) EVCO::TUNE_PARAM].value = tune;
    vco.inputs[(int) EVCO::PITCH1_INPUT].value = pitch1;
    vco.inputs[(int) EVCO::PITCH2_INPUT].value = pitch2;
    vco.inputs[(int) EVCO::FM_INPUT].value = fm;

    vco.outputs[(int) EVCO::SAW_OUTPUT].active = true;
    vco.outputs[(int) EVCO::EVEN_OUTPUT].active = false;
    vco.outputs[(int) EVCO::TRI_OUTPUT].active = false;
    vco.outputs[(int) EVCO::SQUARE_OUTPUT].active = false;
    vco.outputs[(int) EVCO::SINE_OUTPUT].active = false;

    vco.step();
    const float desired = desiredPitch(vco);
    assertClose(vco._freq, desired, 1);     // todo: make better
}

static void foo()
{
    float k = 261.626f;
    float q = log2(k);

    float pitch = 4;
    float freqOld = k * powf(2.0f, pitch);
    float freqNew = powf(2.0f, pitch + q);
    printf("old = %f, new = %f\n", freqOld, freqNew);

    printf("q double = %f\n", log2(261.626));

    //assert(false);
}

static void test0()
{
    testx(3);

#if 0
    EVCO vco;

    vco.step();
    const float desired = desiredPitch(vco);
    assertEQ(vco._freq, desired);
#endif
}

/*
Measure straight saw,  normalizedFreq = 1.0f / (2 * 6.5f);

**** saw at 3392.307861 bins spacing = 0.672913***
neighbors at 3391.634949 and 3392.980774
harm at 6784.615723, 10176.923828, 13.5, 16.9

there are 18 peaks

H0: peak: freq=3392.152344, db=7.919424
H1: freq=6784.304688, db=1.042711
alias: freq=10177.129883, db=-1.219168
H2: freq=13569.282227, db=-2.484016
alias: freq=16961.435547, db=-3.714211
alias or something else?: freq=20353.587891, db=-5.093843
peak: freq=20356.951172, db=-47.860172
*/

const float sampleRate = 44100;
const float normalizedFreq = 1.0f / (2 * 6.5f);     // this will make alias freq spaced from harmonics
static void printOscPeaks(std::function<float()> func)
{
    const int windowSize = 64 * 1024;

    const double binSpacing = 44100.0 / windowSize;
    const float fundamental = sampleRate * normalizedFreq;

    printf("**** saw at %f bins spacing = %f***\n", fundamental, binSpacing);
    printf(" neighbors at %f and %f\n", fundamental - binSpacing, fundamental + binSpacing);
    printf(" harm at %f, %f\n", fundamental * 2, fundamental * 3);


    FFTDataCpx spectrum(windowSize);
    Analyzer::getSpectrum(spectrum, true, func);
    Analyzer::getAndPrintPeaks(spectrum, sampleRate);

}

static void testAlias1()
{
    SawOscillatorParams<float> params;
    SawOscillatorState<float> state;
    SawOscillator<float, false>::setFrequency(params, normalizedFreq);
    printOscPeaks([&state, &params]() {
        return 30 * SawOscillator<float, false>::runSaw(state, params);
        });


}

void testVCO()
{

    test0();
    testAliasSaw();


}