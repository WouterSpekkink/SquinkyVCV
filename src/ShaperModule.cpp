
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
    void addSelector(ShaperModule* module);
};

void ShaperWidget::addSelector(ShaperModule* module)
{
    const float x = 80;
    const float y = 100;
    auto p = createParamCentered<Rogan1PSBlue>(
        Vec(x, y),
        module, Shaper<WidgetComposite>::PARAM_SHAPE, 0, 4, 0);
    p->snap = true;
	p->smooth = false;
    addParam(p);
    addLabel(Vec(x-45, y+15), "clip");
    addLabel(Vec(x-60, y-10), "soft");
    addLabel(Vec(x-16, y-40), "fw");
    addLabel(Vec(x+20, y-10), "hw");
    addLabel(Vec(x+10, y + 15), "fold");
}



/**
 * Global coordinate constants
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
    addLabel(Vec(30, 20), "Wave Shaper");

    addSelector(module);

    addInput(createInputCentered<PJ301MPort>(
            Vec(40,340),
            module,
            Shaper<WidgetComposite>::INPUT_AUDIO));

    addOutput(createOutputCentered<PJ301MPort>(
            Vec(120,340),
            module,
            Shaper<WidgetComposite>::OUTPUT_AUDIO));

    
    const float labelDeltaY = 25;
    const float gainX = 40;
    const float offsetX = 120;
    const float labelDeltaX = -20;
    const float y = 170;

    addParam(createParamCentered<Rogan1PSBlue>(
        Vec(gainX, y),
        module, Shaper<WidgetComposite>::PARAM_GAIN, 0, 10, 1));
    addLabel(Vec(gainX+labelDeltaX, y + labelDeltaY), "gain");

    addParam(createParamCentered<Rogan1PSBlue>(
        Vec(offsetX, y),
        module, Shaper<WidgetComposite>::PARAM_OFFSET, -10, 10, 0));
    addLabel(Vec(offsetX+labelDeltaX,  y + labelDeltaY), "offset");

    const float deltaYTrim = 60;
    const float deltaYInput = 90;
    addParam(createParamCentered<Trimpot>(
        Vec(gainX, y + deltaYTrim),
        module, Shaper<WidgetComposite>::PARAM_GAIN_TRIM, -1, 1, 0));
    addParam(createParamCentered<Trimpot>(
        Vec(offsetX, y + deltaYTrim),
        module, Shaper<WidgetComposite>::PARAM_OFFSET_TRIM, -1, 1, 0));
    addInput(createInputCentered<PJ301MPort>(
            Vec(gainX,y + deltaYInput),
            module,
            Shaper<WidgetComposite>::INPUT_GAIN));
    addInput(createInputCentered<PJ301MPort>(
            Vec(offsetX,y + deltaYInput),
            module,
            Shaper<WidgetComposite>::INPUT_OFFSET));

    
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

