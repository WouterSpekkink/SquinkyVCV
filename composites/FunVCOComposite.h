#pragma once


#include "FunVCO.h"

template <class TBase>
class FunVCOComposite : public TBase
{
public:
    void step() override;

private:
    VoltageControlledOscillator<16,16> vco;
};

template <class TBase>
inline void FunVCOComposite<TBase>::step()
{

}