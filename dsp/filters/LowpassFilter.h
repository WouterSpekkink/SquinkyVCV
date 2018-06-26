#pragma once

template <typename T>
class LowpassFilterState
{

};

template <typename T>
class LowpassFilterParams
{

};

template <typename T>
class LowpassFilter
{
public:
    /**
     * fs is normalize frequency
     */
    static void setCutoff(LowpassFilterParams<T>& params, T fs);

    static T run(const LowpassFilterParams<T>& params, LowpassFilterState<T>& state, T input);
};
