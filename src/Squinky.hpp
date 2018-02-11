#include "rack.hpp"


using namespace rack;


extern Plugin *plugin;

////////////////////
// module widgets
////////////////////

// first module widget is simple freq shifter.
struct BootyWidget : ModuleWidget {
	BootyWidget();
};