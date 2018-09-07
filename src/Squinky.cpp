// plugin main
#include "Squinky.hpp"


// The plugin-wide instance of the Plugin class
Plugin *plugin;

/**
 * Here we register the whole plugin, which may have more than one module in it.
 */
void init(rack::Plugin *p)
{
    plugin = p;
    p->slug = "squinkylabs-plug1";
    p->version = TOSTRING(VERSION);

    p->addModel(modelBootyModule);
#ifdef _CHB
    p->addModel(modelCHBModule);
#endif
    p->addModel(modelTremoloModule);
    p->addModel(modelColoredNoiseModule);
    p->addModel(modelVocalFilterModule);
    p->addModel(modelFunVModule);
    p->addModel(modelVocalModule);

  
    p->addModel(modelLFNModule); 
    p->addModel(modelThreadBoostModule);


#ifdef _GMR
    p->addModel(modelGMRModule);
#endif
#ifdef _CPU_HOG
    assert(modelCPU_HogModule);
    p->addModel(modelCPU_HogModule);
#endif
#ifdef _EV
    p->addModel(modelEVModule);
#endif
#ifdef _EV3
    p->addModel(modelEV3Module);
#endif
}