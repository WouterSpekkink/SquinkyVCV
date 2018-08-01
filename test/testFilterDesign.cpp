
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
    double passBandStop=0;        // -3db point in passband
    double stopBandStop=0;        // within 3db of stopBandAtten
    double stopBandAttenuation=0;
};

LowpassStats characterizeLowpassFilter(std::function<float(float)> filter)
{
    LowpassStats ret;

    const int numSamples = 16 * 1024;
    FFTDataCpx response(numSamples);
    Analyzer::getFreqResponse(response, filter);
    const float gain0 = std::abs(response.get(0));

    return ret;

}

void printStats(const char * label, const LowpassStats& stats)
{

}

void testButter()
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

    const LowpassStats stats = characterizeLowpassFilter(filter);
    printStats("butter6/100", stats);
}

void testFilterDesign()
{
    testButter();
}