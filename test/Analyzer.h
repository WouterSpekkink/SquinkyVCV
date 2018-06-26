#pragma once

#include <vector>
#include <functional>

#include "FFTData.h"
#include "FFT.h"


class Analyzer
{
public:
    Analyzer() = delete;

    class FPoint
    {
    public:
        FPoint(float f, float g) : freq(f), gain(g)
        {
        }
        float freq;
        float gain;
    };

    static std::vector<FPoint> getFeatures(const FFTDataCpx&, float sensitivityDb, float sampleRate);
    static void getAndPrintFeatures(const FFTDataCpx&, float sensitivityDb, float sampleRate);
    static int getMax(const FFTDataCpx&);

    /**
     * 0 = low freq
     * 1 = peak
     * 2 = high
     * dbAtten (typically -3
     */
    static std::tuple<int, int, int> getMaxAndShoulders(const FFTDataCpx&, float dbAtten);

    static void getFreqResponse(FFTDataCpx& out, std::function<float(float)>);

    static float getSlope(const FFTDataCpx& response, float fTest, float sampleRate);

    static void generateSweep(float sampleRate, float* out, int numSamples, float minFreq, float maxFreq);
};
