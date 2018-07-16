
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

    void addHarmonics(CHBModule *module, const Vec& pos);
    void addHarmonicsRow(CHBModule *module, int row, const Vec& pos);
    void resetMe();
};


/*
    const float knobX = 25;
    const float knobY= 70;
    const float knobDY = 45;
    */
inline void CHBWidget::addHarmonics(CHBModule *module, const Vec& pos)
{
    addHarmonicsRow(module, 0, pos);
    Vec pos2 = pos;
    pos2.y += 40;
    addHarmonicsRow(module, 1, pos2);
}
inline void CHBWidget::addHarmonicsRow(CHBModule *module, int row, const Vec& pos)
{
    int firstParam = 0;
    int lastParam = 0;
    switch (row) {
        case 0:
            firstParam = module->chb.PARAM_H0;
            lastParam = module->chb.PARAM_H5;
            break;
        case 1:
            firstParam = module->chb.PARAM_H6;
            lastParam = module->chb.PARAM_H10;
            break;
        default:
            assert(false);
    }

  //  printf("%d, %d, %d\n", row, firstParam, lastParam); fflush(stdout);
   // return;
    for (int param = firstParam; param <= lastParam; ++param) {
        Vec p;
        p.x = pos.x + (param - firstParam) * 30;
        p.y = pos.y;
        addParam(ParamWidget::create<Trimpot>(
            p, module, param, 0.0f, 1.0f, 1.0f));
    }
}

void CHBWidget::resetMe()
{
    printf("on click\n"); fflush(stdout);
   // auto paramNum =CHB<WidgetComposite>::PARAM_H0;

    for (auto p : this->params) {
        p->setValue(p->defaultValue);
    }
}


/**
 */
struct SQPush : SVGButton
{
    SQPush()
    {
        setSVGs(
            SVG::load(assetGlobal("res/ComponentLibrary/BefacoPush_0.svg")),
            SVG::load(assetGlobal("res/ComponentLibrary/BefacoPush_1.svg"))
        );
    }

    void onDragEnd(EventDragEnd &e) override
    {
        SVGButton::onDragEnd(e);
        if (clickHandler) {
            clickHandler();
        }
    }

    void onClick( std::function<void(void)> callback)
    {
        clickHandler = callback;
    }

    std::function<void(void)> clickHandler;
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


    const float row1 = 30;
    const float label1 = row1 + 25;
    addInput(Port::create<PJ301MPort>(
        Vec(20, row1), Port::INPUT, module, module->chb.CV_INPUT));
    addLabel(Vec(15, label1), "CV");

    addInput(Port::create<PJ301MPort>(
        Vec(70, row1), Port::INPUT, module, module->chb.ENV_INPUT));
    addLabel(Vec(65, label1), "EG");

    addInput(Port::create<PJ301MPort>(
        Vec(120, row1), Port::INPUT, module, module->chb.AUDIO_INPUT));
    addLabel(Vec(115, label1), "In");


    addParam(ParamWidget::create<CKSS>(
        Vec(170, row1), module, module->chb.PARAM_FOLD, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(160, label1-46), "fold");
    addLabel(Vec(160, label1), "clip");


    addParam(ParamWidget::create<Trimpot>(
        Vec(150, 100), module, module->chb.PARAM_PITCH, -5.0f, 5.0f, 0));
    addLabel(Vec(140, 120), "Pitch");

    addParam(ParamWidget::create<Trimpot>(
        Vec(100, 100), module, module->chb.PARAM_EXTGAIN, -5.0f, 5.0f, 0));
    addLabel(Vec(95, 120), "Gain");

    //auto sw = new BefacoSwitch();
    auto sw = new SQPush();
   // sw->box.size = Vec();
    sw->box.pos = Vec(20, 120);
    sw->onClick( [this, module]() {
        this->resetMe();
#if 0
        printf("on click\n"); fflush(stdout);
        auto paramNum = module->chb.PARAM_H0;

        for (auto p : this->params) {
            printf("id = %d\n", p->paramId);
            if (paramNum == p->paramId) {
                printf("found it\n");
                p->setValue(0);
            }
#endif


    });
    addChild(sw);


    addHarmonics(module, Vec(25, 220));

    addOutput(Port::create<PJ301MPort>(
        Vec(40, 300), Port::OUTPUT, module, module->chb.OUTPUT));
    addLabel(Vec(35, 325), "Out");



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


