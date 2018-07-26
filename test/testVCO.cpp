
#include <assert.h>

#include "asserts.h"
#include "EvenVCO.h"
#include "TestComposite.h"


using EVCO = EvenVCO <TestComposite>;

#if 0
// Compute frequency, pitch is 1V/oct
float pitch = 1.0 + roundf(TBase::params[OCTAVE_PARAM].value) + TBase::params[TUNE_PARAM].value / 12.0;
pitch += TBase::inputs[PITCH1_INPUT].value + TBase::inputs[PITCH2_INPUT].value;
pitch += TBase::inputs[FM_INPUT].value / 4.0;


// float freq = 261.626 * powf(2.0, pitch);
#endif

float desiredPitch(const EVCO& vco)
{
    float pitch = 1.0f + roundf(vco.params[(int)EVCO::OCTAVE_PARAM].value) + vco.params[(int) EVCO::TUNE_PARAM].value / 12.0f;
    pitch += vco.inputs[(int) EVCO::PITCH1_INPUT].value + vco.inputs[(int) EVCO::PITCH2_INPUT].value;
    pitch += vco.inputs[(int) EVCO::FM_INPUT].value / 4.0f;
    printf("theirs: pitch = %f exp = %f\n", pitch, powf(2.0, pitch));
    float freq = 261.626f * powf(2.0f, pitch);
    return freq;
}


static void testx(float octave, float tune=0, float pitch1=0, float pitch2=0, float fm=0)
{
    EVCO vco;

    vco.params[(int) EVCO::OCTAVE_PARAM].value = octave;
    vco.params[(int) EVCO::TUNE_PARAM].value = tune;
    vco.inputs[(int) EVCO::PITCH1_INPUT].value = pitch1;
    vco.inputs[(int) EVCO::PITCH2_INPUT].value = pitch2;
    vco.inputs[(int) EVCO::FM_INPUT].value = fm;

    vco.step();
    const float desired = desiredPitch(vco);
    assertClose(vco._freq, desired, 1);     // todo: make better
}

static void test0()
{
    testx(3);
#if 0
    EVCO vco;
 
    vco.step();
    const float desired = desiredPitch(vco);
    assertEQ(vco._freq, desired);
#endif
}

void testVCO()
{

    test0();
}