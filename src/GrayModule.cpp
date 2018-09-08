
#include "Squinky.hpp"
#include "WidgetComposite.h"


#include "Gray.h"

/**
 */
struct GrayModule : Module
{
public:
    GrayModule();
    /**
     *
     *
     * Overrides of Module functions
     */
    void step() override;

    Gray<WidgetComposite> gray;
private:
};

GrayModule::GrayModule()
    : Module(gray.NUM_PARAMS,
    gray.NUM_INPUTS,
    gray.NUM_OUTPUTS,
    gray.NUM_LIGHTS),
    gray(this)
{
}

void GrayModule::step()
{
    gray.step();
}

////////////////////
// module widget
////////////////////

struct GrayWidget : ModuleWidget
{
    GrayWidget(GrayModule *);

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

    GrayModule* const module;
};

/**
 * Global coordinate contstants
 */
/**
 * Widget constructor will describe my implementation structure and
 * provide meta-data.
 * This is not shared by all modules in the DLL, just one
 */
GrayWidget::GrayWidget(GrayModule *module) :
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

 
    // screws
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
    addChild(Widget::create<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
    addChild(Widget::create<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH))); 
}

Model *modelGrayModule = Model::create<GrayModule,
    GrayWidget>("Squinky Labs",
    "squinkylabs-gry",
    "gry", EFFECT_TAG, OSCILLATOR_TAG, WAVESHAPER_TAG);

