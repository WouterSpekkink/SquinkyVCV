
#include "StochasticGrammar.h"

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
