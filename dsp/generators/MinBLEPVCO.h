#pragma once

/* VCO core using MinBLEP to reduce aliasing.
 * Originally based on Befaco EvenVCO
 */

class MinBLEPVCO
{
public:
    friend class TestMB;
    enum class Waveform {Sin, Square, Saw, Tri, Even, END };

    void step();
   
    void setSampleTime(float time)
    {
        sampleTime = time;
    }

    void enableWaveform(Waveform wf, bool flag);
   
    float getWaveform(Waveform wf) const
    {
        return waveformOutputs[(int) wf];
    }

private:
   // bool waveformEnabled[Waveforms::END] = {false};
    float waveformOutputs[(int)Waveform::END] = {0};
    bool doSaw = false;
    bool doEven = false;
    bool doTri = false;
    bool doSquare = false;
    bool doSin = false;


    float sampleTime = 0;

    /**
     * It's merely a convenience that we use the waveforms enum for dispatcher.
     * Could use anything.
     */
    Waveform dispatcher = Waveform::Saw;
    int loopCounter = 0;

    /**
     * Waveform generation helper
     */
    void step_even(float deltaPhase);
    void step_saw(float deltaPhase);
    void step_sq(float deltaPhase);
    void step_sin(float deltaPhase);
    void step_tri(float deltaPhase);
    void step_all(float deltaPhase);

    void zeroOutputsExcept(Waveform);
  
};


inline void MinBLEPVCO::enableWaveform(Waveform wf, bool flag)
{
    switch (wf) {
        case Waveform::Saw:
            doSaw = flag;
            break;
        default:
            assert(false);
    }
   
}


void MinBLEPVCO::zeroOutputsExcept(Waveform  except)
{
    for (int i = 0; i < (int) Waveform::END; ++i) {
        const Waveform wf = Waveform(i);
        if (wf != except) {
            // if we do even, we do sin at same time
            if ((wf == Waveform::Sin) && (except == Waveform::Even)) {
            } else {
                waveformOutputs[i] = 0;
            }
        }
    }
}



inline void MinBLEPVCO::step()
{
    assert(sampleTime > 0);
    // We don't need to look for connected outputs every cycle.
    // do it less often, and store results.
    if (--loopCounter < 0) {
        loopCounter = 16;

#if 0
        doSaw = TBase::outputs[SAW_OUTPUT].active;
        doEven = TBase::outputs[EVEN_OUTPUT].active;
        doTri = TBase::outputs[TRI_OUTPUT].active;
        doSq = TBase::outputs[SQUARE_OUTPUT].active;
        doSin = TBase::outputs[SINE_OUTPUT].active;
#endif



        /**
         * Figure out which specialized processing function to run
         */
        if (doSaw && !doEven && !doTri && !doSquare && !doSin) {
            dispatcher = Waveform::Saw;
            zeroOutputsExcept(dispatcher);
        } else if (!doSaw && doEven && !doTri && !doSquare) {
            dispatcher = Waveform::Even;
            zeroOutputsExcept(dispatcher);
        } else if (!doSaw && !doEven && !doTri && !doSquare && doSin) {
            dispatcher = Waveform::Sin;
            zeroOutputsExcept(dispatcher);
        } else if (!doSaw && !doEven && doTri && !doSquare && !doSin) {
            dispatcher = Waveform::Tri;
            zeroOutputsExcept(dispatcher);
        } else if (!doSaw && !doEven && !doTri && doSquare && !doSin) {
            dispatcher = Waveform::Square;
            zeroOutputsExcept(dispatcher);
        } else {
            dispatcher = Waveform::END;
        }
    }

#if 0       // TODO:
    // Compute frequency, pitch is 1V/oct
    float pitch = 1.0 + roundf(TBase::params[OCTAVE_PARAM].value) + TBase::params[TUNE_PARAM].value / 12.0;
    pitch += TBase::inputs[PITCH1_INPUT].value + TBase::inputs[PITCH2_INPUT].value;
    pitch += TBase::inputs[FM_INPUT].value / 4.0;

    // Use lookup table for pitch lookup
    const float q = float(log2(261.626));       // move up to pitch range of even vco
    pitch += q;
    _freq = expLookup(pitch);


    // Advance phase
    float f = (_testFreq) ? _testFreq : _freq;
    float deltaPhase = clamp(f * TBase::engineGetSampleTime(), 1e-6f, 0.5f);

    // call the dedicated dispatch routines for the special case waveforms.
    switch (dispatcher) {
        case SAW_OUTPUT:
            step_saw(deltaPhase);
            break;
        case EVEN_OUTPUT:
            step_even(deltaPhase);
            break;
        case SINE_OUTPUT:
            step_sin(deltaPhase);
            break;
        case TRI_OUTPUT:
            step_tri(deltaPhase);
            break;
        case SQUARE_OUTPUT:
            step_sq(deltaPhase);
            break;
        case NUM_OUTPUTS:
            step_all(deltaPhase);
            break;
        default:
            assert(false);
    }
#endif
}

