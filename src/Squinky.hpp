#include "rack.hpp"

#define _GMR

using namespace rack;
extern Plugin *plugin;
extern Model *modelBootyModule;
extern Model *modelVocalModule;
extern Model *modelVocalFilterModule;
extern Model *modelColoredNoiseModule;
extern Model *modelTremoloModule;
extern Model *modelThreadBoostModule;
extern Model *modelLFNModule;
extern Model *modelCHBModule;
#ifdef _GMR
extern Model *modelGMRModule;
#endif
#ifdef _CPU_HOG
extern Model *modelCPU_HogModule;
#endif
extern Model *modelEVModule;
extern Model *modelFunVModule;




