#pragma once

#include <vector>
class FFTDataCpx;


class Analyzer
{
public:
    Analyzer() = delete;
    static void printf(const FFTDataCpx&);

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
    
    
};
