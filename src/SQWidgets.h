#pragma once

#include "Rack.hpp"
#include "WidgetComposite.h"

#include <functional>


/**
 * Like Trimpot, but with blue stripe
 */
struct BlueTrimmer : SVGKnob {
	BlueTrimmer() {
		minAngle = -0.75*M_PI;
		maxAngle = 0.75*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/BlueTrimmer.svg")));
	}
};

/**
 * Like Rogan1PSBlue, but smaller.
 */
struct Blue30Knob : SVGKnob {
	Blue30Knob() {    
		minAngle = -0.83*M_PI;
		maxAngle = 0.83*M_PI;
		setSVG(SVG::load(assetPlugin(plugin, "res/Blue30.svg")));
	}
};


/**
 * A very basic momentary push button.
 */
struct SQPush : SVGButton
{
    SQPush()
    {
        setSVGs(
            SVG::load(assetPlugin(plugin, "res/BluePush_0.svg")),
            SVG::load(assetPlugin(plugin, "res/BluePush_1.svg"))
          //  SVG::load(assetGlobal("res/ComponentLibrary/BefacoPush_0.svg")),
          //  SVG::load(assetGlobal("res/ComponentLibrary/BefacoPush_1.svg"))
        );
    }

    void onDragEnd(EventDragEnd &e) override
    {
        SVGButton::onDragEnd(e);
        if (clickHandler) {
            clickHandler();
        }
    }

    /**
     * User of button passes in a callback lamba here
     */
    void onClick(std::function<void(void)> callback)
    {
        clickHandler = callback;
    }

    std::function<void(void)> clickHandler;
};