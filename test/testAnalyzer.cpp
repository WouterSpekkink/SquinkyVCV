
#include <assert.h>
#include <vector>

#include "Analyzer.h"
#include "asserts.h"
#include "AudioMath.h"
#include "FFTData.h"
#include "MeasureTime.h"

static void ana0()
{
    FFTDataCpx f(16);
    Analyzer::print(f);
}

static void ana1()
{
    FFTDataCpx x(1);
    x.set(0, cpx( float(AudioMath::gainFromDb(0)), 0));

    auto data = Analyzer::getFeatures(x, 3, 44100);
    assert(data.size() == 1);
    assertClose(data[0].gain, 0, .0001);
    assertClose(data[0].freq, 0, .0001);
}


static void ana2()
{
    FFTDataCpx x(2);
    x.set(0, cpx(float(AudioMath::gainFromDb(0)), 0));
    x.set(1, cpx(float(AudioMath::gainFromDb(10)), 0));

    auto data = Analyzer::getFeatures(x, 3, 44100);
    assert(data.size() == 2);
    assertClose(data[0].gain, 0, .0001);
    assertClose(data[0].freq, 0, .0001);

    assertClose(data[1].gain, 10, .0001);
    assertClose(data[1].freq, 44100 / 4, .0001);
}

static void ana3()
{
    const int size = 1024;
    std::vector<float> buffer(size);
    Analyzer::generateSweep(44100., buffer.data(), size, 20, 20000);
    
    auto minMax = AudioMath::getMinMax(buffer.data(), size);
    assertClose(minMax.first, -1, .01);
    assertClose(minMax.second, 1, .01);
}


#if 1
static void ana7()
{
    const int size = 64;
    FFTDataCpx x(size);
    std::function<float(float)> unity = [](float x) {
        return x;
    };
    Analyzer::getFreqResponse(x, unity);

    for (int i = 0; i < size/2; ++i) {
        assertClose(std::abs(x.get(i)), 1, .0001);
    }
}
#endif



void testAnalyzer()
{
    ana0();
    ana1();
    ana2();
    ana3();
    ana7();
}