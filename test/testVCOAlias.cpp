
#include "Analyzer.h"
#include "SawOscillator.h"

const float sampleRate = 44100;
//const float normalizedFreq = 1.0f / (4 * 6.53f);     // this will make alias freq spaced from harmonics
//const int numSamples = 16 * 1024;

const float normalizedFreq = 1.0f / (1000);     // this will make alias freq spaced from harmonics
const int numSamples = 1024;


class AliasStats
{
public:
    float totalAliasDb;
    float totalAliasAWeighted;
    float maxAliasFreq;
};



void testAlias(std::function<float()> func)
{
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, func);
    Analyzer::getAndPrintFeatures(spectrum, 3, sampleRate);


    // measure spectrum with not
}

void testRawSaw()
{
    // adjust the freq to even
   // static float makeEvenPeriod(float desiredFreq, float sampleRate, int numSamples);
    float freq = Analyzer::makeEvenPeriod(sampleRate * normalizedFreq, sampleRate, numSamples);
    printf("desired freq = %f, round %f\n", sampleRate * normalizedFreq, freq);

    // make saw osc at correct freq
    SawOscillatorParams<float> params;
    SawOscillatorState<float> state;
    SawOscillator<float, false>::setFrequency(params, normalizedFreq);
    testAlias([&state, &params]() {
        return 30 * SawOscillator<float, false>::runSaw(state, params);
        });


    //test it
}




void testVCOAlias()
{
    testRawSaw();
}