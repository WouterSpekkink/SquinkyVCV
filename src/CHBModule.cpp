
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"


#include "CHB.h"


/**
 */
struct CHBModule : Module
{
public:
    CHBModule();
    /**
     *
     * Overrides of Module functions
     */
    void step() override;
    void onSampleRateChange() override;

    CHB<WidgetComposite> chb;
private:
};

void CHBModule::onSampleRateChange()
{
    //float rate = engineGetSampleRate();
   // gmr.setSampleRate(rate);
   float sampleTime = engineGetSampleTime();
   chb.setSampleTime(sampleTime);
}

CHBModule::CHBModule()
    : Module(chb.NUM_PARAMS,
    chb.NUM_INPUTS,
    chb.NUM_OUTPUTS,
    chb.NUM_LIGHTS),
    chb(this)
{
    onSampleRateChange();
    chb.init();
}

void CHBModule::step()
{
    chb.step();
}

////////////////////
// module widget
////////////////////

struct CHBWidget : ModuleWidget
{
    CHBWidget(CHBModule *);

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
CHBWidget::CHBWidget(CHBModule *module) : ModuleWidget(module)
{ 
    box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/chb_panel.svg")));
        addChild(panel);
    }
    printf("\nbox size = %f, %f\n", box.size.x, box.size.y);

    const float row1=30;
    addInput(Port::create<PJ301MPort>(
        Vec(20, row1), Port::INPUT, module, module->chb.CV_INPUT));
    addInput(Port::create<PJ301MPort>(
        Vec(70, row1), Port::INPUT, module, module->chb.ENV_INPUT));
 

    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(150, 100), module, module->chb.PARAM_PITCH, -5.0f, 5.0f, 0));

    const float knobX = 25;
    const float knobY= 70;
    const float knobDY = 45;



    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY), module, module->chb.PARAM_H0, 0.0f, 1.0f, 1.0f));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 1 * knobDY), module, module->chb.PARAM_H1, 0.0f, 1.0f, 1.0f));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 2 * knobDY), module, module->chb.PARAM_H2, 0.0f, 1.0f, 1.0f));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 3 * knobDY), module, module->chb.PARAM_H3, 0.0f, 1.0f, 1.0f));
    addParam(ParamWidget::create<Rogan1PSBlue>(
        Vec(knobX, knobY + 4 * knobDY), module, module->chb.PARAM_H4, 0.0f, 1.0f, 1.0f));
   
    addOutput(Port::create<PJ301MPort>(
        Vec(40, 300), Port::OUTPUT, module, module->chb.OUTPUT));
 


    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

Model *modelCHBModule = Model::create<CHBModule,
    CHBWidget>("Squinky Labs",
    "squinkylabs-CHB",
    "CHB", EFFECT_TAG, LFO_TAG);


