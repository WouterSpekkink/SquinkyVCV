
#include "Squinky.hpp"

#include "WidgetComposite.h"
#include "VocalFilter.h"

/**
 * Implementation class for VocalWidget
 */
struct VocalFilterModule : Module
{

    VocalFilterModule();

    /**
     * Overrides of Module functions
     */
    void step() override;
   // json_t *toJson() override;
   // void fromJson(json_t *rootJ) override;
    void onSampleRateChange() override;

    //using VocalFilter = VocalFilter<WidgetComposite>;
    VocalFilter<WidgetComposite> vocalFilter;
private:
    typedef float T;
};

VocalFilterModule::VocalFilterModule() : Module(vocalFilter.NUM_PARAMS, vocalFilter.NUM_INPUTS, vocalFilter.NUM_OUTPUTS, vocalFilter.NUM_LIGHTS),
vocalFilter(this)
{
    // TODO: can we assume onSampleRateChange() gets called first, so this is unnecessary?
    onSampleRateChange();
    vocalFilter.init();
}

void VocalFilterModule::onSampleRateChange()
{
    T rate = engineGetSampleRate();
    vocalFilter.setSampleRate(rate);
}

void VocalFilterModule::step()
{
    vocalFilter.step();
}

////////////////////
// module widget
////////////////////

struct VocalFilterWidget : ModuleWidget
{
    VocalFilterWidget(VocalFilterModule *);
    void addVowelLabels();
};

void VocalFilterWidget::addVowelLabels()
{
      const float ledX = 20;
    const float ledDx = 20;
    const float ledY = 24;
      
    const float vOffsetX = -8;
    const float vOffsetY = 14;
    for (int i=0; i<5; ++i) {
        VocalFilter<WidgetComposite>::LightIds id = (VocalFilter<WidgetComposite>::LightIds) i;
        std::string ltext;
        switch (id) {
            case VocalFilter<WidgetComposite>::LED_A:
                ltext = "A";
                break;
            case VocalFilter<WidgetComposite>::LED_E:
                ltext = "E";
                break;
            case VocalFilter<WidgetComposite>::LED_I:
                ltext = "I";
                break;
            case VocalFilter<WidgetComposite>::LED_O:
                ltext = "O";
                break;
            case VocalFilter<WidgetComposite>::LED_U:
                ltext = "U";
                break;
            default:
                assert(false);

        }
        Label * label = nullptr;label = new Label();
        label->text = ltext;
        label->color = COLOR_BLACK;
        label->box.pos = Vec(ledX + vOffsetX + i * ledDx, ledY + vOffsetY);
         
        addChild(label);

         addChild(ModuleLightWidget::create<MediumLight<RedLight>>(
            Vec(ledX + i * ledDx, ledY), module, id));
    }

}


/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */

VocalFilterWidget::VocalFilterWidget(VocalFilterModule *module) : ModuleWidget(module)
{
    box.size = Vec(12 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

    addVowelLabels();

    const float mid = 100;          // the middle area, with the four main knobs
    const float row2 = mid + 20;    //the first row of knobs
    const float row3 = row2 + 60;

    const float col1 = 10;      // the left hand strip of inputs and atternuverters
    const float col2 = 50;      // left column of big knobs
    const float col3 = 100;
    const float col4 = 150;     // inputs and attv on right

    const float labelOffset = -12;  // height of label above knob
    const float dy = 30;            // vertical space between input and atten

    //Vowels
    Label * label = new Label();
    label->box.pos = Vec(col2, row2 + labelOffset);
    label->text = "Vowel";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(col2, row2), module, module->vocalFilter.FILTER_VOWEL_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(col1, row2-dy), Port::INPUT, module, module->vocalFilter.FILTER_VOWEL_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(col1, row2), module, module->vocalFilter.FILTER_VOWEL_TRIM_PARAM, -1.0, 1.0, 1.0));

    // Fc
    label = new Label();
    label->box.pos = Vec(col3, row2 + labelOffset);
    label->text = "Fc";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(col3, row2), module, module->vocalFilter.FILTER_FC_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(col4, row2-dy), Port::INPUT, module, module->vocalFilter.FILTER_FC_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(col4, row2), module, module->vocalFilter.FILTER_FC_TRIM_PARAM, -1.0, 1.0, 1.0));

    // Q
    label = new Label();
    label->box.pos = Vec(col2, row3 + labelOffset);
    label->text = "Q";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(col2, row3), module, module->vocalFilter.FILTER_Q_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(col1, row3 +dy), Port::INPUT, module, module->vocalFilter.FILTER_Q_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(col1, row3), module, module->vocalFilter.FILTER_Q_TRIM_PARAM, -1.0, 1.0, 1.0));


    // Brightness

    label = new Label();
    label->box.pos = Vec(col3, row3 + labelOffset);
    label->text = "Brite";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(col3, row3), module, module->vocalFilter.FILTER_BRIGHTNESS_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(col4, row3+dy), Port::INPUT, module, module->vocalFilter.FILTER_BRIGHTNESS_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(col4, row3), module, module->vocalFilter.FILTER_BRIGHTNESS_TRIM_PARAM, -1.0, 1.0, 1.0));

}

#if 0 // orig
VocalFilterWidget::VocalFilterWidget(VocalFilterModule *module) : ModuleWidget(module)
{
    box.size = Vec(9 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

    addVowelLabels();

   
    // LEDS



   

    const float knobX = 75;
    const float trimX = 46;
    const float inputX = 8;
    float knobY = 30;
    const float space = 50;
    const float labelX = 0;
    const float labelOffset = -4;
    const float inputYOffset = 16;
    const float trimYOffset = 18;
    const float switchX = 120;
    Label * label=nullptr;
  

    // Brightness knob
       // Filter FC
    knobY += space;
    label = new Label();
    label->box.pos = Vec(labelX, knobY + labelOffset);
    label->text = "Brightness";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(knobX, knobY), module, module->vocalFilter.FILTER_BRIGHTNESS_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(inputX, knobY + inputYOffset), Port::INPUT, module, module->vocalFilter.FILTER_BRIGHTNESS_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(trimX, knobY + trimYOffset), module, module->vocalFilter.FILTER_BRIGHTNESS_TRIM_PARAM, -1.0, 1.0, 1.0));


    // Filter FC
    knobY += space;
    label = new Label();
    label->box.pos = Vec(labelX, knobY + labelOffset);
    label->text = "Fc filter";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(knobX, knobY), module, module->vocalFilter.FILTER_FC_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(inputX, knobY + inputYOffset), Port::INPUT, module, module->vocalFilter.FILTER_FC_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(trimX, knobY + trimYOffset), module, module->vocalFilter.FILTER_FC_TRIM_PARAM, -1.0, 1.0, 1.0));

    //Vowels
    knobY += space;
    label = new Label();
    label->box.pos = Vec(labelX, knobY + labelOffset);
    label->text = "Vowel";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(knobX, knobY), module, module->vocalFilter.FILTER_VOWEL_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(inputX, knobY + inputYOffset), Port::INPUT, module, module->vocalFilter.FILTER_VOWEL_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(trimX, knobY + trimYOffset), module, module->vocalFilter.FILTER_VOWEL_TRIM_PARAM, -1.0, 1.0, 1.0));

    // Q  
    knobY += space;
    label = new Label();
    label->box.pos = Vec(labelX, knobY + labelOffset);
    label->text = "Q";
    label->color = COLOR_BLACK;
    addChild(label);
    addParam(ParamWidget::create<Rogan1PSBlue>(Vec(knobX, knobY), module, module->vocalFilter.FILTER_Q_PARAM, -5.0, 5.0, 0.0));
    addInput(Port::create<PJ301MPort>(Vec(inputX, knobY + inputYOffset), Port::INPUT, module, module->vocalFilter.FILTER_Q_CV_INPUT));
    addParam(ParamWidget::create<Trimpot>(Vec(trimX, knobY + trimYOffset), module, module->vocalFilter.FILTER_Q_TRIM_PARAM, -1.0, 1.0, 1.0));

    // 3 pos vocal model
    //addParam(ParamWidget::create<CKSSThree>(Vec(switchX, 255), module, module->vocalFilter.FILTER_MODEL_SELECT_PARAM, 0.0f, 2.0f, 0.0f));
    label = new Label();
    label->box.pos = Vec(10, 282);
    label->text = "Model";
    label->color = COLOR_BLACK;
    addChild(label);

    addParam(ParamWidget::create<RoundBlackSnapKnob>(Vec(60, 276), module, module->vocalFilter.FILTER_MODEL_SELECT_PARAM, 0.0f, 3.0f, 3.0f));
	

    // I.O on row 3
    const float AudioInputX = 10.0;
    const float outputX = 57.0;
    const float row3 = 317.5;

    addInput(Port::create<PJ301MPort>(Vec(AudioInputX, row3), Port::INPUT, module, module->vocalFilter.AUDIO_INPUT));
    addOutput(Port::create<PJ301MPort>(Vec(outputX, row3), Port::OUTPUT, module, module->vocalFilter.AUDIO_OUTPUT));


 /*************************************************
     *  screws
     */
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

}
#endif

// Specify the Module and ModuleWidget subclass, human-readable
// manufacturer name for categorization, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelVocalFilterModule = Model::create<VocalFilterModule, VocalFilterWidget>("Squinky Labs",
    "squinkylabs-vocalfilter",
    "Vocal Filter", EFFECT_TAG, FILTER_TAG);
