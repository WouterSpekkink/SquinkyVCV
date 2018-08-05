
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#if 1
#include "FunVCOComposite.h"


/**
 */
struct FunVModule : Module
{
public:
    FunVModule();
    /**
     *
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    FunVCOComposite<WidgetComposite> vco;
private:
};

void FunVModule::onSampleRateChange()
{
    float rate = engineGetSampleRate();
    vco.setSampleRate(rate);
}

FunVModule::FunVModule()
    : Module(vco.NUM_PARAMS,
    vco.NUM_INPUTS,
    vco.NUM_OUTPUTS,
    vco.NUM_LIGHTS),
    vco(this)
{
    onSampleRateChange();
    //gmr.init();
}

void FunVModule::step()
{
    vco.step();
}

////////////////////
// module widget
////////////////////

struct FunVWidget : ModuleWidget
{
    FunVWidget(FunVModule *);

    void addTop3(FunVModule *, float verticalShift);
    void addMiddle4(FunVModule *, float verticalShift);
    void addJacks(FunVModule *, float verticalShift);

    void addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
    }
};

void FunVWidget::addTop3(FunVModule * module, float verticalShift)
{
    const float left = 8;
    const float right = 112;
    const float center = 49;

    addParam(ParamWidget::create<NKK>(Vec(left, 66 + verticalShift),
        module, module->vco.MODE_PARAM, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(left -6, 48+ verticalShift), "anlg");
    addLabel(Vec(left -6, 102+ verticalShift), "dgtl");

    addParam(ParamWidget::create<Rogan3PSBlue>(Vec(center, 61 + verticalShift),
        module, module->vco.FREQ_PARAM, -54.0f, 54.0f, 0.0f));
    addLabel(Vec(center +6, 40+ verticalShift), "pitch");

    addParam(ParamWidget::create<NKK>(Vec(right, 66 + verticalShift),
        module, module->vco.SYNC_PARAM, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(right-6, 48+ verticalShift), "hard");
    addLabel(Vec(right-6, 102+ verticalShift), "soft");
}

void FunVWidget::addMiddle4(FunVModule * module, float verticalShift)
{
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(23, 143 + verticalShift),
        module, module->vco.FINE_PARAM, -1.0f, 1.0f, 0.0f));
    addLabel(Vec(23, 126 +verticalShift), "fine");

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(91, 143 + verticalShift),
        module, module->vco.PW_PARAM, 0.0f, 1.0f, 0.5f));
    addLabel(Vec(91, 126 +verticalShift), "p width");

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(23, 208 + verticalShift),
        module, module->vco.FM_PARAM, 0.0f, 1.0f, 0.0f));
    addLabel(Vec(23, 188 +verticalShift), "fm cv");

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(91, 208 + verticalShift),
        module, module->vco.PWM_PARAM, 0.0f, 1.0f, 0.0f));
    addLabel(Vec(91, 188 +verticalShift), "pwm cv");

}

void FunVWidget::addJacks(FunVModule * module, float verticalShift)
{
    addInput(Port::create<PJ301MPort>(Vec(11, 273+verticalShift), Port::INPUT, module, module->vco.PITCH_INPUT));
    addLabel(Vec(7, 255+verticalShift), "v/8");

    addInput(Port::create<PJ301MPort>(Vec(45, 273+verticalShift), Port::INPUT, module, module->vco.FM_INPUT));
    addLabel(Vec(44, 255+verticalShift), "fm");

    addInput(Port::create<PJ301MPort>(Vec(80, 273+verticalShift), Port::INPUT, module, module->vco.SYNC_INPUT));
    addLabel(Vec(70, 255+verticalShift), "sync");

    addInput(Port::create<PJ301MPort>(Vec(114, 273+verticalShift), Port::INPUT, module, module->vco.PW_INPUT));
    addLabel(Vec(104, 255+verticalShift), "pwm");

    addOutput(Port::create<PJ301MPort>(Vec(11, 317+verticalShift), Port::OUTPUT, module, module->vco.SIN_OUTPUT));
    addLabel(Vec(7, 298+verticalShift), "sin");

    addOutput(Port::create<PJ301MPort>(Vec(45, 317+verticalShift), Port::OUTPUT, module, module->vco.TRI_OUTPUT));
    addLabel(Vec(40, 298+verticalShift), "tri");

    addOutput(Port::create<PJ301MPort>(Vec(80, 317+verticalShift), Port::OUTPUT, module, module->vco.SAW_OUTPUT));
    addLabel(Vec(75, 298+verticalShift), "saw");

    addOutput(Port::create<PJ301MPort>(Vec(114, 317+verticalShift), Port::OUTPUT, module, module->vco.SQR_OUTPUT));
    addLabel(Vec(109, 298+verticalShift), "sqr");
}
/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
FunVWidget::FunVWidget(FunVModule *module) : ModuleWidget(module)
{
    box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

    addTop3(module, 0);
    addMiddle4(module, 0);
    addJacks(module, 0);

    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

    //addChild(ModuleLightWidget::create<SmallLight<GreenRedLight>>(Vec(99, 42.5f), module, module->vco.::PHASE_POS_LIGHT));
}

Model *modelFunVModule = Model::create<FunVModule,
    FunVWidget>("Squinky Labs",
    "squinkylabs-funv",
    "FUnV", EFFECT_TAG, LFO_TAG);

#endif

