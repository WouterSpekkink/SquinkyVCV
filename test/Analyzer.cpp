#include <assert.h>

#include "Analyzer.h"
#include "AudioMath.h"
#include "FFT.h"
#include "FFTData.h"
#include "SinOscillator.h"

void Analyzer::print(const FFTDataCpx&)
{

}

std::vector<Analyzer::FPoint> Analyzer::getFeatures(const FFTDataCpx& data, float sensitivityDb, float sampleRate)
{
    std::vector<FPoint> ret;
    float lastDb = 10000000000;
    for (int i = 0; i < data.size(); ++i) {
        const float db = (float) AudioMath::db( std::abs(data.get(i)));
        if (std::abs(db - lastDb) >= sensitivityDb) {
            float freq = FFT::bin2Freq(i, sampleRate, data.size());
            FPoint p(freq, db);
            ret.push_back(p);
        }
    }
    return ret;
}

void Analyzer::getFreqResponse(FFTDataCpx& out, std::function<float(float)> func)
{
    const float db = (float) AudioMath::db(1);

    for (int i = 0; i < out.size(); ++i) {
        out.set(i, cpx(db, 0));
    }

    //  TestBuffers<float> buf;

    // First set up a test signal (noise for now)
    assert(false);
    // Run the test signal though func, capture output in fft real
    assert(false);
    // then take the inverse fft
    assert(false);
}


void Analyzer::generateSweep(float sampleRate, float* out, int numSamples, float minFreq, float maxFreq)
{
    assert(maxFreq > minFreq);
    const double minLog = std::log2(minFreq);
    const double maxLog = std::log2(maxFreq);
    const double delta = (maxLog - minLog) / numSamples;

    SinOscillatorParams<double> params;
    SinOscillatorState<double> state;

    double fLog = minLog;
    for (int i = 0; i < numSamples; ++i, fLog+=delta) {
        const double freq = std::pow(2, fLog);
        assert(freq < (sampleRate / 2));

        SinOscillator<double, false>::setFrequency(params, freq / sampleRate);
        double val = SinOscillator<double, false>::run(state, params);

       // ::printf("out[%d] = %f f=%f\n", i, val, freq);
        out[i] = (float) val;
    }
}