#pragma once

#include <vector>
#include <functional>

#include "FFTData.h"
//class FFTDataCpx;


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
     */
    static std::tuple<int, int, int> getMaxAndShoulders(const FFTDataCpx&);

    static void getFreqResponse(FFTDataCpx& out, std::function<float(float)>);
    static void generateSweep(float sampleRate, float* out, int numSamples, float minFreq, float maxFreq);
};
