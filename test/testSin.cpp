
#include "Analyzer.h"
#include "SinOscillator.h"

#include <functional>

const static int numSamples = 256 * 1024;
const double sampleRate = 44100.0;
const double sampleTime = 1.0 / sampleRate;

const double testFreq = 700;



static void testOsc(const std::string& title, double freq, std::function<float(void)> osc)
{
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, osc);

   // Analyzer::getAndPrintFeatures(spectrum, 3, sampleRate, -140);

    double signal = 0;
    double noise = 0;
    int matches = 0;
    for (int i = 0; i < numSamples / 2; ++i) {
        const double f = FFT::bin2Freq(i, sampleRate, numSamples);
        const double mag = spectrum.getAbs(i);

       // if (f > 600 && f < 800) printf("%.2f: %f\n", f, mag);
      //  if (mag > .00000001)  printf("%.2f: %e\n", f, mag);
        if (freq == f) {
            ++matches;
            signal += mag;
        } else {
            noise += mag;
        }
    }
    assert(matches == 1);
    assert(noise > 0);
    printf("%s SNR = %f (db)\n", title.c_str(), AudioMath::db(signal / noise));
}

template <typename T>
static void testPhaseAcc()
{
    const double f = Analyzer::makeEvenPeriod(testFreq, sampleRate, numSamples);
    SinOscillatorParams<T> params;
    SinOscillatorState<T> state;

    std::string title = (sizeof(T) > 4) ? "double w lookup" : "float w lookup";
    SinOscillator<T, true>::setFrequency(params, float(f * sampleTime));
    testOsc(title, f, [&state, &params]() {
        return (float) SinOscillator<T, true>::run(state, params);
        });
}


static void testPhaseAcc2()
{
    const double f = Analyzer::makeEvenPeriod(testFreq, sampleRate, numSamples);

    std::string title = "phase acc std::sin";

    double acc = 0;
    testOsc(title, f, [f, &acc]() {
           // return SinOscillator<T, true>::run(state, params);

            const double inc = f * sampleTime;
            acc += inc;
            if (acc > 1) {
                acc -= 1;
            }
            return float(std::sin(acc * AudioMath::Pi * 2));
        });
}

void testSin()
{
    testPhaseAcc<float>();
    testPhaseAcc<double>();
    testPhaseAcc2();
}