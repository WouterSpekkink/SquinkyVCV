#pragma once

#include <vector>
#include <functional>

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

   // void takeFFT(FFTDataCpx& out, const  float* samples, int numSamples);

   // fillFFTFrame(FFDataReal& out,)
    //std::function<double(double)> makeFunc_Audi
    static void getFreqResponse(FFTDataCpx& out, std::function<float(float)>);
    
    
};
