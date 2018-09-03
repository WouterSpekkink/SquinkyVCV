


#include "Squinky.hpp"
#include "SQWidgets.h"
#include "WidgetComposite.h"

#include <sstream>

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
    friend struct CHBEconomyItem;
    CHBWidget(CHBModule *);

   // Menu* createContextMenu() override;

    /**
     * Helper to add a text label to this widget
     */
    void addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
    }

    void addHarmonics(CHBModule *module);
    void addVCOKnobs(CHBModule *module);
    void addOtherKnobs(CHBModule *module);
    void addMisc(CHBModule *module);
    void addBottomJacks(CHBModule *module);


  //  void addHarmonicsRow(CHBModule *module, int row, const Vec& pos);
 //   void addVCO(CHBModule *module, const Vec& pos);
 //   void addMixer(CHBModule *module, const Vec& pos);
 //   void addFolder(CHBModule *module, const Vec& pos);

    void resetMe(CHBModule *module);
private:
    bool fake;
    #if 0
    bool isEconomy() const
    {
        return module->chb.isEconomy();
    }
    void setEconomy(bool b)
    {
        module->chb.setEconomy(b);
    }
    #endif
    const int numHarmonics;
    CHBModule* const module;
    std::vector<ParamWidget* > harmonicParams;
    std::vector<float> harmonicParamMemory;
};

#if 0
struct CHBEconomyItem : MenuItem
{
    void onAction(EventAction &e) override
    {
        const bool econ = !theWidget->isEconomy();
        theWidget->setEconomy(econ);
    }

    void step() override
    {
        rightText = CHECKMARK(theWidget->isEconomy());
    }

    CHBWidget* theWidget = nullptr;
};

inline Menu* CHBWidget::createContextMenu()
{
    Menu* theMenu = ModuleWidget::createContextMenu();
    CHBEconomyItem * item = new CHBEconomyItem();
    item->text = "CPU Economy Mode";
    item->theWidget = this;
    theMenu->addChild(item);

    return theMenu;
}
#endif

/**
 * Global coordinate contstants
 */
const float colHarmonicsJacks = 21;
const float rowFirstHarmonicJackY = 47;
const float harmonicJackSpacing = 32;
const float harmonicTrimDeltaY = 27;

// columns of knobs
const float col1 = 95;
const float col2 = 150;
const float col3 = 214;

// rows of knobs
const float row1 = 75;
const float row2 = 131;
const float row3 = 228;
const float row4 = 287;
const float row5 = 332;

const float labelAboveKnob = 32;
const float labelAboveJack = 30;

inline void CHBWidget::addHarmonics(CHBModule *module)
{
    for (int i=0; i< numHarmonics; ++i) {
        const float row = rowFirstHarmonicJackY + i * harmonicJackSpacing;
        addInput(createInputCentered<PJ301MPort>(
            Vec(colHarmonicsJacks, row),
            module, 
            module->chb.H0_INPUT+i));

        auto p = createParamCentered<BlueTrimmer>(
            Vec(colHarmonicsJacks + harmonicTrimDeltaY, row), 
            module,
            module->chb.PARAM_H0+i,
            0.0f, 1.0f, 1.0f);
        addParam(p);
        harmonicParams.push_back(p);
    }
}


inline void CHBWidget::addVCOKnobs(CHBModule *module)
{
    addParam(createParamCentered<Blue30Knob>(
        Vec(col2, row1),
        module,
        module->chb.PARAM_OCTAVE,
        -5.0f, 4.0f, 0.f));
    addLabel(Vec(col2-26, row1 - labelAboveKnob), "Octave");

    addParam(createParamCentered<Blue30Knob>(
        Vec(col3, row1),
        module,
        module->chb.PARAM_TUNE,
        -5.0f, 5.0f, 0));
    addLabel(Vec(col3-26, row1 - labelAboveKnob), "Tune");

    addParam(createParamCentered<Blue30Knob>(
        Vec(col2, row2),
        module,
        module->chb.PARAM_PITCH_MOD_TRIM,
        0, 1.0f, 1.0f));
    addLabel(Vec(col2-18, row2 - labelAboveKnob), "Mod");

    addParam(createParamCentered<Blue30Knob>(
        Vec(col3, row2),
        module,
        module->chb.PARAM_LINEAR_FM_TRIM,
        0, 1.0f, 1.0f));
    addLabel(Vec(col3-18, row2 - labelAboveKnob), "LFM");
}

inline void CHBWidget::addOtherKnobs(CHBModule *module)
{
    // gain
     addParam(createParamCentered<Blue30Knob>(
        Vec(col1, 165),
        module,
        module->chb.PARAM_EXTGAIN,
        -5.0f, 5.0f, 0.f));
    addLabel(Vec(col1-20, 165 - labelAboveKnob), "Gain");

    // slopw
   addParam(createParamCentered<Blue30Knob>(
        Vec(185, 188),
        module,
        module->chb.PARAM_SLOPE,
        -5, 5, 5));
    addLabel(Vec(185-24, 188 - labelAboveKnob), "Slope");
    
    //even
   addParam(createParamCentered<Blue30Knob>(
        Vec(col2, row3),
        module,
         module->chb.PARAM_MAG_EVEN,
          0, 1, 1));
    addLabel(Vec(col2-20, row3 - labelAboveKnob), "Even");

    //odd
    addParam(createParamCentered<Blue30Knob>(
        Vec(col3, row3),
        module,
        module->chb.PARAM_MAG_ODD,
        0, 1, 1));
     addLabel(Vec(col3-20, row3 - labelAboveKnob), "Odd");
}

void CHBWidget::addMisc(CHBModule *module)
{
    
    auto sw = new SQPush();
    Vec pos(col1, 104);
    sw->center(pos);
    sw->onClick([this, module]() {
        this->resetMe(module);
    });
    addChild(sw);

    addLabel(Vec(col1-24, 104 - labelAboveKnob), "Preset");
    

    addParam(createParamCentered<NKKSmall>(
        Vec(col1, 219),
        module,
        module->chb.PARAM_FOLD,
        0.0f, 1.0f, 1.0f));
    addLabel(Vec(col1-20, 219 - 40), "Fold");
    addLabel(Vec(col1-20, 219 + 10), "Clip");
   
}

static const char* labels[] = {
    "V/Oct",
    "Mod",
    "LFM",
    "Slope",
    "Ext In",
    "Gain",
    "EG",
    "Out",
    };
static const int offsets[] = {
    -1,
    0,
    3,
    -4,
    -4,
    -2,
    4,
    2
};

void CHBWidget::addBottomJacks(CHBModule *module)
{
    const int deltaX = .5f + ((col3 - col1) / 3.0);
    for (int jackRow=0; jackRow<2; ++jackRow) {
        for (int jackCol=0; jackCol < 4; ++jackCol) {
            const Vec pos(col1 + deltaX * jackCol,
                jackRow == 0 ? row4 : row5);
            const int index = jackCol + 4 * jackRow;

           // printf("col=%d, row=%d, pos =%f, %f\n", jackCol, jackRow, pos.x, pos.y);
           // fflush(stdout);
           auto color = COLOR_BLACK;
           if (index == 7) {
               color = COLOR_WHITE;
           }
            addInput(createInputCentered<PJ301MPort>(
                pos,
                module, 
                module->chb.H0_INPUT));
            addLabel( Vec(pos.x-20+offsets[index], pos.y -labelAboveJack),
                labels[index],
                color);       
        }
    }
}
#if 0
void CHBWidget::addMixer(CHBModule *module, const Vec& pos)
{
    float trimRow = pos.y + 30;
    float labelRow = pos.y + 4;

    addInput(Port::create<PJ301MPort>(
        Vec(pos.x, trimRow - 4), Port::INPUT, module, module->chb.SLOPE_INPUT));

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x + 30, trimRow), module, module->chb.PARAM_SLOPE, -5, 5, 5));
    addLabel(Vec(pos.x + 5, labelRow), "slope");

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x + 60, trimRow), module, module->chb.PARAM_MAG_EVEN, 0, 1, 1));
    addLabel(Vec(pos.x + 48, labelRow), "even");

    addParam(ParamWidget::create<Blue30Knob>(
        Vec(pos.x + 90, trimRow), module, module->chb.PARAM_MAG_ODD, 0, 1, 1));
    addLabel(Vec(pos.x + 83, labelRow), "odd");

    addHarmonics(module, Vec(pos.x, trimRow + 55));
}
#endif

#if 0
void CHBWidget::addFolder(CHBModule *module, const Vec& pos)
{
    addInput(Port::create<PJ301MPort>(
        Vec(pos.x, pos.y + 7), Port::INPUT, module, module->chb.AUDIO_INPUT));
    addLabel(Vec(pos.x - 4, pos.y - 14), "Ext");

    addInput(Port::create<PJ301MPort>(
        Vec(pos.x, pos.y + 55), Port::INPUT, module, module->chb.ENV_INPUT));
    addLabel(Vec(pos.x - 4, pos.y + 36), "EG");

    addParam(ParamWidget::create<Trimpot>(
        Vec(pos.x + 3, pos.y + 106), module, module->chb.PARAM_EXTGAIN, -5.0f, 5.0f, 0));
    addLabel(Vec(pos.x - 20, pos.y + 85), "Gain");
    addInput(Port::create<PJ301MPort>(
        Vec(pos.x - 32, pos.y + 102), Port::INPUT, module, module->chb.GAIN_INPUT));

    addParam(ParamWidget::create<NKKSmall>(
        Vec(pos.x + 5, pos.y + 150), module, module->chb.PARAM_FOLD, 0.0f, 1.0f, 1.0f));
    addLabel(Vec(pos.x - 6, pos.y + 128), "fold");
    addLabel(Vec(pos.x - 6, pos.y + 180), "clip");

    addChild(ModuleLightWidget::create<SmallLight<GreenRedLight>>(
        Vec(pos.x - 20, pos.y + 140), module, module->chb.GAIN_GREEN_LIGHT));
}
#endif

#if 0
inline void CHBWidget::addVCO(CHBModule *module, const Vec& pos)
{
    const float inputRow = pos.y + 7;
    const float label1 = inputRow - 18;
    //-------------------- make the pitch control column
    addInput(Port::create<PJ301MPort>(
        Vec(17 + pos.x, inputRow), Port::INPUT, module, module->chb.CV_INPUT));
    addLabel(Vec(15 + pos.x, label1), "CV");

    addParam(ParamWidget::create<RoundBlackSnapKnob>(
        Vec(15 + pos.x, pos.y + 51), module, module->chb.PARAM_OCTAVE,
        -5.0f, 4.0f, 0.f));
    addLabel(Vec(14 + pos.x, pos.y + 34), "Oct");

    addParam(ParamWidget::create<Trimpot>(
        Vec(20 + pos.x, pos.y + 98), module, module->chb.PARAM_TUNE,
        -5.0f, 5.0f, 0));
    addLabel(Vec(9 + pos.x, pos.y + 80), "Tune");

    //--------------------------- make the mod column

    addInput(Port::create<PJ301MPort>(
        Vec(60 + pos.x, inputRow), Port::INPUT, module, module->chb.PITCH_MOD_INPUT));
    addLabel(Vec(53 + pos.x, label1), "Mod");

    addParam(ParamWidget::create<Trimpot>(
        Vec(60 + pos.x, inputRow + 45), module, module->chb.PARAM_PITCH_MOD_TRIM,
        0, 1.0f, 1.0f));

    addInput(Port::create<PJ301MPort>(
        Vec(100 + pos.x, inputRow), Port::INPUT, module, module->chb.LINEAR_FM_INPUT));
    addLabel(Vec(93 + pos.x, label1), "LFM");

    addParam(ParamWidget::create<Trimpot>(
        Vec(100 + pos.x, inputRow + 45), module, module->chb.PARAM_LINEAR_FM_TRIM,
        0, 1.0f, 1));
}
#endif

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

    for (int i = 1; i < numHarmonics; ++i) {
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
        for (int i = 0; i < numHarmonics; ++i) {
            harmonicParamMemory[i] = harmonicParams[i]->value;
        }
    }

    // fundamental -> all
    if (isOnlyFundamental) {
        for (int i = 0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue(1);
        }
    }
    // all -> preset, if any
    else if (isAll && havePreset) {
        for (int i = 0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue(harmonicParamMemory[i]);
        }
    }
    // preset -> fund. if no preset all -> fund
    else {
        for (int i = 0; i < numHarmonics; ++i) {
            harmonicParams[i]->setValue((i == 0) ? 1 : 0);
        }
    }
}





/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
CHBWidget::CHBWidget(CHBModule *module) :
    ModuleWidget(module),
    numHarmonics(module->chb.numHarmonics),
    module(module)
{
    box.size = Vec(16 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/chb_panel.svg")));
        addChild(panel);
    }
    addHarmonics(module);
    addVCOKnobs(module);
    addOtherKnobs( module);
    addMisc(module);
    addBottomJacks( module);
    
#if 0
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
    #endif

    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
}

Model *modelCHBModule = Model::create<CHBModule,
    CHBWidget>("Squinky Labs",
    "squinkylabs-CHB",
    "CHB", EFFECT_TAG, OSCILLATOR_TAG, WAVESHAPER_TAG);


#endif