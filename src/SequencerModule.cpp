#ifdef _EXP
#include "Squinky.hpp"

#include "WidgetComposite.h"
//#include "VocalFilter.h"
#include "Widgets.hpp"


struct SequencerModule : Module
{
};


struct SequencerWidget : ModuleWidget
{
    SequencerWidget(SequencerModule *);
};


struct NoteDisplay : OpaqueWidget {
    SequencerModule *module;

    void draw(NVGcontext *vg) override 
    {
        nvgScale(vg, 2, 2);
        nvgFillColor(vg, nvgRGBA(0xff, 0x00, 0x00, 0xff));
        nvgBeginPath(vg);
        nvgRect(vg, 50, 50, 30, 30);
		nvgFill(vg);

         nvgFillColor(vg, nvgRGBA(0x00, 0x00, 0xff, 0xff));
        nvgBeginPath(vg);
        nvgRect(vg, 0, 0, 30, 30);
		nvgFill(vg);
    }
};

                                  //SequencerModule
 SequencerWidget::SequencerWidget(SequencerModule *module) : ModuleWidget(module)
{
    const int width = (14 + 14) * RACK_GRID_WIDTH;      // 14 for panel, 14 for notes
    box.size = Vec(width, RACK_GRID_HEIGHT);

    {
        SVGPanel *panel = new SVGPanel();
        panel->box.size = box.size;
        panel->setBackground(SVG::load(assetPlugin(plugin, "res/blank_panel.svg")));
        addChild(panel);
    }
    #if 1
	{
		NoteDisplay *display = new NoteDisplay();
		display->module = module;
		display->box.pos = Vec( 14 * RACK_GRID_WIDTH, 0);
		display->box.size = Vec(14 * RACK_GRID_WIDTH,RACK_GRID_HEIGHT);
		addChild(display);
	}
    #endif
}

// Specify the Module and ModuleWidget subclass, human-readable
// manufacturer name for categorization, module slug (should never
// change), human-readable module name, and any number of tags
// (found in `include/tags.hpp`) separated by commas.
Model *modelSequencerModule = Model::create<SequencerModule, SequencerWidget>("Squinky Labs",
    "squinkylabs-sequencer",
    "S", SEQUENCER_TAG);

#endif