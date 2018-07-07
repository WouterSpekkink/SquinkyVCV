
#include "StochasticGrammar.h"


float Random::get()
{
    assert(false);
    return 0;
}


/***************************************************************************************************************
 *
 * ProductionRuleKeys
 *
 ***************************************************************************************************************/

void ProductionRuleKeys::breakDown(GKEY key, GKEY * outKeys)
{
    switch (key) {
        // terminal keys expand to themselves
        case sg_w2:
        case sg_w:
        case sg_h:
        case sg_q:
        case sg_e:
        case sg_e3:
        case sg_sx:
        case sg_68:
        case sg_78:
        case sg_98:
        case sg_dq:
        case sg_dh:
        case sg_de:
            //case sg_hdq:
            //case sg_qhe:
            *outKeys++ = key;
            *outKeys++ = sg_invalid;
            break;
        case sg_798:
            *outKeys++ = sg_78;
            *outKeys++ = sg_98;
            *outKeys++ = sg_invalid;
            break;
        case sg_ww:
            *outKeys++ = sg_w;
            *outKeys++ = sg_w;
            *outKeys++ = sg_invalid;
            break;
        case sg_hh:
            *outKeys++ = sg_h;
            *outKeys++ = sg_h;
            *outKeys++ = sg_invalid;
            break;
        case sg_qq:
            *outKeys++ = sg_q;
            *outKeys++ = sg_q;
            *outKeys++ = sg_invalid;
            break;
        case sg_sxsx:
            *outKeys++ = sg_sx;
            *outKeys++ = sg_sx;
            *outKeys++ = sg_invalid;
            break;
        case sg_ee:
            *outKeys++ = sg_e;
            *outKeys++ = sg_e;
            *outKeys++ = sg_invalid;
            break;
        case sg_e3e3e3:
            *outKeys++ = sg_e3;
            *outKeys++ = sg_e3;
            *outKeys++ = sg_e3;
            *outKeys++ = sg_invalid;
            break;
        case sg_hdq:
            *outKeys++ = sg_h;
            *outKeys++ = sg_dq;
            *outKeys++ = sg_invalid;
            break;
        case sg_hq:
            *outKeys++ = sg_h;
            *outKeys++ = sg_q;
            *outKeys++ = sg_invalid;
            break;
        case sg_qh:
            *outKeys++ = sg_q;
            *outKeys++ = sg_h;
            *outKeys++ = sg_invalid;
            break;

        case sg_qhe:
            *outKeys++ = sg_q;
            *outKeys++ = sg_h;
            *outKeys++ = sg_e;
            *outKeys++ = sg_invalid;
            break;
        case sg_q78:
            *outKeys++ = sg_q;
            *outKeys++ = sg_78;
            *outKeys++ = sg_invalid;
            break;
        case sg_qe68:
            *outKeys++ = sg_q;
            *outKeys++ = sg_e;
            *outKeys++ = sg_68;
            *outKeys++ = sg_invalid;
            break;
        default:
            assert(false);
    }
}


const char * ProductionRuleKeys::toString(GKEY key)
{
    const char * ret;
    switch (key) {
        case sg_w2: ret = "2xw"; break;
        case sg_ww: ret = "w,w"; break;
        case sg_w: ret = "w"; break;
        case sg_h: ret = "h"; break;
        case sg_hh: ret = "h,h"; break;
        case sg_q: ret = "q"; break;
        case sg_qq: ret = "q,q"; break;
        case sg_e: ret = "e"; break;
        case sg_ee: ret = "e,e"; break;

        case sg_e3e3e3: ret = "3e,3e,3e"; break;
        case sg_e3: ret = "3e"; break;

        case sg_sx: ret = "sx"; break;
        case sg_sxsx: ret = "sx, sx"; break;
        case sg_68: ret = "<6/8>"; break;
        case sg_78: ret = "<7/8>"; break;
        case sg_98: ret = "<9/8>"; break;
        case sg_798: ret = "7+9/8"; break;

        case sg_dq: ret = "q."; break;
        case sg_dh: ret = "h."; break;
        case sg_de: ret = "e."; break;

        case sg_hdq: ret = "h,q."; break;
        case sg_qhe: ret = "q,h,e"; break;
        case sg_qh: ret = "q,h"; break;
        case sg_hq: ret = "h,q"; break;
        case sg_q78: ret = "q,<7/8>"; break;
        case sg_qe68: ret = "q,e,<6/8>"; break;


        default:
            printf("can't print key %d\n", key);
            assert(false);
            ret = "error";
    }
    return ret;
}

int ProductionRuleKeys::getDuration(GKEY key)
{
    int ret;

    assert((PPQ % 3) == 0);
    switch (key) {
        case sg_798:
        case sg_w2: ret = 2 * 4 * PPQ; 	break;
        case sg_ww: ret = 2 * 4 * PPQ; 	break;
        case sg_w: ret = 4 * PPQ; 	break;
        case sg_h: ret = 2 * PPQ; 	break;
        case sg_hh: ret = 2 * 2 * PPQ; 	break;
        case sg_q: ret = 1 * PPQ; 	break;
        case sg_qq: ret = 2 * PPQ; 	break;
        case sg_e:
            assert((PPQ % 2) == 0);
            ret = PPQ / 2;
            break;
        case sg_ee: ret = PPQ; 	break;
        case sg_sxsx: ret = PPQ / 2; break;
        case sg_sx:
            assert((PPQ % 4) == 0);
            ret = PPQ / 4;
            break;
        case sg_e3e3e3: ret = PPQ; break;
        case sg_e3:
            assert(PPQ % 3 == 0);
            ret = PPQ / 3;
            break;
        case sg_68: ret = 6 * (PPQ / 2); break;
        case sg_78: ret = 7 * (PPQ / 2); break;

        case sg_q78:
        case sg_qe68:
        case sg_98:
            ret = 9 * (PPQ / 2); break;

        case sg_dq: ret = 3 * PPQ / 2; break;
        case sg_dh: ret = 3 * PPQ; break;
        case sg_de: ret = 3 * PPQ / 4; break;

        case sg_hdq: ret = 2 * PPQ + 3 * PPQ / 2; break;
        case sg_qhe: ret = PPQ * 3 + PPQ / 2; break;



        case sg_hq:
        case sg_qh: ret = PPQ * 3; break;


        default:
#ifdef _MSC_VER
            printf("can't get dur key %d\n", key);
#endif
            assert(false);
            ret = 0;
    }
    return ret;

}


/***************************************************************************************************************
*
* ProductionRule
*
***************************************************************************************************************/


// generate production, return code for what happened
int ProductionRule::_evaluateRule(const ProductionRule& rule, float random)
{
    assert(random >= 0 && random <= 1);
    assert(false);   // the rest is for 1..256
                     //int rand = r.get() & 0xff;
    int rand = (int) (random * 256);
    //printf("evaluateRule called with rand is %d\n", rand);

    int i = 0;
    for (bool done2 = false; !done2; ++i) {
        assert(i < numEntries);
        //printf("prob[%d] is %d\n", i,  rule.entries[i].probability);
        if (rule.entries[i].probability >= rand) {
            GKEY code = rule.entries[i].code;
            //printf("rule fired on code abs val=%d\n", code);
            return code;
        }
    }
    assert(false);	// no rule fired
    return 0;
}

void ProductionRule::evaluate(EvaluationState& es, int ruleToEval)
{
    //printf("\n evaluate called on rule #%d\n", ruleToEval);
    const ProductionRule& rule = es.rules[ruleToEval];

#ifdef _MSC_VER
    assert(rule._isValid(ruleToEval));
#endif
    GKEY result = _evaluateRule(rule, es.r.get());
    if (result == sg_invalid)		// request to terminate recursion
    {
        GKEY code = ruleToEval;		// our "real" terminal code is our table index
                                    //printf("production rule #%d terminated\n", ruleToEval);
                                    //printf("rule terminated! execute code %s\n",  ProductionRuleKeys::toString(code));
        es.writeSymbol(code);
    } else {
        //printf("production rule #%d expanded to %d\n", ruleToEval, result);

        // need to expand,then eval all of the expanded codes

        GKEY buffer[ProductionRuleKeys::bufferSize];
        ProductionRuleKeys::breakDown(result, buffer);
        for (GKEY * p = buffer; *p != sg_invalid; ++p) {
            //printf("expanding rule #%d with %d\n", ruleToEval, *p);
            evaluate(es, *p);
        }
        //printf("done expanding %d\n", ruleToEval);
    }
}

// is the data self consistent, and appropriate for index
#ifdef _MSC_VER
bool ProductionRule::_isValid(int index) const
{
    if (index == sg_invalid) {
        printf("rule not allowed in first slot\n");
        return false;
    }


    if (entries[0] == ProductionRuleEntry()) {
        printf("rule at index %d is ininitizlied. bad graph (%s)\n",
            index,
            ProductionRuleKeys::toString(index));
        return false;
    }

    float last = -1;
    bool foundTerminator = false;
    for (int i = 0; !foundTerminator; ++i) {
        if (i >= numEntries) {
            printf("entries not terminated index=%d 'i' is too big: %d\n", index, i);
            return false;
        }
        const ProductionRuleEntry& e = entries[i];
        if (e.probability <= last)			// probabilities grow
        {
            printf("probability not growing is %f was %f\n", e.probability, last);
            return false;
        }
        if (e.probability == 1.0f) {
            foundTerminator = true;					// must have a 255 to end it
            if (e.code == index) {
                printf("rule terminates on self: recursion not allowed\n");
                return false;
            }
        }

        if (e.code < sg_invalid || e.code > sg_last) {
            printf("rule[%d] entry[%d] had invalid code: %d\n", index, i, e.code);
            return false;
        }

        // if we are terminating recursion, then by definition our duration is correct
        if (e.code != sg_invalid) {
            // otherwise, make sure the entry has the right duration
            int entryDuration = ProductionRuleKeys::getDuration(e.code);
            int ruleDuration = ProductionRuleKeys::getDuration(index);
            if (entryDuration != ruleDuration) {
                printf("production rule[%d] (name %s) duration mismatch (time not conserved) rule dur = %d entry dur %d\n",
                    index, ProductionRuleKeys::toString(index), ruleDuration, entryDuration);
                return false;
            }
        }
        last = e.probability;
    }
    return true;
}
#endif

#ifdef _MSC_VER
bool ProductionRule::isGrammarValid(const ProductionRule * rules, int numRules, GKEY firstRule)
{
    //printf("is grammar valid, numRules = %d first = %d\n", numRules, firstRule);
    if (firstRule < sg_first) {
        printf("first rule index (%d) bad\n", firstRule);
        return false;
    }
    if (numRules != (sg_last + 1)) {
        printf("bad number of rules\n");
        return false;
    }

    const ProductionRule& r = rules[firstRule];

    if (!r._isValid(firstRule)) {
        return false;
    }

    // now, make sure every entry goes to something real
    bool foundTerminator = false;
    for (int i = 0; !foundTerminator; ++i) {
        const ProductionRuleEntry& e = r.entries[i];
        if (e.probability == 0xff)
            foundTerminator = true;					// must have a 255 to end it	
        GKEY _newKey = e.code;
        if (_newKey != sg_invalid) {
            GKEY outKeys[4];
            ProductionRuleKeys::breakDown(_newKey, outKeys);
            for (GKEY * p = outKeys; *p != sg_invalid; ++p) {
                if (!isGrammarValid(rules, numRules, *p)) {
                    printf("followed rules to bad one\n");
                    return false;
                }
            }
        }
    }

    return true;
}
#endif

