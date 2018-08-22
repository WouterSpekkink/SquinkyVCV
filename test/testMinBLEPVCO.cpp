#include <assert.h>

#include "asserts.h"
#include "MinBLEPVCO.h"

static float sampleTime = 1.0f / 44100.0f;


class TestMB
{
public:
    static void  setAllWaveforms(MinBLEPVCO* vco);
    static void test1();
};

// puts non-zero in all the waveforms
 void TestMB::setAllWaveforms(MinBLEPVCO* vco)
{
  //  float * wave = vco->_getWaveforms();
    for (int i = 0; i < (int)MinBLEPVCO::Waveform::END; ++i) {
        vco->waveformOutputs[i] = 1;
    }
}

void TestMB::test1()
{
    MinBLEPVCO vco;
    setAllWaveforms(&vco);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Saw), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Sin), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Square), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Tri), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Even), 0);

 
    vco.zeroOutputsExcept(MinBLEPVCO::Waveform::Saw);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Saw), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Sin), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Square), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Tri), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Even), 0);

    // special case for even and sin
    setAllWaveforms(&vco);
    vco.zeroOutputsExcept(MinBLEPVCO::Waveform::Even);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Saw), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Sin), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Square), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Tri), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Even), 0);

    setAllWaveforms(&vco);
    vco.zeroOutputsExcept(MinBLEPVCO::Waveform::Square);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Saw), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Sin), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Square), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Tri), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Even), 0);
}


static void test0()
{
    MinBLEPVCO vco;

    // Don't enable any waveforms
    vco.setSampleTime(sampleTime);
    vco.step();

    // should get nothing out.
    assert(vco.getWaveform(MinBLEPVCO::Waveform::Sin) == 0);
    assert(vco.getWaveform(MinBLEPVCO::Waveform::Square) == 0);
    assert(vco.getWaveform(MinBLEPVCO::Waveform::Saw) == 0);
    assert(vco.getWaveform(MinBLEPVCO::Waveform::Tri) == 0);
    assert(vco.getWaveform(MinBLEPVCO::Waveform::Even) == 0);
}


static void testSaw1()
{
    printf("**** testSaw1 disabled ******\n");
#if 0
    MinBLEPVCO vco;

    // Don't enable any waveforms
    vco.setSampleTime(sampleTime);
    vco.enableWaveform(MinBLEPVCO::Waveform::Saw, true);
    vco.step();

    // should get nothing out.
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Sin), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Square), 0);
    assertNE(vco.getWaveform(MinBLEPVCO::Waveform::Saw), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Tri), 0);
    assertEQ(vco.getWaveform(MinBLEPVCO::Waveform::Even), 0);
#endif
}
void testMinBLEPVCO()
{
    TestMB::test1();
    test0();
    testSaw1();
}