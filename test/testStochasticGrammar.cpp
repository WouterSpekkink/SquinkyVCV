
#include "StochasticGrammar.h"

#include <string>

static void test0()
{
    GKEY key;
    GKEY outKeys[ProductionRuleKeys::bufferSize];
    for (key = sg_first; key <= sg_last; ++key) {
        ProductionRuleKeys::breakDown(key, outKeys);
        for (GKEY* p = outKeys; *p != sg_invalid; ++p) {
            assert(*p >= sg_first);
            assert(*p <= sg_last);
        }

        std::string s = ProductionRuleKeys::toString(key);
        assert(!s.empty());
        assert(s.length() < 256);

       // printf("key: %s\n", s.c_str());

        int dur = ProductionRuleKeys::getDuration(key);
        assert(dur > 0);
        assert(dur <= PPQ * 8);

    }
}

/** to make a real grammar and test it

    need to make a big table of ProductinRule. one entry for every key.
    validate that it's valid
    then you can start generating music from it.
*/

void testStochasticGrammar()
{
    test0();
}