
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
    printf("on sample rate change\n"); fflush(stdout);
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
    printf("ctor\n"); fflush(stdout);
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
    box.size = Vec(8 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }


		addChild(Widget::create<ScrewSilver>(Vec(15, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15, 365)));
		addChild(Widget::create<ScrewSilver>(Vec(15*6, 0)));
		addChild(Widget::create<ScrewSilver>(Vec(15*6, 365)));

		addParam(ParamWidget::create<RoundHugeBlackKnob>(Vec(22, 32), module, module->vco.OCTAVE_PARAM, -5.0, 4.0, 0.0));
		addParam(ParamWidget::create<RoundSmallBlackKnob>(Vec(73, 131), module, module->vco.TUNE_PARAM, -7.0, 7.0, 0.0));
		addParam(ParamWidget::create<RoundLargeBlackKnob>(Vec(16, 230), module, module->vco.PWM_PARAM, -1.0, 1.0, 0.0));

		addInput(Port::create<PJ301MPort>(Vec(8, 120), Port::INPUT, module, module->vco.PITCH1_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(19, 157), Port::INPUT, module, module->vco.PITCH2_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(48, 183), Port::INPUT, module, module->vco.FM_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(86, 189), Port::INPUT, module, module->vco.SYNC_INPUT));

		addInput(Port::create<PJ301MPort>(Vec(72, 236), Port::INPUT, module, module->vco.PWM_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(10, 283), Port::OUTPUT, module, module->vco.TRI_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(87, 283), Port::OUTPUT, module, module->vco.SINE_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(48, 306), Port::OUTPUT, module, module->vco.EVEN_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(10, 327), Port::OUTPUT, module, module->vco.SAW_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(87, 327), Port::OUTPUT, module, module->vco.SQUARE_OUTPUT));

#if 0
    addInput(Port::create<PJ301MPort>(
        Vec(40, 200), Port::INPUT, module, module->vco.CLOCK_INPUT));
    addOutput(Port::create<PJ301MPort>(
        Vec(40, 300), Port::OUTPUT, module, module->vco.TRIGGER_OUTPUT));

    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
 #endif

}

Model *modelEVModule = Model::create<EVModule,
    EVWidget>("Squinky Labs",
    "squinkylabs-evco",
    "EVCO", EFFECT_TAG);

    #endif

