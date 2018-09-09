
#include "Squinky.hpp"
#include "WidgetComposite.h"


#include "Shaper.h"

/**
 */
struct ShaperModule : Module
{
public:
    ShaperModule();
    /**
     *
     *
     * Overrides of Module functions
     */
    void step() override;

    Shaper<WidgetComposite> shaper;
private:
};

ShaperModule::ShaperModule()
    : Module(shaper.NUM_PARAMS,
    shaper.NUM_INPUTS,
    shaper.NUM_OUTPUTS,
    shaper.NUM_LIGHTS),
    shaper(this)
{
}

void ShaperModule::step()
{
    shaper.step();
}

////////////////////
// module widget
////////////////////

struct ShaperWidget : ModuleWidget
{
    ShaperWidget(ShaperModule *);

    /**
     * Helper to add a text label to this widget
     */
    Label* addLabel(const Vec& v, const char* str, const NVGcolor& color = COLOR_BLACK)
    {
        Label* label = new Label();
        label->box.pos = v;
        label->text = str;
        label->color = color;
        addChild(label);
        return label;
    }


private:

    ShaperModule* const module;
};



/**
 * Global coordinate contstants
 */
/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
ShaperWidget::ShaperWidget(ShaperModule *module) :
    ModuleWidget(module),
    module(module)
{
    box.size = Vec(10 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);
    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }

  

   
    addInput(createInputCentered<PJ301MPort>(
            Vec(100,40),
            module,
            Shaper<WidgetComposite>::INPUT_AUDIO));

    addOutput(createOutputCentered<PJ301MPort>(
            Vec(100,120),
            module,
            Shaper<WidgetComposite>::OUTPUT_AUDIO));
  
   

 
    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH))); 
}

Model *modelShaperModule = Model::create<ShaperModule,
    ShaperWidget>("Squinky Labs",
    "squinkylabs-shp",
    "shap", EFFECT_TAG, OSCILLATOR_TAG, WAVESHAPER_TAG);

