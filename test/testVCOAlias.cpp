
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

/*

Next: examine the spectrum. make sure all freq in spectrum are signal or alias
*/


std::pair< std::vector<double>, std::vector<double>> getFrequencies(float fundamental, float sampleRate)
{
    std::vector<double> harmonics;
    std::vector<double> alias;
    const float nyquist = sampleRate / 2;
    bool done = false;
    for (int i = 1; !done; ++i) {
        double freq = fundamental * i;
        if (freq < nyquist) {
            harmonics.push_back(freq);
        } else {
            double over = freq - nyquist;
            if (over < nyquist) {
                freq = nyquist - over;
                alias.push_back(freq);
            } else {
                done = true;
            }
        }
    }
    return std::pair< std::vector<double>, std::vector<double>>(harmonics, alias);
}


void testAlias(std::function<float()> func, float fundamental)
{
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, func);
    Analyzer::getAndPrintFeatures(spectrum, 3, sampleRate);


    auto x = getFrequencies(fundamental, sampleRate);
    for (double h : x.first)
        printf("harmonic at %.2f\n", h);
    for (double a : x.second)
        printf("alias at %.2f\n", a);
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
        }, freq);

    

    //test it
}




void testVCOAlias()
{
    testRawSaw();
}