
#include "Analyzer.h"
#include "AudioMath.h"
#include "FFT.h"
#include "FFTData.h"

void Analyzer::printf(const FFTDataCpx&)
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
}