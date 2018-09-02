#pragma once

#include "Rack.hpp"
#include "WidgetComposite.h"

struct BlueTrimmer : SVGKnob {
	BlueTrimmer() {
		minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/BlueTrimmer.svg")));
	}
};

struct Blue30Knob : SVGKnob {
	Blue30Knob() {    
		minAngle = -0.83*M_PI;
		maxAngle = 0.83*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/Blue30.svg")));
	}
};