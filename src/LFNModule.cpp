
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

class LFNLabelUpdater
{
public:
    void update(struct LFNWidget& widget);
    void makeLabels(struct LFNWidget& widget);
private:
    LFNModule*  module;
    Label*  labels[5]={0,0,0,0,0};
    float   values[5]={111, 111, 111, 111, 111};

    
};



struct LFNWidget : ModuleWidget
{
    LFNWidget(LFNModule *);

    Label* addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
    }

     void draw(NVGcontext *vg) override
     {
         updater.update(*this);
         ModuleWidget::draw(vg);
     }

     LFNLabelUpdater updater;
     LFNModule&     module;
};

static const float knobX=20;
static const float knobY=80;
static const float knobDy = 50;

/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
LFNWidget::LFNWidget(LFNModule *module) : ModuleWidget(module), module(*module)
{
    box.size = Vec(6 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

    addOutput(Port::create<PJ301MPort>(Vec(50, 340), Port::OUTPUT, module, module->lfn.OUTPUT));
 

    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY - 1 * knobDy), module, module->lfn.FREQ_RANGE_PARAM, -5, 5, 0));
    addLabel(Vec(knobX+50,knobY - 1 * knobDy), "R");
 

    // make all the qeq filter gains
    // TODO: why do they go to 5?
    const float gmin=0, gmax=5;
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 0 * knobDy), module, module->lfn.EQ0_PARAM, gmin, gmax, gmax));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 1 * knobDy), module, module->lfn.EQ1_PARAM, gmin, gmax, gmax));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 2 * knobDy), module, module->lfn.EQ2_PARAM, gmin, gmax, gmax));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 3 * knobDy), module, module->lfn.EQ3_PARAM, gmin, gmax, gmax));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 4 * knobDy), module, module->lfn.EQ4_PARAM, gmin, gmax, gmax));

    updater.makeLabels(*this);
    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

void LFNLabelUpdater::makeLabels(LFNWidget& widget)
{
    printf("** need to make labels\n");
    for (int i=0; i<5; ++i) {
        labels[0] = widget.addLabel(Vec(knobX+40, knobY + i * knobDy), "Hz");
    }
}

void LFNLabelUpdater::update(struct LFNWidget& widget)
{
    for (int i=0; i<5; ++i) {
        auto param = widget.module.lfn.EQ0_PARAM + i;
        float val = widget.module.params[param].value;
    }
}

Model *modelLFNModule = Model::create<LFNModule,
    LFNWidget>("Squinky Labs",
    "squinkylabs-lfn",
    "LFN", EFFECT_TAG, LFO_TAG);

