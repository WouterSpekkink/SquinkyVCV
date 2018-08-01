
#include "Analyzer.h"
#include "BiquadFilter.h"
#include "BiquadParams.h"
#include "BiquadState.h"
#include "ButterworthFilterDesigner.h"
//#include "FFT.h"

#include <functional>


//std::function<float(float)> filter
const float sampleRate = 44100;

class LowpassStats
{
public:
    double passBandStop = 0;        // -3db point in passband
    double stopBandStart = 0;        // within 3db of stopBandAtten
    double stopBandAttenuation = 0;
};

LowpassStats characterizeLowpassFilter(std::function<float(float)> filter, float stopBandGain = -200)
{
    LowpassStats ret;
    bool inPassBand = true;
    bool inStopBand = false;

    const int numSamples = 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);
    const float gain0 = std::abs(response.get(0));

    const double _3DbDown = AudioMath::db(gain0) - 3;

    ret.stopBandAttenuation = -1000;
    float minDb = 100;
    for (int i = 0; i < numSamples; ++i) {
        const float db = (float) AudioMath::db(std::abs(response.get(i)));
        const float freq = FFT::bin2Freq(i, sampleRate, numSamples);

        if (inPassBand && db < _3DbDown) {
            ret.passBandStop = freq;
            inPassBand = false;
        }
        // if we bounce back up from min, we are in stop band.
        // only works for Ch I and elliptic
        if (!inPassBand && !inStopBand && (db > (minDb + 3))) {
            inStopBand = true;
            ret.stopBandStart = freq;
        }
        // If we are below something passed in, we are in stop.
        if (!inPassBand && !inStopBand && (db < stopBandGain)) {
            inStopBand = true;
            ret.stopBandStart = freq;
            ret.stopBandAttenuation = db;
        }
        minDb = std::min(minDb, db);
        if (inStopBand) {
            ret.stopBandAttenuation = std::max<double>(ret.stopBandAttenuation, db);
        }
    }

    return ret;

}

void printStats(const char * label, const LowpassStats& stats)
{
    printf("%s band=%.2f,%.2f atten=%.2f\n", label, stats.passBandStop, stats.stopBandStart, stats.stopBandAttenuation);
}

void testButter6()
{
    const float Fc = 100;
    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;

    ButterworthFilterDesigner<float>::designSixPoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<float>::run(x, state, params);
        return x;
    };

    const LowpassStats stats = characterizeLowpassFilter(filter, -60);
    printStats("butter6/100", stats);
}

void testButter8()
{
    const float Fc = 100;
    BiquadParams<float, 4> params;
    BiquadState<float, 4> state;

    ButterworthFilterDesigner<float>::designEightPoleLowpass(
        params, Fc / sampleRate);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<float>::run(x, state, params);
        return x;
    };

    const LowpassStats stats = characterizeLowpassFilter(filter, -60);
    printStats("butter8/100", stats);
}

void designSixPoleCheby(BiquadParams<float, 3>& outParams, float frequency, float ripple)
{
    assert(frequency > 0 && frequency < .5);
    using Filter = Dsp::ChebyILowPass<6, 1>;
    Filter f;
    f.SetupAs(frequency, ripple);
    assert(f.GetStageCount() == 3);
    BiquadFilter<float>::fillFromStages(outParams, f.Stages(), f.GetStageCount());
}

void testCheby6_1()
{
    const float Fc = 100;
    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;

    designSixPoleCheby(params, Fc / sampleRate, 6);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<float>::run(x, state, params);
        return x;
    };

    const LowpassStats stats = characterizeLowpassFilter(filter, -60);
    printStats("cheby6/100/6", stats);
}

void testCheby6_3()
{
    const float Fc = 100;
    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;

    designSixPoleCheby(params, Fc / sampleRate, 3);

    std::function<float(float)> filter = [&state, &params](float x) {
        x = (float) BiquadFilter<float>::run(x, state, params);
        return x;
    };

    const LowpassStats stats = characterizeLowpassFilter(filter, -60);
    printStats("cheby6/100/3", stats);
}

void testFilterDesign()
{
    testButter6();
    testButter8();
    testCheby6_1();
    testCheby6_3();
}