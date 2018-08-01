/**
 * ButterworthFilterDesigner
 * a bunch of functions for generating the parameters of butterworth filters
 */

#include "ButterworthFilterDesigner.h"
#include "DSPFilters/Dsp.h"
#include "BiquadFilter.h"
#include <memory>

/* Porting to new lib:
 *      1) do we need to use SimpleFilter, or can we use something more low level?
 *      2) can you specify freq without a sample rate?
 */

template <typename T>
void ButterworthFilterDesigner<T>::designSixPoleLowpass(BiquadParams<T, 3>& outParams, T frequency)
{
    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Butterworth::LowPass <6>, 0> lp6;

    lp6.setup(6,    // order
        44100,// sample rate
        44100 * frequency // center frequency
        );

    Dsp::Cascade::Storage cascadeStorage = lp6.getCascadeStorage();
    assert(cascadeStorage.maxStages == 3);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);
}



template <typename T>
void ButterworthFilterDesigner<T>::designFivePoleLowpass(BiquadParams<T, 3>& outParams, T frequency)
{
#if 0
    using Filter = Dsp::ButterLowPass<5, 1>;
    std::unique_ptr<Filter> lp5(new Filter());      // std::make_unique is not until C++14
    lp5->SetupAs(frequency);
    assert(lp5->GetStageCount() == 3);
    BiquadFilter<T>::fillFromStages(outParams, lp5->Stages(), lp5->GetStageCount());
#endif
}

template <typename T>
void ButterworthFilterDesigner<T>::designThreePoleLowpass(BiquadParams<T, 2>& outParams, T frequency)
{
    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Butterworth::LowPass <3>, 0> lp3;

    lp3.setup(3,    // order
        44100,// sample rate
        44100 * frequency // center frequency
    );

    Dsp::Cascade::Storage cascadeStorage = lp3.getCascadeStorage();
    assert(cascadeStorage.maxStages == 2);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);
}

template <typename T>
void ButterworthFilterDesigner<T>::designFourPoleLowpass(BiquadParams<T, 2>& outParams, T frequency)
{
    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Butterworth::LowPass <4>, 0> lp4;

    lp4.setup(4,    // order
        44100,// sample rate
        44100 * frequency // center frequency
    );

    Dsp::Cascade::Storage cascadeStorage = lp4.getCascadeStorage();
    assert(cascadeStorage.maxStages == 2);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);
}

template <typename T>
void ButterworthFilterDesigner<T>::designTwoPoleLowpass(BiquadParams<T, 1>& outParams, T frequency)
{
    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Butterworth::LowPass <2>, 0> lp2;

    lp2.setup(2,    // order
        44100,// sample rate
        44100 * frequency // center frequency
    );

    Dsp::Cascade::Storage cascadeStorage = lp2.getCascadeStorage();
    assert(cascadeStorage.maxStages == 1);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);
}

template <typename T>
void ButterworthFilterDesigner<T>::designSixPoleElliptic(BiquadParams<T, 3>& outParams, T frequency, T rippleDb, T rolloff)
{
    assert(rippleDb > 0);

    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Elliptic::LowPass <6>, 0> el;
    /*
    void setup(int order,
        double sampleRate,
        double cutoffFrequency,
        double rippleDb,
        double rolloff);
        */
    el.setup(6, 44100, 44100 * frequency, rippleDb, rolloff);

    Dsp::Cascade::Storage cascadeStorage = el.getCascadeStorage();
    assert(cascadeStorage.maxStages == 3);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);
}

template <typename T>
void ButterworthFilterDesigner<T>::designEightPoleElliptic(BiquadParams<T, 4>& outParams, T frequency, T rippleDb, T rolloff)
{
    assert(rippleDb > 0);

    assert(frequency > 0 && frequency < .5);
    Dsp::SimpleFilter <Dsp::Elliptic::LowPass <8>, 0> el;
    /*
    void setup(int order,
    double sampleRate,
    double cutoffFrequency,
    double rippleDb,
    double rolloff);
    */
    el.setup(8, 44100, 44100 * frequency, rippleDb, rolloff);

    Dsp::Cascade::Storage cascadeStorage = el.getCascadeStorage();
    assert(cascadeStorage.maxStages == 4);
    BiquadFilter<T>::fillFromStages(outParams, cascadeStorage.stageArray, cascadeStorage.maxStages);

}

// Explicit instantiation, so we can put implementation into .cpp file
// TODO: option to take out float version (if we don't need it)
// Or put all in header
template class ButterworthFilterDesigner<double>;
template class ButterworthFilterDesigner<float>;

