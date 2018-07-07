
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


/**
 * simplest possible grammar.
 */
static GKEY init0()
{
    printf("called init0\n");
    // This rule always generate sg-w2 (two whole notes tied together)
    ProductionRule& r = rules[sg_w2];

    r.entries[0].probability = 1;
    r.entries[0].code = sg_invalid;		// terminate expansion		


    return sg_w2;
}

/**
 * Simple grammar with a rule but no random.
 */
static GKEY init1()
{

    {
        // start with w2 duration
        ProductionRule& r = rules[sg_w2];

        // break into w,w prob 100
        r.entries[0].probability = 1.0f;
        r.entries[0].code = sg_ww;
    }

    {
        // now need rule for w hole
        printf("in init1 making 100 for %d\n", sg_w);
        ProductionRule& r = rules[sg_w];
        r.entries[0].probability = 1.0f;
        r.entries[1].code = sg_invalid;
    }
    printf("leave init 1. rule 1 p0 = %f\n", rules[sg_w2].entries[0].probability);
    return sg_w2;
}


/**
 * Simple grammar with randomness initializer
 */
static GKEY init2()
{
    printf("in init2 making 50/50 for %d\n", sg_w2);
    {
        // start with w2 duration
        ProductionRule& r = rules[sg_w2];

        // break into w,w prob 50

        r.entries[0].probability = .5f;
        r.entries[0].code = sg_ww;
        r.entries[1].probability = 1.0f;
        r.entries[1].code = sg_invalid;		// always terminate
    }

    {
        // now need rule for w hole
        printf("in init1 making 100 for %d\n", sg_w);
        ProductionRule& r = rules[sg_w];
        r.entries[1].probability = 1.0f;
        r.entries[1].code = sg_invalid;		// always terminate
    }
    printf("leave init 1. rule 1 p0 = %f\n", rules[sg_w2].entries[0].probability);

    return sg_w2;
}


static void testGrammarSub(INITFN f)
{
    GKEY init = f();


    bool b = ProductionRule::isGrammarValid(rules, numRules, init);
    assert(b);

    printf("test sub finished validating grammar\n");

    Random r;
    TestEvaluator es(r);
    es.rules = rules;
    es.numRules = numRules;
    ProductionRule::evaluate(es, init);

    assert(es.getNumSymbols() > 0);
}

void testStochasticGrammar()
{
    test0();
    testGrammarSub(init0);
    testGrammarSub(init1);
    testGrammarSub(init2);
}