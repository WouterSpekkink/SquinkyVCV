
#include <sstream>
#include "Squinky.hpp"
#include "WidgetComposite.h"

#ifdef _CHB
#include "CHB.h"


/**
 */
struct CHBModule : Module
{
public:
    CHBModule();
    /**
     *
     *
     * Overrides of Module functions
     */
    void step() override;
  //  void onSampleRateChange() override;

    CHB<WidgetComposite> chb;
private:
};

CHBModule::CHBModule()
    : Module(chb.NUM_PARAMS,
    chb.NUM_INPUTS,
    chb.NUM_OUTPUTS,
    chb.NUM_LIGHTS),
    chb(this)
{
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
    void addVCO(CHBModule *module, const Vec& pos);
    void addMixer(CHBModule *module, const Vec& pos);
    void addFolder(CHBModule *module, const Vec& pos);
    void resetMe(CHBModule *module);
private:
    const int numHarmonics;
    std::vector<ParamWidget* > harmonicParams;
    std::vector<float> harmonicParamMemory;
};

inline void CHBWidget::addHarmonics(CHBModule *module, const Vec& pos)
{
    addHarmonicsRow(module, 0, pos);
    Vec pos2 = pos;
    pos2.y += 60;
    addHarmonicsRow(module, 1, pos2);
}

inline void CHBWidget::addHarmonicsRow(CHBModule *module, int row, const Vec& pos)
{
    int firstParam = 0;
    int lastParam = 0;
    int firstInput = 0;
    switch (row) {
        case 0:
            firstParam = module->chb.PARAM_H0;
            lastParam = module->chb.PARAM_H4;
            firstInput = module->chb.H0_INPUT;
            break;
        case 1:
            firstParam = module->chb.PARAM_H5;
            lastParam = module->chb.PARAM_H9;
            firstInput = module->chb.H6_INPUT;
            break;
        default:
            assert(false);
    }

   int input = firstInput;
    for (int param = firstParam; param <= lastParam; ++param, ++input) {
        Vec pKnob;
        Vec pJack;
        pKnob.x = pos.x + (param - firstParam) * 30;
        pKnob.y = pos.y;

        pJack = pKnob;
        pKnob.y += 30;
        pKnob.x += 3;
       
        addInput(Port::create<PJ301MPort>(
            pJack, Port::INPUT, module, input));
        auto p = ParamWidget::create<Trimpot>(
           pKnob, module, param, 0.0f, 1.0f, 1.0f);
        addParam(p);

        harmonicParams.push_back(p);

        std::stringstream str;
        str << "h_" << param - module->chb.PARAM_H0;
        p->label = str.str();
    }
}

void CHBWidget::resetMe(CHBModule *module)
{
    bool isOnlyFundamental = true;
    bool isAll = true;
    bool havePreset = !harmonicParamMemory.empty();
    const float val0 = harmonicParams[0]->value;
    if (val0 < .99) {
        isOnlyFundamental = false;
        isAll = false;
    }

    for (int i=1; i < numHarmonics; ++i) {
        const float value = harmonicParams[i]->value;
        if (value < .9) {
            isAll = false;
        }

        if (value > .1) {
            isOnlyFundamental = false;
        }
    }

    if (!isOnlyFundamental && !isAll) {
        // take snapshot
        if (harmonicParamMemory.empty()) {
            harmonicParamMemory.resize(numHarmonics);
        }
        for (int i=0; i < numHarmonics; ++i) {
            harmonicParamMemory[i] = harmonicParams[i]->value;
        }
    }

    // fundamental -> all
    if (isOnlyFundamental) {
        for (int i=0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue(1);
        }
    }
    // all -> preset, if any
    else if (isAll && havePreset) {
        for (int i=0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue(harmonicParamMemory[i]);
        }
    }
    // preset -> fund. if no preset all -> fund
    else  {
        for (int i=0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue((i == 0) ? 1 : 0);
        }
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

    void onClick(std::function<void(void)> callback)
    {
        clickHandler = callback;
    }

    std::function<void(void)> clickHandler;
};

inline void CHBWidget::addVCO(CHBModule *module, const Vec& pos)
{  
    const float inputRow = pos.y + 7;
    const float label1 = inputRow-18;
    //-------------------- make the pitch control column
    addInput(Port::create<PJ301MPort>(
        Vec(17 + pos.x, inputRow), Port::INPUT, module, module->chb.CV_INPUT));
    addLabel(Vec(15+ pos.x, label1), "CV");

    addParam(ParamWidget::create<RoundBlackSnapKnob>(
        Vec(15 + pos.x, pos.y + 51), module, module->chb.PARAM_OCTAVE,
        -5.0f, 4.0f, 0.f));
    addLabel(Vec(14 + pos.x, pos.y+34), "Oct");

    addParam(ParamWidget::create<Trimpot>(
        Vec(20 + pos.x, pos.y+98), module, module->chb.PARAM_TUNE,
        -5.0f, 5.0f, 0));
    addLabel(Vec(9 + pos.x, pos.y + 80), "Tune");

    //--------------------------- make the mod column

     addInput(Port::create<PJ301MPort>(
        Vec(60 + pos.x, inputRow), Port::INPUT, module, module->chb.PITCH_MOD_INPUT));
    addLabel(Vec(53+ pos.x, label1), "Mod");

    addParam(ParamWidget::create<Trimpot>(
        Vec(60 + pos.x, inputRow+45), module, module->chb.PARAM_PITCH_MOD_TRIM,
        0, 1.0f, 1.0f));
  
    addInput(Port::create<PJ301MPort>(
        Vec(100 + pos.x, inputRow), Port::INPUT, module, module->chb.LINEAR_FM_INPUT));
    addLabel(Vec(93+ pos.x, label1), "LFM");

     addParam(ParamWidget::create<Trimpot>(
        Vec(100 + pos.x, inputRow+45), module, module->chb.PARAM_LINEAR_FM_TRIM,
        0, 1.0f, 1));
}

void CHBWidget::addMixer(CHBModule *module, const Vec& pos)
{
    float trimRow = pos.y + 30;
    float labelRow = pos.y + 4;

    addInput(Port::create<PJ301MPort>(
        Vec(pos.x, trimRow - 4), Port::INPUT, module, module->chb.SLOPE_INPUT));

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x + 30 ,trimRow), module, module->chb.PARAM_SLOPE, -5,5,5));
    addLabel(Vec(pos.x+5, labelRow), "slope");

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x + 60, trimRow), module, module->chb.PARAM_MAG_EVEN, 0, 1, 1));
    addLabel(Vec(pos.x+48, labelRow), "even");

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x+ 90, trimRow), module, module->chb.PARAM_MAG_ODD, 0,1,1));
    addLabel(Vec(pos.x+83, labelRow), "odd");

    addHarmonics(module, Vec(pos.x, trimRow + 55));
}

void CHBWidget::addFolder(CHBModule *module, const Vec& pos)
{
   addInput(Port::create<PJ301MPort>(
        Vec(pos.x, pos.y + 7), Port::INPUT, module, module->chb.AUDIO_INPUT));
    addLabel(Vec(pos.x-4, pos.y-14), "Ext");

    addInput(Port::create<PJ301MPort>(
        Vec(pos.x, pos.y + 55), Port::INPUT, module, module->chb.ENV_INPUT));
    addLabel(Vec(pos.x-4, pos.y + 36), "EG");

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x+3, pos.y + 106), module, module->chb.PARAM_EXTGAIN, -5.0f, 5.0f, 0));
    addLabel(Vec(pos.x-20, pos.y+85), "Gain");
    addInput(Port::create<PJ301MPort>(
        Vec(pos.x - 32, pos.y+102), Port::INPUT, module, module->chb.GAIN_INPUT));
    
    addParam(ParamWidget::create<CKSS>(
        Vec(pos.x+5, pos.y + 148), module, module->chb.PARAM_FOLD, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(pos.x-6, pos.y + 128), "fold");
    addLabel(Vec(pos.x-6, pos.y + 168), "clip");
}

/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
CHBWidget::CHBWidget(CHBModule *module) : 
    ModuleWidget(module),
    numHarmonics(module->chb.numHarmonics)
{
    box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/chb_panel.svg")));
        addChild(panel);
    }

    const float row1 = 30;
    addVCO(module, Vec(10, row1));
    addMixer(module, Vec(12, 155));
    addFolder(module, Vec(188, row1));

    auto sw = new SQPush();
    sw->box.pos = Vec(180, 240);
    sw->onClick([this, module]() {
        this->resetMe(module);
    });
    addChild(sw);
    addOutput(Port::create<PJ301MPort>(
        Vec(180, 330), Port::OUTPUT, module, module->chb.MIX_OUTPUT));
    addLabel(Vec(170, 310), "Out");

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


#endif