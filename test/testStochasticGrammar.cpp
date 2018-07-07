
#include "StochasticGrammar.h"

#include <string>
#include <vector>


// Test basic integrity of key data
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

/**************************************************************************************
 * Make some simple grammars and test them
 **************************************************************************************/


static const int numRules = fullRuleTableSize;
static Random r;


typedef GKEY(*INITFN)();

static ProductionRule rules[numRules];


class TestEvaluator : public ProductionRule::EvaluationState
{
public:
    TestEvaluator(Random& xr) : ProductionRule::EvaluationState(xr)
    {
    }
    virtual void writeSymbol(GKEY key)
    {
        keys.push_back(key);
    }

    int getNumSymbols()
    {
        printf("final keys: ");
        for (size_t i = 0; i< keys.size(); ++i) printf("%s, ", ProductionRuleKeys::toString(keys[i]));
        printf("\n");
        return (int) keys.size();
    }
private:
    std::vector<GKEY> keys;
};


// simplest possible grammar.

static GKEY init0()
{
    printf("called init0\n");
    // This rule always generate sg-w2 (two whole notes tied together)
    ProductionRule& r = rules[sg_w2];

    r.entries[0].probability = 1;
    r.entries[0].code = sg_invalid;		// terminate expansion		


    return sg_w2;
}

INITFN x = init0;
static void testSub(INITFN f)
{
    GKEY init = f();


    bool b = ProductionRule::isGrammarValid(rules, numRules, init);
    assert(b);

    printf("test sub finisjed validating grammar\n");

    Random r;
    TestEvaluator es(r);
    es.rules = rules;
    es.numRules = numRules;
    ProductionRule::evaluate(es, init);

    assert(es.getNumSymbols() > 0);
}



void gt0()
{
    printf("gt0\n");
    testSub(init0);
}


void testStochasticGrammar()
{
    test0();
    gt0();
}