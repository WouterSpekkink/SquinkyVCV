//#include "Befaco.hpp"

#if defined(_MSC_VER)
#define __attribute__(x)
#endif

#include "dsp/minblep.hpp"
#include "dsp/filter.hpp"
#include "AudioMath.h"

using namespace rack;

#define _EVEN
/**
 * Before optimization, cpu = 43
 * turn off tri = 42
 * turn off tri and even, 38
 * turn off tri, even, and sin 21
 * turn off tri, even, sin, sqr, saw, 17
 * everything off and no exp, 3
 * even only, no exp or sin 6.7
 */
template <class TBase>
struct EvenVCO : TBase {
	enum ParamIds {
		OCTAVE_PARAM,
		TUNE_PARAM,
		PWM_PARAM,
		NUM_PARAMS
	};
	enum InputIds {
		PITCH1_INPUT,
		PITCH2_INPUT,
		FM_INPUT,
		SYNC_INPUT,
		PWM_INPUT,
		NUM_INPUTS
	};
	enum OutputIds {
		TRI_OUTPUT,
		SINE_OUTPUT,
		EVEN_OUTPUT,
		SAW_OUTPUT,
		SQUARE_OUTPUT,
		NUM_OUTPUTS
	};

	float phase = 0.0;
	/** The value of the last sync input */
	float sync = 0.0;
	/** The outputs */
#ifdef _TRI
	float tri = 0.0;
#endif
	/** Whether we are past the pulse width already */
	bool halfPhase = false;

	MinBLEP<16> triSquareMinBLEP;
#ifdef _TRI
    MinBLEP<16> triMinBLEP;
#endif
#ifdef _SIN
    MinBLEP<16> sineMinBLEP;
#endif
#ifdef _EVEN
    MinBLEP<16> doubleSawMinBLEP;
#endif
#ifdef _SAW
    MinBLEP<16> sawMinBLEP;
#endif
#ifdef _SQ
    MinBLEP<16> squareMinBLEP;
#endif

	//RCFilter triFilter;

	EvenVCO();
	void step() override;
};

template <class TBase>
//EvenVCO<TBase>::EvenVCO() : TBase(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS) {
 EvenVCO<TBase>::EvenVCO() : TBase() {
	triSquareMinBLEP.minblep = rack::minblep_16_32;
	triSquareMinBLEP.oversample = 32;
#ifdef _TRI
	triMinBLEP.minblep = minblep_16_32;
	triMinBLEP.oversample = 32;
#endif
#ifdef _SIN
	sineMinBLEP.minblep = minblep_16_32;
	sineMinBLEP.oversample = 32;
#endif
#ifdef _EVEN
	doubleSawMinBLEP.minblep = minblep_16_32;
	doubleSawMinBLEP.oversample = 32;
#endif
#ifdef _SAW
	sawMinBLEP.minblep = minblep_16_32;
	sawMinBLEP.oversample = 32;
#endif
#ifdef _SQ
	squareMinBLEP.minblep = minblep_16_32;
	squareMinBLEP.oversample = 32;
#endif
}

template <class TBase>
void EvenVCO<TBase>::step() {
	// Compute frequency, pitch is 1V/oct
	float pitch = 1.0 + roundf(TBase::params[OCTAVE_PARAM].value) + TBase::params[TUNE_PARAM].value / 12.0;
	pitch += TBase::inputs[PITCH1_INPUT].value + TBase::inputs[PITCH2_INPUT].value;
	pitch += TBase::inputs[FM_INPUT].value / 4.0;
//	float freq = 261.626 * powf(2.0, pitch);
    float freq = 523;
	freq = clamp(freq, 0.0f, 20000.0f);
   // printf("pitch = %f, freq = %f\n", pitch, freq);

	// Pulse width
	float pw = TBase::params[PWM_PARAM].value + TBase::inputs[PWM_INPUT].value / 5.0;
	const float minPw = 0.05;
	pw = rescale(clamp(pw, -1.0f, 1.0f), -1.0f, 1.0f, minPw, 1.0f - minPw);

	// Advance phase
	float deltaPhase = clamp(freq * TBase::engineGetSampleTime(), 1e-6f, 0.5f);
	float oldPhase = phase;
	phase += deltaPhase;

	if (oldPhase < 0.5 && phase >= 0.5) {
       // printf("doing blep\n");
		float crossing = -(phase - 0.5) / deltaPhase;
		triSquareMinBLEP.jump(crossing, 2.0);
#ifdef _EVEN
		doubleSawMinBLEP.jump(crossing, -2.0);
#endif
	}

	if (!halfPhase && phase >= pw) {
#ifdef _SQ
		float crossing  = -(phase - pw) / deltaPhase;
		squareMinBLEP.jump(crossing, 2.0);
#endif
		halfPhase = true;
	}

	// Reset phase if at end of cycle
	if (phase >= 1.0) {
		phase -= 1.0;
		float crossing = -phase / deltaPhase;
		triSquareMinBLEP.jump(crossing, -2.0);
#ifdef _EVEN
		doubleSawMinBLEP.jump(crossing, -2.0);
#endif
#ifdef _SQ
		squareMinBLEP.jump(crossing, -2.0);
		sawMinBLEP.jump(crossing, -2.0);
#endif
		halfPhase = false;
	}

	// Outputs
#ifdef _TRI
	float triSquare = (phase < 0.5) ? -1.0 : 1.0;
	triSquare += triSquareMinBLEP.shift();

	// Integrate square for triangle
	tri += 4.0 * triSquare * freq * TBase::engineGetSampleTime();
	tri *= (1.0 - 40.0 * TBase::engineGetSampleTime());
#endif

#ifdef _SIN
	float sine = -cosf(2*AudioMath::Pi * phase);
#else
    float sine = 0;
#endif
	float doubleSaw = (phase < 0.5) ? (-1.0 + 4.0*phase) : (-1.0 + 4.0*(phase - 0.5));
#ifdef _EVEN
	doubleSaw += doubleSawMinBLEP.shift();
	float even = 0.55 * (doubleSaw + 1.27 * sine);
#endif
#ifdef _SAW
	float saw = -1.0 + 2.0*phase;
	saw += sawMinBLEP.shift();
#endif
#ifdef _SQ
	float square = (phase < pw) ? -1.0 : 1.0;
	square += squareMinBLEP.shift();
#endif

	// Set outputs
#ifdef _TRI
    TBase::outputs[TRI_OUTPUT].value = 5.0*tri;
#endif
#ifdef _SIN
    TBase::outputs[SINE_OUTPUT].value = 5.0*sine;
#endif
#ifdef _EVEN
    TBase::outputs[EVEN_OUTPUT].value = 5.0*even;
#endif
#ifdef _SAW
    TBase::outputs[SAW_OUTPUT].value = 5.0*saw;
#endif
#ifdef _SQ
    TBase::outputs[SQUARE_OUTPUT].value = 5.0*square;
#endif
}

#if 0
struct EvenVCOWidget : ModuleWidget {
	EvenVCOWidget(EvenVCO *module) : ModuleWidget(module) {
		setPanel(SVG::load(assetPlugin(plugin, "res/EvenVCO.svg")));

		addChild(Widget::create<Knurlie>(Vec(15, 0)));
		addChild(Widget::create<Knurlie>(Vec(15, 365)));
		addChild(Widget::create<Knurlie>(Vec(15*6, 0)));
		addChild(Widget::create<Knurlie>(Vec(15*6, 365)));

		addParam(ParamWidget::create<BefacoBigSnapKnob>(Vec(22, 32), module, EvenVCO::OCTAVE_PARAM, -5.0, 4.0, 0.0));
		addParam(ParamWidget::create<BefacoTinyKnob>(Vec(73, 131), module, EvenVCO::TUNE_PARAM, -7.0, 7.0, 0.0));
		addParam(ParamWidget::create<Davies1900hRedKnob>(Vec(16, 230), module, EvenVCO::PWM_PARAM, -1.0, 1.0, 0.0));

		addInput(Port::create<PJ301MPort>(Vec(8, 120), Port::INPUT, module, EvenVCO::PITCH1_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(19, 157), Port::INPUT, module, EvenVCO::PITCH2_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(48, 183), Port::INPUT, module, EvenVCO::FM_INPUT));
		addInput(Port::create<PJ301MPort>(Vec(86, 189), Port::INPUT, module, EvenVCO::SYNC_INPUT));

		addInput(Port::create<PJ301MPort>(Vec(72, 236), Port::INPUT, module, EvenVCO::PWM_INPUT));

		addOutput(Port::create<PJ301MPort>(Vec(10, 283), Port::OUTPUT, module, EvenVCO::TRI_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(87, 283), Port::OUTPUT, module, EvenVCO::SINE_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(48, 306), Port::OUTPUT, module, EvenVCO::EVEN_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(10, 327), Port::OUTPUT, module, EvenVCO::SAW_OUTPUT));
		addOutput(Port::create<PJ301MPort>(Vec(87, 327), Port::OUTPUT, module, EvenVCO::SQUARE_OUTPUT));
	}
};


Model *modelEvenVCO = Model::create<EvenVCO, EvenVCOWidget>("Befaco", "EvenVCO", "EvenVCO", OSCILLATOR_TAG);
#endif