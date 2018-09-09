#pragma once

/**
 * A traditional decimator, using IIR filters for interpolation
 *
 * template parameter OVERSAMPLE is the 
 * decimation rate.
 */
template <int OVERSAMPLE>
class IIRDecimator
{
public:
    float process(const float * input)
    {
        float x = 0;
        for (int i = 0; i < OVERSAMPLE; ++i) {
            x = BiquadFilter<float>::run(input[i], state, params);
        }
        return x;
    }

    /**
    * cutoff is normalized freq (.5 = nyquist)
    */
    void setCutoff(float cutoff)
    {
        assert(cutoff > 0 && cutoff < .5f);
        ButterworthFilterDesigner<float>::designSixPoleLowpass(params, cutoff);
    }
private:
    BiquadParams<float, 3> params;
    BiquadState<float, 3> state;
};