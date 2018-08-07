
#include <assert.h>

#include "Analyzer.h"
#include "asserts.h"
#include "EvenVCO.h"
#include "FunVCO.h"
#include "SawOscillator.h"
#include "TestComposite.h"


using EVCO = EvenVCO <TestComposite>;
using FUN = VoltageControlledOscillator<16, 16>;

float desiredPitch(const EVCO& vco)
{
    // This is just the original code as reference
    float pitch = 1.0f + roundf(vco.params[(int) EVCO::OCTAVE_PARAM].value) + vco.params[(int) EVCO::TUNE_PARAM].value / 12.0f;
    pitch += vco.inputs[(int) EVCO::PITCH1_INPUT].value + vco.inputs[(int) EVCO::PITCH2_INPUT].value;
    pitch += vco.inputs[(int) EVCO::FM_INPUT].value / 4.0f;

    float freq = 261.626f * powf(2.0f, pitch);
   // printf("theirs: pitch = %f exp = %f\n", pitch, freq);
    return freq;
}


static void testx(float octave, float tune = 0, float pitch1 = 0, float pitch2 = 0, float fm = 0)
{
    EVCO vco;

    vco.params[(int) EVCO::OCTAVE_PARAM].value = octave;
    vco.params[(int) EVCO::TUNE_PARAM].value = tune;
    vco.inputs[(int) EVCO::PITCH1_INPUT].value = pitch1;
    vco.inputs[(int) EVCO::PITCH2_INPUT].value = pitch2;
    vco.inputs[(int) EVCO::FM_INPUT].value = fm;

    vco.outputs[(int) EVCO::SAW_OUTPUT].active = true;
    vco.outputs[(int) EVCO::EVEN_OUTPUT].active = false;
    vco.outputs[(int) EVCO::TRI_OUTPUT].active = false;
    vco.outputs[(int) EVCO::SQUARE_OUTPUT].active = false;
    vco.outputs[(int) EVCO::SINE_OUTPUT].active = false;

    vco.step();
    const float desired = desiredPitch(vco);

   // printf("test, oct=%f, freq=%.2f desired=%.2f\n", octave, vco._freq, desired);
    assertClose(vco._freq, desired, 1.5);     // todo: make better
}

/*
octave: -5 to 4
tune: -7 to 7
pitch1: ? -5,5?

 */
static void testInit()
{
    EVCO vco;

    vco.step();
    const float desired = desiredPitch(vco);
    assertClose(vco._freq, desired, 1);         // todo: tighten up
}

static void testOctaves()
{
    EVCO vco;
    for (int octave = -5; octave <= 4; ++octave) {
        testx(octave);
    }
}

// test that we go up to 20k
static void testMaxFreq()
{
    testx(4, 7, 0, 0);
    testx(4, 7, 1, 0);
    testx(4, 7, 0, 1);

}

static void testMinFreq()
{
    testx(-5, -7, 0, 0);
    testx(-5, -7, -2, 0);
}

void testVCO()
{
    testInit();
    testOctaves();
    testMaxFreq();
    testMinFreq();
}