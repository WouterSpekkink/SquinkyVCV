#pragma once

/**
 * Outputs a stair-step decimation.
 * So maybe it's the integral or a decimation?
 */
class Decimator
{
public:
    float clock(bool& needsInput)
    {
        needsInput = false;
        return memory;
    }
    void acceptData(float data)
    {
        memory = data;
    }

    void setDecimationRate(float r)
    {
        rate = r;
    }

private:
    float rate=0;
    float memory=0;
    float phaseAccumulator = 0;
};
