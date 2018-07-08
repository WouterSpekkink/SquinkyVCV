#pragma once
template <int order>
class Poly
{
public:
    Poly();
    float run(float x)
    {
        fillPowers(x);
        return doSum();
    }
    void setGain(int index, float value)
    {
        assert(index >= 0 && index < order);
        gains[index] = value;
    }
private:
    float gains[order];

    // powers[0] = x, powers[1] = x**2
    float powers[order];

    void fillPowers(float);
    float doSum();
};

template <int order>
inline Poly<order>::Poly()
{
    assert(order == 11);
    for (int i = 0; i < order; ++i) {
        gains[i] = 0;
        powers[i] = 0;
    }
}

template <int order>
inline void Poly<order>::fillPowers(float x)
{
    float value = x;
    for (int i = 0; i < order; ++i) {
        powers[i] = value;
        value *= x;
    }
}



template <int order>
inline float Poly<order>::doSum()
{
    float ret = gains[0] * powers[0];
        ret += gains[1] * (2 * powers[1] - 1);
        ret += gains[2] * (4 * powers[2] - 3 * powers[0]);
        ret += gains[3] * (8 * powers[3] - 8 * powers[1] + 1);
        ret += gains[4] * (16 * powers[4] - 20 * powers[2] + 5 * powers[0]);
        ret += gains[5] * (32 * powers[5] - 48 * powers[3] + 18 * powers[1] - 1);
        ret += gains[6] * (64 * powers[6] - 112 * powers[4] + 56 * powers[2] - 7 * powers[0]);
        ret += gains[7] * (128 * powers[7] - 256 * powers[5] + 160 * powers[3] - 32 * powers[1] + 1);
        ret += gains[2];
        ret += gains[2];
        ret += gains[2];
        return ret;
}