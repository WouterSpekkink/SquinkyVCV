
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
    addLabel(Vec(left -6, 48+ verticalShift), "Anlg");
    addLabel(Vec(left -6, 102+ verticalShift), "Dgtl");

    addParam(ParamWidget::create<Rogan3PSBlue>(Vec(center, 61 + verticalShift),
        module, module->vco.FREQ_PARAM, -54.0f, 54.0f, 0.0f));
    addLabel(Vec(center +6, 40+ verticalShift), "Pitch");

    addParam(ParamWidget::create<NKK>(Vec(right, 66 + verticalShift),
        module, module->vco.SYNC_PARAM, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(right-6, 48+ verticalShift), "Hard");
    addLabel(Vec(right-6, 102+ verticalShift), "Soft");
}

void FunVWidget::addMiddle4(FunVModule * module, float verticalShift)
{
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(23, 143 + verticalShift),
        module, module->vco.FINE_PARAM, -1.0f, 1.0f, 0.0f));

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(91, 143 + verticalShift),
        module, module->vco.PW_PARAM, 0.0f, 1.0f, 0.5f));

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(23, 208 + verticalShift),
        module, module->vco.FM_PARAM, 0.0f, 1.0f, 0.0f));

    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(91, 208 + verticalShift),
        module, module->vco.PWM_PARAM, 0.0f, 1.0f, 0.0f));

}

void FunVWidget::addJacks(FunVModule * module, float verticalShift)
{
    addInput(Port::create<PJ301MPort>(Vec(11, 273), Port::INPUT, module, module->vco.PITCH_INPUT));
    addInput(Port::create<PJ301MPort>(Vec(45, 273), Port::INPUT, module, module->vco.FM_INPUT));
    addInput(Port::create<PJ301MPort>(Vec(80, 273), Port::INPUT, module, module->vco.SYNC_INPUT));
    addInput(Port::create<PJ301MPort>(Vec(114, 273), Port::INPUT, module, module->vco.PW_INPUT));

    addOutput(Port::create<PJ301MPort>(Vec(11, 317), Port::OUTPUT, module, module->vco.SIN_OUTPUT));
    addOutput(Port::create<PJ301MPort>(Vec(45, 317), Port::OUTPUT, module, module->vco.TRI_OUTPUT));
    addOutput(Port::create<PJ301MPort>(Vec(80, 317), Port::OUTPUT, module, module->vco.SAW_OUTPUT));
    addOutput(Port::create<PJ301MPort>(Vec(114, 317), Port::OUTPUT, module, module->vco.SQR_OUTPUT));
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

