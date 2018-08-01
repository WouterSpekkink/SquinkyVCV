
#include "Analyzer.h"
#include "SawOscillator.h"

const float sampleRate = 44100;
//const float normalizedFreq = 1.0f / (4 * 6.53f);     // this will make alias freq spaced from harmonics
//const int numSamples = 16 * 1024;

const float normalizedFreq = 1.0f / (1000);     // this will make alias freq spaced from harmonics
const int numSamples = 256 * 1024;


class AliasStats
{
public:
    float totalAliasDb;
    float totalAliasAWeighted;
    float maxAliasFreq;
};

std::vector<int> getLocalPeaks(const FFTDataCpx& spectrum)
{
    std::vector<int> ret;
    for (int i = 0; i < spectrum.size(); ++i) {
        const float freq = FFT::bin2Freq(i, 44100, spectrum.size());
        const bool print = (freq > 30 && freq < 60);
        const float lastMag = (i == 0) ? 0 : std::abs(spectrum.get(i - 1));
        const float nextMag = (i == (spectrum.size()-1)) ? 0 : std::abs(spectrum.get(i + 1));
        const float mag = std::abs(spectrum.get(i));
       // if (print) printf("mag=%.2f last=%.2f next=%.2f bin %d f=%.2f\n", mag, lastMag, nextMag, i, freq);
        if (mag > lastMag && mag > nextMag) {
            ret.push_back(i);
        }

    }
    return ret;
}

/*

Next: examine the spectrum. make sure all freq in spectrum are signal or alias
*/

std::pair< std::set<double>, std::set<double>> getFrequencies(float fundamental, float sampleRate)
{
    std::set<double> harmonics;
    std::set<double> alias;
    const float nyquist = sampleRate / 2;
    bool done = false;
    for (int i = 1; !done; ++i) {
        double freq = fundamental * i;
        if (freq < nyquist) {
            //harmonics.push_back(freq);
            harmonics.insert(freq);
        } else {
            double over = freq - nyquist;
            if (over < nyquist) {
                freq = nyquist - over;
                //alias.push_back(freq);
                alias.insert(freq);
            } else {
                done = true;
            }
        }
    }
    return std::pair< std::set<double>, std::set<double>>(harmonics, alias);
}

bool freqIsInSet(float freq, const std::set<double> set)
{
    auto lb = set.lower_bound(freq);
    auto xx = lb;
    --xx;

    // TODO: are the frequencies as close as they should be?
    // should we quantize harmonic frequencies? (yes!)
    bool isHarmonic = false;
    if (xx != set.end() && lb != set.end()) {
        if (freq >= *xx && freq <= *lb) {
            if ((freq / *xx) < 1.00001) {
                isHarmonic = true;
            } else if (*lb / freq < 1.00001) {
                isHarmonic = true;
            }
       }
    }
    return isHarmonic;
}
void testAlias(std::function<float()> func, float fundamental)
{
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, func);

   // Analyzer::getAndPrintFeatures(spectrum, 3, sampleRate);


    auto frequencies = getFrequencies(fundamental, sampleRate);
#if 0
    for (double h : x.first)
        printf("harmonic at %.2f\n", h);
    for (double a : x.second)
        printf("alias at %.2f\n", a);
#endif
   

    auto peaks = getLocalPeaks(spectrum);
    for (int peak : peaks) {
        const float freq = FFT::bin2Freq(peak, sampleRate, numSamples);
#if 0
        if (freq < 2000) {

           
            bool isHarmonic = freqIsInSet(freq, frequencies.first);

            printf("peak bin %d freq %f h=%d\n", peak, freq, isHarmonic);
        }
#endif
        const bool isHarmonic = freqIsInSet(freq, frequencies.first);
        const bool isAlias = freqIsInSet(freq, frequencies.second);
        if (!isHarmonic && !isAlias) {
            printf("freq is not either %f\n", freq);
            assert(freq < 1);           // don't care about dc
            // we need magnitude in here
        }
        if (isHarmonic && isAlias) {
            printf("freq is both %f\n", freq);
            freqIsInSet(freq, frequencies.second);
            assert(false);
        }

    }
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