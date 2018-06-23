
#include <assert.h>

#include "Analyzer.h"
#include "FFTData.h"

static void ana0()
{
    FFTDataCpx f(16);
    Analyzer::printf(f);
}

void testFilters()
{
    ana0();

}