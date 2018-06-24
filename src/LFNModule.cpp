
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#include "LFN.h"


/**
 */
struct LFNModule : Module
{
public:
    LFNModule();
    /**
     * 
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    LFN<WidgetComposite> lfn;
private:
};

void LFNModule::onSampleRateChange()
{
    float rate = engineGetSampleRate();
    lfn.setSampleRate(rate);
}

LFNModule::LFNModule()
    : Module(lfn.NUM_PARAMS,
    lfn.NUM_INPUTS,
    lfn.NUM_OUTPUTS,
    lfn.NUM_LIGHTS),
    lfn(this)
{
    onSampleRateChange();
    lfn.init();
}

void LFNModule::step()
{
    lfn.step();
}

////////////////////
// module widget
////////////////////

struct LFNWidget : ModuleWidget
{
    LFNWidget(LFNModule *);

    void addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
    }
    //void addClockSection(TremoloModule *module);
    //void addIOSection(TremoloModule *module);
    //void addMainSection(TremoloModule *module);
};


/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
LFNWidget::LFNWidget(LFNModule *module) : ModuleWidget(module)
{
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

  
    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

Model *modelLFNModule = Model::create<LFNModule,
    LFNWidget>("Squinky Labs",
    "squinkylabs-lfn",
    "LFN", EFFECT_TAG, LFO_TAG);

