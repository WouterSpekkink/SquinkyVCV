
#include "StochasticGrammar.h"
#include "TriggerSequencer.h"

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

// test all the gkeys
void testAllKeys()
{
    const int siz = ProductionRuleKeys::bufferSize;
    GKEY buffer[siz];

    for (GKEY gk = sg_first; gk <= sg_last; ++gk) {
        printf("testing key %d\n", gk);
        printf("to string: %s\n", ProductionRuleKeys::toString(gk));
        const int dur = ProductionRuleKeys::getDuration(gk);
        ProductionRuleKeys::breakDown(gk, buffer);
        int sum = 0;
        for (GKEY * p = buffer; *p != sg_invalid; ++p) {
            printf("adding to sum %d\n", ProductionRuleKeys::getDuration(*p));
            sum += ProductionRuleKeys::getDuration(*p);

        }
        printf("dur = %d sum = %d (should be the same)\n", dur, sum);
        assert(dur == sum);
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


/*********************************************************************************************************
 * TriggerSequencer
 **********************************************************************************************************/


 // test event at zero fires at zero
static void ts0()
{
    printf("ts0b\n");
    TriggerSequencer::Event seq[] =
    {
        {TriggerSequencer::TRIGGER, 0},
    {TriggerSequencer::END, 100}
    };
    TriggerSequencer ts(seq);

    ts.clock();
    assert(ts.getTrigger());

    ts.clock();
    assert(!ts.getTrigger());

}

// test trigger at 1 happens at 1
static void ts1()
{
    printf("ts1\n");
    TriggerSequencer::Event seq[] =
    {
        {TriggerSequencer::TRIGGER, 1},
    {TriggerSequencer::END, 0}
    };
    TriggerSequencer ts(seq);

    ts.clock();
    assert(!ts.getTrigger());

    ts.clock();
    assert(ts.getTrigger());

    ts.clock();
    assert(!ts.getTrigger());

    ts.clock();
    assert(!ts.getTrigger());
}


// 4 clock loop: delay 4, trigger, end
static void ts2()
{
    printf("ts2\n");
    TriggerSequencer::Event seq[] =
    {
        {TriggerSequencer::TRIGGER, 4},
    {TriggerSequencer::END, 0}
    };
    TriggerSequencer ts(seq);

    bool firstTime = true;
    // first time through, 4 clocks of nothing. then clock, 0,0,0
    for (int i = 0; i< 4; ++i) {
        printf("--- loop ----\n");

        ts.clock();
        if (firstTime) {
            assert(!ts.getTrigger()); assert(!ts.getEnd());
            firstTime = false;
        } else {
            printf("second time around, t=%d e=%d\n", ts.getTrigger(), ts.getEnd());
            // second time around we finally see the trigger

            assert(ts.getTrigger());


            // second time around, need to clock the end of the last time
            assert(ts.getEnd());
            ts.reset(seq);				// start it up again

            printf("e\n");
            assert(!ts.getTrigger());	// resetting should not set us up for a trigger
        }
        ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
        ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());


        ts.clock(); assert(!ts.getTrigger());
        //	assert(ts.getEnd());

        //	ts.reset(seq);
    }
}

// test trigger seq qith
// 4 clock loop: trigger, delay 4 end
static void ts3()
{
    printf("ts3\n");
    TriggerSequencer::Event seq[] =
    {
        {TriggerSequencer::TRIGGER, 0},
    {TriggerSequencer::END, 4}
    };
    TriggerSequencer ts(seq);


    bool firstLoop = true;
    for (int i = 0; i< 4; ++i) {
        printf("--- loop ----\n");

        // 1

        ts.clock();
        if (firstLoop) {
            assert(ts.getTrigger());
            // we just primed loop at top, so it's got a ways
            assert(!ts.getEnd());
            firstLoop = false;
        } else {
            // second time around, need to clock the end of the last time
            assert(ts.getEnd());
            ts.reset(seq);				// start it up again

            printf("e\n");
            assert(ts.getTrigger());	// resetting should have set us up for a trigger
        }
        // 2
        ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
        // 3
        ts.clock(); assert(!ts.getTrigger()); assert(!ts.getEnd());
        // 4
        ts.clock();
        assert(!ts.getTrigger());
        assert(!ts.getEnd());
    }
}

// test trigger seq with straight ahead 4/4 as generated by a grammar
static void ts4()
{
    printf("ts4\n");

    TriggerSequencer::Event seq[] =
    {
        {TriggerSequencer::TRIGGER, 0},
    {TriggerSequencer::TRIGGER, 4},
    {TriggerSequencer::TRIGGER, 4},
    {TriggerSequencer::TRIGGER, 4},
    {TriggerSequencer::END, 4}
    };
    TriggerSequencer ts(seq);


    //bool firstLoop = true;
    for (int i = 0; i< 100; ++i) {
        bool firstTime = (i == 0);
        // repeating pattern of trigg, no, no, no
        for (int j = 0; j<4; ++j) {
            for (int k = 0; k<4; ++k) {
                //	printf("test loop, i=%d, j=%d, k=%d\n", i, j, k);
                ts.clock();

                bool expectEnd = (k == 0) && (j == 0) && !firstTime;
                assert(ts.getEnd() == expectEnd);
                if (ts.getEnd()) {
                    ts.reset(seq);
                }
                assert(ts.getTrigger() == (k == 0));
            }
        }
    }
}
/********************************************************************************************

**********************************************************************************************/



void testStochasticGrammar()
{
    test0();
    testAllKeys();
    testGrammarSub(init0);
    testGrammarSub(init1);
    testGrammarSub(init2);
    ts0();
    ts1();
    ts2();
    ts3();
    ts4();
}