
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#include "GMR.h"


/**
 */
struct GMRModule : Module
{
public:
    GMRModule();
    /**
     * 
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    GMR<WidgetComposite> GMR;
private:
};

void GMRModule::onSampleRateChange()
{
    float rate = engineGetSampleRate();
    GMR.setSampleRate(rate);
}

GMRModule::GMRModule()
    : Module(GMR.NUM_PARAMS,
    GMR.NUM_INPUTS,
    GMR.NUM_OUTPUTS,
    GMR.NUM_LIGHTS),
    GMR(this)
{
    onSampleRateChange();
    GMR.init();
}

void GMRModule::step()
{
    GMR.step();
}

////////////////////
// module widget
////////////////////

struct GMRWidget : ModuleWidget
{
    GMRWidget(GMRModule *);

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
GMRWidget::GMRWidget(GMRModule *module) : ModuleWidget(module)
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

Model *modelGMRModule = Model::create<GMRModule,
    GMRWidget>("Squinky Labs",
    "squinkylabs-GMR",
    "GMR", EFFECT_TAG, LFO_TAG);

