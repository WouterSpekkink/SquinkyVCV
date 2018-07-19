
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#if 1

#include "EvenVCO.h"


/**
 */
struct EVModule : Module
{
public:
    EVModule();
    /**
     *
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    EvenVCO<WidgetComposite> vco;
private:
};

void EVModule::onSampleRateChange()
{
    // TODO
   // float rate = engineGetSampleRate();
    //vco.setSampleRate(rate);
}

EVModule::EVModule()
    : Module(vco.NUM_PARAMS,
    vco.NUM_INPUTS,
    vco.NUM_OUTPUTS,
    vco.NUM_LIGHTS),
    vco(this)
{
    onSampleRateChange();
   // vco.init();
}

void EVModule::step()
{
    vco.step();
}

////////////////////
// module widget
////////////////////

struct EVWidget : ModuleWidget
{
    EVWidget(EVModule *);

    void addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
    }
};


/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
EVWidget::EVWidget(EVModule *module) : ModuleWidget(module)
{
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

#if 0
    addInput(Port::create<PJ301MPort>(
        Vec(40, 200), Port::INPUT, module, module->vco.CLOCK_INPUT));
    addOutput(Port::create<PJ301MPort>(
        Vec(40, 300), Port::OUTPUT, module, module->vco.TRIGGER_OUTPUT));
 #endif
    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

Model *modelEVModule = Model::create<EVModule,
    EVWidget>("Squinky Labs",
    "squinkylabs-evco",
    "EVCO", EFFECT_TAG);

    #endif

