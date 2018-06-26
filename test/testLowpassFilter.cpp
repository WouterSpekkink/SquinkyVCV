
#include "LowpassFilter.h"

template<typename T>
static void test0()
{
    LowpassFilterState<T> state;
    LowpassFilterParams<T> params;
    LowpassFilter<T>::setCutoff(params, T(.1));
    LowpassFilter<T>::run(params, state, 0);
}

template<typename T>
void _testLowpassFilter()
{
    test0<T>();
}

void testLowpassFilter()
{
    _testLowpassFilter<float>();
    _testLowpassFilter<double>();
}