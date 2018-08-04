
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

class AliasStats
{
public:
    float totalAliasDb;
    float totalAliasAWeighted;
    float maxAliasFreq;
};

#if 0
bool should(double freq)
{
//    return ((freq > 1680) && (freq < 1698));
    return ((freq > 3370) && (freq < 3400));
}
#endif


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

class FrequencySets
{
public:
    FrequencySets(double fundamental, double sampleRate);
    void expandFrequencies(const FFTDataCpx& spectrum);
    bool checkOverlap() const;

    std::set<double> harmonics;
    std::set<double> alias;

    void dump(const char *, const FFTDataCpx& spectrum) const;
private:
    static void expandFrequencies(std::set<double>&, const FFTDataCpx& spectrum);
};

inline FrequencySets::FrequencySets(double fundamental, double sampleRate)
{
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
}

inline void expandHelper(double& maxDb, bool& done, int& i, int deltaI, const FFTDataCpx& spectrum, std::set<double>& f)
{
    if (i >= spectrum.size() || i < 0) {
        done = true;
    } else {
        const double db = AudioMath::db(spectrum.getAbs(i));
        if (db < (maxDb - 10)) {
            done = true;
        } else {
            const double newFreq = FFT::bin2Freq(i, sampleRate, spectrum.size());
           // if (newFreq < 2600)
          //      printf("inserting new freq %f db=%f m=%f\n ", newFreq, db, maxDb);
            maxDb = std::max(maxDb, db);
            f.insert(newFreq);
        }
    }
    i += deltaI;
}

inline void FrequencySets::expandFrequencies(std::set<double>& f, const FFTDataCpx& spectrum)
{
    for (double freq : f) {
        const int bin = FFT::freqToBin(freq, sampleRate, spectrum.size());
        double maxDb = AudioMath::db(spectrum.getAbs(bin));

        // search upward
        bool done;
        int i;
        for (i = bin + 1, done = false; !done ; ) {
            expandHelper(maxDb, done, i, 1, spectrum, f);
        }

        for (i = bin - 1, done = false; !done; ) {
            expandHelper(maxDb, done, i, -1, spectrum, f);
        }
    }
}

inline void FrequencySets::dump(const char *label, const FFTDataCpx& spectrum) const
{
    printf("**** %s ****\n", label);
    for (auto f : harmonics) {
        int bin = FFT::freqToBin(f, sampleRate, spectrum.size());
        printf("harm at %.2f db:%.2f\n", f, AudioMath::db(spectrum.getAbs(bin)));
    }
    for (auto f : alias) {
        int bin = FFT::freqToBin(f, sampleRate, spectrum.size());
        printf("alias at %.2f db:%.2f\n", f, AudioMath::db(spectrum.getAbs(bin)));
    }
}

inline void FrequencySets::expandFrequencies(const FFTDataCpx& spectrum)
{
   // dump("before expand freq", spectrum);
    expandFrequencies(harmonics, spectrum);
    expandFrequencies(alias, spectrum);
 

    //dump("after expand freq", spectrum);
    assert(checkOverlap());

}



inline bool FrequencySets::checkOverlap() const
{
    std::vector<double> overlap;

    std::set_intersection(harmonics.begin(), harmonics.end(),
        alias.begin(), alias.end(),
        std::back_inserter(overlap));
    if (!overlap.empty()) {
        for (auto x : overlap) {
            printf("overlap at %f\n", x);
        }
    }
    return overlap.empty();
}

// probably don't need this
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


void testAlias(std::function<float()> func, double fundamental, int numSamples)
{
    printf("test alias fundamental=%f,%f,%f\n", fundamental, fundamental * 2, fundamental * 3);
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, func);
    FrequencySets frequencies(fundamental, sampleRate);
    assert(frequencies.checkOverlap());

    frequencies.expandFrequencies(spectrum);


    double totalSignal = 0;
    double totalAlias = 0;

    // let's look at every spectrum line
    for (int i=1; i<numSamples/2; ++i) {
        const double freq = FFT::bin2Freq(i, sampleRate, numSamples);
        const double mag = spectrum.getAbs(i);
        const double db = AudioMath::db(mag);

        const bool isHarmonic = freqIsInSet(freq, frequencies.harmonics);
        const bool isH2 = frequencies.harmonics.find(freq) != frequencies.harmonics.end();
        const bool isAlias = freqIsInSet(freq, frequencies.alias);
        const bool isA2 = frequencies.alias.find(freq) != frequencies.alias.end();
        assert(isH2 == isHarmonic);
        assert(isA2 == isAlias);
        assert(!isA2 || !isH2);

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

void testRawSaw(double normalizedFreq)
{
    const int numSamples = 64 * 1024;
    // adjust the freq to even

    double freq = Analyzer::makeEvenPeriod(sampleRate * normalizedFreq, sampleRate, numSamples);
    printf("\ndesired freq = %f, round %f\n", sampleRate * normalizedFreq, freq);

    // make saw osc at correct freq
    SawOscillatorParams<float> params;
    SawOscillatorState<float> state;
    SawOscillator<float, false>::setFrequency(params, (float) normalizedFreq);
    testAlias([&state, &params]() {
        return 30 * SawOscillator<float, false>::runSaw(state, params);
        }, freq, numSamples);

}


/*
First try:
desired freq = 844.180682, round 842.486572
test alias fundamental=842.486572,1684.973145,2527.459717
total sig = 3.239564 alias = 0.100040 ratiodb=-30.206276

desired freq = 1688.361365, round 1687.664795
test alias fundamental=1687.664795,3375.329590,5062.994385
total sig = 6.824180 alias = 0.158808 ratiodb=-32.663559

desired freq = 3376.722729, round 3375.329590
test alias fundamental=3375.329590,6750.659180,10125.988770
total sig = 3.512697 alias = 0.166856 ratiodb=-26.465975
Test passed. Press any key to continue...

first bin expand 6 db:

desired freq = 844.180682, round 842.486572
test alias fundamental=842.486572,1684.973145,2527.459717
total sig = 22.917906 alias = 1.563906 ratiodb=-23.319288

desired freq = 1688.361365, round 1687.664795
test alias fundamental=1687.664795,3375.329590,5062.994385
total sig = 17.118325 alias = 5.358352 ratiodb=-10.088601

desired freq = 3376.722729, round 3375.329590
test alias fundamental=3375.329590,6750.659180,10125.988770
total sig = 14.605277 alias = 6.552373 ratiodb=-6.962224
Test passed. Press any key to continue...

get overlap with 10db window, 16k fft, 6db ok
64k, 6db ok

*/

void testVCOAlias()
{
    testPitchQuantize();
    testRawSaw(1.0f / (8 * 6.53f));
    testRawSaw(1.0f / (4 * 6.53f));
    testRawSaw(1.0f / (2 * 6.53f));
}