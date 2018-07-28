
#include <assert.h>

#include "Analyzer.h"
#include "asserts.h"
#include "EvenVCO.h"
#include "FunVCO.h"
#include "SawOscillator.h"
#include "TestComposite.h"


using EVCO = EvenVCO <TestComposite>;
using FUN = VoltageControlledOscillator<16,16>;

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
Measure straight saw,  normalizedFreq = 1.0f / (4 * 6.5f);
freq=1688.36 db=8.191729  
freq=8441.81 db=-5.991078  
freq=13506.89 db=-10.405733  
freq=15195.25 db=-11.574288  
freq=18571.98 db=-13.661299  
alias:
freq=20462.94 db=-16.407371  
freq=18774.58 db=-16.857035  
freq=17086.22 db=-17.191533  
freq=15397.86 db=-17.510285  
freq=13709.50 db=-17.816966  
freq=12021.13 db=-18.114632  
freq=10332.77 db=-18.405863  
freq=8644.41 db=-18.692861  
freq=6956.05 db=-18.977511 
freq=5267.69 db=-19.261458 
freq=3579.33 db=-19.546149 
freq=1890.97 db=-19.832927 
freq=202.60 db=-20.125769 

Origiginal fun:


*/

const float sampleRate = 44100;
const float normalizedFreq = 1.0f / (4 * 6.53f);     // this will make alias freq spaced from harmonics
static void printOscPeaks(std::function<float()> func)
{
    const int windowSize = 64 * 1024;

    const double binSpacing = 44100.0 / windowSize;
    const float fundamental = sampleRate * normalizedFreq;

    std::vector<double> harmonics;
    std::vector<double> alias;
    std::vector<double> interesting;
    for (int i = 1; i < 30; ++i) {
        double freq = normalizedFreq * i;
        if (freq < .5) {
            harmonics.push_back(freq * sampleRate);
            interesting.push_back(freq* sampleRate);
        } else {
            double over = freq - .5;
            if (over < .5) {
                freq = .5 - over;
                alias.push_back(freq* sampleRate);
                interesting.push_back(freq* sampleRate);
            }
        }
    }
    printf("******* freq = %f **********\n", fundamental);
    printf(" harmonics at ");
    for (auto x : harmonics) printf("%f ", x);
    printf("\n");
    printf(" alias at ");
    for (auto x : alias) printf("%f ", x);
    printf("\n");

  //  printf("**** saw at %f bins spacing = %f***\n", fundamental, binSpacing);
  //  printf(" neighbors at %f and %f\n", fundamental - binSpacing, fundamental + binSpacing);
  //  printf(" harm at %f, %f\n", fundamental * 2, fundamental * 3);


    FFTDataCpx spectrum(windowSize);
    Analyzer::getSpectrum(spectrum, true, func);

    Analyzer::getAndPrintFreqOfInterest(spectrum, sampleRate, harmonics);
    printf("alias:\n");
    Analyzer::getAndPrintFreqOfInterest(spectrum, sampleRate, alias);
   // Analyzer::getAndPrintPeaks(spectrum, sampleRate, -10);
   
    
    //Analyzer::getAndPrintFreqOfInterest(spectrum, sampleRate, interesting);
    // 27 with -20
    // 79 with -30
    // 328 with -40
    // new criteria - mag > avgx3:, 27, 79..
    // avx4: 15, 48, 189
}

static void testAliasSaw()
{
    SawOscillatorParams<float> params;
    SawOscillatorState<float> state;
    SawOscillator<float, false>::setFrequency(params, normalizedFreq);
    printOscPeaks([&state, &params]() {
        return 30 * SawOscillator<float, false>::runSaw(state, params);
        });


}

static void testAliasFun()
{

    FUN vco;
    vco.sawEnabled = true;
    vco.sinEnabled = false;
    vco.sqEnabled = false;
    vco.triEnabled = false;
    vco.init();

    printOscPeaks([&vco]() {
        const float deltaTime = 1.0f / sampleRate;
        vco.process(deltaTime, 0);
        return vco.saw();
        });
}

static void testAliasFunOrig()
{

    VoltageControlledOscillatorOrig<16, 16> vco;
    vco.freq = sampleRate * normalizedFreq;
    vco.sampleTime = 1.0f / sampleRate;
   
    printOscPeaks([&vco]() {
        const float deltaTime = 1.0f / sampleRate;
        vco.process(deltaTime, 0);
        return 15 * vco.saw();
        });
}
void testVCO()
{

    test0();
    testAliasSaw();
   //testAliasFun();
   // testAliasFunOrig();


}