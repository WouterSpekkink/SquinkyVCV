#ifdef _EXP
#include "Squinky.hpp"

#include "WidgetComposite.h"
//#include "VocalFilter.h"


struct SequencerModule : Module
{
};


struct SequencerWidget : ModuleWidget
{
    SequencerWidget(SequencerModule *);
};

 SequencerWidget::SequencerWidget(SequencerModule *module) : ModuleWidget(module)
{
    box.size = Vec(9 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }
}

// Specify the Module and ModuleWidget subclass, human-readable
// manufacturer name for categorization, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelSequencerModule = Model::create<SequencerModule, SequencerWidget>("Squinky Labs",
    "squinkylabs-sequencer",
    "S", SEQUENCER_TAG);

#endif