
#include "asserts.h"
#include "poly.h"

static void test0()
{
    Poly<11> poly;
    float x = poly.run(0);
    assertEQ(x, 0);

    x = poly.run(1);
    assertEQ(x, 0);

    poly.setGain(0, 1);
    x = poly.run(0);
    assertEQ(x, 0);
}

static void test1()
{
    Poly<11> poly;
    poly.setGain(0, 1);
    float x = poly.run(1);
    assertNE(x, 0);
}


void testPoly()
{
    test0();
    test1();
}