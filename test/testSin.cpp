
#include "Analyzer.h"
#include "SinOscillator.h"

#include <functional>

const static int numSamples = 16 * 1025;
const double sampleRate = 44100.0;
const double sampleTime = 1.0 / sampleRate;



static void testOsc(std::function<float(void)> osc)
{
    FFTDataCpx spectrum(numSamples);
    Analyzer::getSpectrum(spectrum, false, osc);

    Analyzer::getAndPrintFeatures(spectrum, 3, sampleRate, -140);
}

static void testPhaseAcc()
{
    double f = 700;
    SinOscillatorParams<float> params;
    SinOscillatorState<float> state;

   f = Analyzer::makeEvenPeriod(f, sampleRate, numSamples);
  // printf("sin freq = %f. bin spacing = %f\n", f, sampleRate / numSamples);
    
    SinOscillator<float, true>::setFrequency(params, float(f * sampleTime));
    testOsc([&state, &params]() {
        return SinOscillator<float, true>::run(state, params);
        });

}
void testSin()
{
    testPhaseAcc();
}