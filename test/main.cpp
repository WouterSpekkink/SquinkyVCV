/**
  * Unit test entry point
  */

#include <stdio.h>
#include <assert.h>
#include <string>

extern void testBiquad();
extern void testTestSignal();
extern void testSaw();
extern void testLookupTable();
extern void testSinOscillator();
extern void testHilbert();
extern void testAudioMath();
extern void perfTest();
extern void testFrequencyShifter();
extern void testStateVariable();
extern void testVocalAnimator();
extern void testObjectCache();
extern void testThread();
extern void testFFT();
extern void testRingBuffer();
extern void testManagedPool();
extern void testColoredNoise();
extern void testFFTCrossFader();
extern void testFinalLeaks();

int main(int argc, char ** argv)
{
    printf("in test main\n");
    bool runPerf = false;
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "--perf") {
            runPerf = true;
        }
    }
#ifdef _PERF
    runPerf = true;
#ifndef NDEBUG
#error asserts should be off for perf test
#endif

#endif
    // While this code may work in 32 bit applications, it's not tested for that.
    // Want to be sure we are testing the case we care about.
    assert(sizeof(size_t) == 8);

    testAudioMath();
    testRingBuffer();
    testManagedPool();
    testLookupTable();
    testObjectCache();

 printf("in test main 2\n");

    testTestSignal();
    testBiquad();
    testSaw();
    
    testSinOscillator();
    testHilbert();
    testStateVariable();
 printf("in test main 3\n");

    testFFT();
    testFFTCrossFader();
    printf("in test main 4\n"); 
   
    testThread();
    
     
 printf("in test main 5\n");

    // after testing all the components, test composites.
    // this test make test.exe freeze, even if it's not run!
    testColoredNoise();

    testFrequencyShifter();
    testVocalAnimator();

    if (runPerf) {
        perfTest();
    }


    testFinalLeaks();

    // When we run inside Visual Studio, don't exit debugger immediately
#if defined(_MSC_VER)
    printf("Test passed. Press any key to continue...\n"); fflush(stdout);
    getchar();
#else
    printf("Tests passed.\n");
#endif
}