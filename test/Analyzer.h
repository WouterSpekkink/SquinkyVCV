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
        FPoint(float f, float g) : freq(f), gainDb(g)
        {
        }
        float freq;
        float gainDb;
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

    /**
     * Calculates the frequency response of func 
     * by calling it with a known test signal.
     */
    static void getFreqResponse(FFTDataCpx& out, std::function<float(float)> func);

    /**
     * Caclulates the spectrum of func 
     * by calling it can capturing its output
     */
    static void getSpectrum(FFTDataCpx& out, std::function<float()> func);

    static float getSlope(const FFTDataCpx& response, float fTest, float sampleRate);

    static void generateSweep(float sampleRate, float* out, int numSamples, float minFreq, float maxFreq);
};
