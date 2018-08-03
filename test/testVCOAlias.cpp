
#include "Analyzer.h"
#include "asserts.h"
#include "SawOscillator.h"
#include "SinOscillator.h"



static void testPitchQuantize()
{
    const double sampleRate = 44100;
    const int numSamples = 16;
    const double inputFreq = 44100.0 / 4.0;
    double freq = Analyzer::makeEvenPeriod(inputFreq, sampleRate, numSamples);

    // check that quantized pitch is in the bin we expect.
    assertEQ(freq, FFT::bin2Freq(4, sampleRate, numSamples));

    // make saw osc at correct freq
    SinOscillatorParams<double> params;
    SinOscillatorState<double> state;
    SinOscillator<double, false>::setFrequency(params, 1.0 / 4.0);
   

    // check that spectrum has only a single freq
    std::function<float()> func = [&state, &params]() {
        return float(30 * SinOscillator<double, false>::run(state, params));
        };
    FFTDataCpx spectrum(numSamples);


    Analyzer::getSpectrum(spectrum, false, func);
    for (int i = 0; i < numSamples / 2; ++i) {
        const float abs = spectrum.getAbs(i);
        if (i == 4) {
            assertGE(abs, .5);
        }
        else {
            assertLT(abs, 0.000000001);
        }
    }
}

const float sampleRate = 44100;

// this is a typical case
const float normalizedFreq = 1.0f / (4 * 6.53f);     // this will make alias freq spaced from harmonics
const int numSamples = 16 * 1024;


// this is a crazy torture case
//const float normalizedFreq = 1.0f / (1000);     // this will make alias freq spaced from harmonics
//const int numSamples = 256 * 1024;


class AliasStats
{
public:
    float totalAliasDb;
    float totalAliasAWeighted;
    float maxAliasFreq;
};

bool should(double freq)
{
//    return ((freq > 1680) && (freq < 1698));
    return ((freq > 3370) && (freq < 3400));
}


#if 0   // this is the problem right now - we are missing peaks
std::vector<int> getLocalPeaks(const FFTDataCpx& spectrum)
{
    std::vector<int> ret;
    for (int i = 0; i < spectrum.size(); ++i) {
        const double freq = FFT::bin2Freq(i, 44100, spectrum.size());
       // const bool print = (freq > 30 && freq < 60);
        const float lastMag = (i == 0) ? 0 : std::abs(spectrum.get(i - 1));
        const float nextMag = (i == (spectrum.size()-1)) ? 0 : std::abs(spectrum.get(i + 1));
        const float mag = std::abs(spectrum.get(i));
       
        if (should(freq)) {
            printf("getloc mag=%.2f last=%.2f next=%.2f bin %d f=%f\n", mag, lastMag, nextMag, i, freq);
       
        }
        if (mag > lastMag && mag > nextMag) {
            ret.push_back(i);
        }

    }
    return ret;
}
#endif

/*

Next: examine the spectrum. make sure all freq in spectrum are signal or alias
*/

std::pair< std::set<double>, std::set<double>> getFrequencies(double fundamental, double sampleRate)
{
    std::set<double> harmonics;
    std::set<double> alias;
    const double nyquist = sampleRate / 2;
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



bool freqIsInSet(double freq, const std::set<double> set)
{
    auto lb = set.lower_bound(freq);
    bool isHarmonic = false;
    if (lb != set.end() && freq <= *lb) {
        const double ratio = *lb / freq;
        isHarmonic = (*lb / freq < 1.00001);
    }
    if (isHarmonic) {
        return true;
    }


    // does this second try catch anything?
    auto xx = lb;
    --xx;
    if (xx != set.end() && freq >= *xx) {
        isHarmonic = ((freq / *xx) < 1.00001);
    }
    if (false) {
        printf("freqInSet(%f) looked at %f ", freq, *lb);
        if (xx != set.end()) printf("and %f", *xx);

        printf("\n");
    }
    return isHarmonic;
}

void testAlias(std::function<float()> func, double fundamental)
{
    printf("test alias fundamental=%f,%f,%f\n", fundamental, fundamental * 2, fundamental * 3);
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, func);
    auto frequencies = getFrequencies(fundamental, sampleRate);
#if 0
    for (double h : frequencies.first)
        printf("harmonic at %f\n", h);
    for (double a : frequencies.second)
        printf("alias at %f\n", a);
#endif

    double totalSignal = 0;
    double totalAlias = 0;

    // let's look at every spectrum line
    for (int i=1; i<numSamples/2; ++i) {
        const double freq = FFT::bin2Freq(i, sampleRate, numSamples);
        const double mag = spectrum.getAbs(i);
        const double db = AudioMath::db(mag);

        const bool isHarmonic = freqIsInSet(freq, frequencies.first);
        const bool isH2 = frequencies.first.find(freq) != frequencies.first.end();
        const bool isAlias = freqIsInSet(freq, frequencies.second);
        const bool isA2 = frequencies.second.find(freq) != frequencies.second.end();
        assert(isH2 == isHarmonic);
        assert(isA2 == isAlias);
        assert(!isA2 || !isH2);

#if 0
        if (isAlias || isHarmonic) {
          
            printf("freq %f, harm=%d alias=%d db=%f\n", freq, isHarmonic, isAlias,
                db);
            freqIsInSet(freq, frequencies.first);

        }
#endif
        if (isH2) {
            totalSignal += mag;
        }
        if (isA2) {
            totalAlias += mag;
        }
    }

    printf("total sig = %f alias = %f ratiodb=%f\n",
        totalSignal, totalAlias, AudioMath::db(totalAlias / totalSignal));
}

void testRawSaw()
{
    // adjust the freq to even
   // static float makeEvenPeriod(float desiredFreq, float sampleRate, int numSamples);
    double freq = Analyzer::makeEvenPeriod(sampleRate * normalizedFreq, sampleRate, numSamples);
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
    testPitchQuantize();
    testRawSaw();
}