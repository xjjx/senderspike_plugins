//------------------------------------------------------------------------------------
//
//	file:		sn_01g.h
//
//	purpose:	SN01 compressor GUI
//
//  authors:	2019 - 2021 Oto Spál
//				2026 -      Pawel ,Xj' Piatek (JUCE port)
//
//------------------------------------------------------------------------------------

# pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnob.h"
#include "SignalNoiseCompressor.h"
#include "SignalNoiseSwitchButton.h"
#include "SignalNoiseFilmstripLNF.h"
#include "SignalNoiseGR.h"

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseCompressorGUI : public juce::AudioProcessorEditor,
                                 private juce::Timer
{
private:
	SignalNoiseCompressor& processor;
	void timerCallback() override;

	juce::Image background;

	SignalNoiseKnobLookAndFeel largeLNF;
	SignalNoiseKnobLookAndFeel normalLNF;
	SignalNoiseKnobLookAndFeel screwLNF;
	SignalNoiseKnobLookAndFeel smallScrewLNF;
	SignalNoiseFilmstripLNF switchLNF;

	std::unique_ptr<SignalNoiseKnobPrecise>	thrsKnob;		// threshold
	std::unique_ptr<SignalNoiseKnob>		funcKnob;		// ratio
	std::unique_ptr<SignalNoiseKnobPrecise>	gainKnob;		// gain
	std::unique_ptr<SignalNoiseKnob>		attkKnob;		// attack
	std::unique_ptr<SignalNoiseKnob>		relsKnob;		// release
	std::unique_ptr<SignalNoiseKnob>		kwdtKnob;		// knee width
	std::unique_ptr<SignalNoiseKnob>		kneeKnob;		// knee strength
	std::unique_ptr<SignalNoiseKnob>		compKnob;		// dry amount
	std::unique_ptr<SignalNoiseKnob>		linkKnob;		// link amount
	std::unique_ptr<SignalNoiseSwitchButton> fbckSwitch;	// FF/FB switch

	std::unique_ptr<SignalNoiseGR>	grMeter; // GR meter
	juce::Slider	modeSlider;	// mode [flat, A, B]
	juce::Slider	pushSlider;	// 'thrust' [0, +9, +18]
	juce::Slider	linkSlider;	// Detector Max/Avg/Power switch

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modeAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pushAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> linkAttachment;

	int				_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(const ParamDesc&, juce::LookAndFeel*);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);
	virtual void setup3waySwitch(
		juce::Slider* slider,
		const ParamDesc& p,
		juce::LookAndFeel* lnf,
		std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment
	);

public:
	SignalNoiseCompressorGUI(SignalNoiseCompressor&);
	virtual ~SignalNoiseCompressorGUI();

	void paint (juce::Graphics&) override;
	void resized() override;
};
