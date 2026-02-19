//------------------------------------------------------------------------------------
//
//	file:		sn_01g.h
//
//	purpose:	SN01 compressor GUI
//
//  authors:	2019 - 2021 Oto Spál
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
	SignalNoiseFilmstripLNF switchLNF;

	std::unique_ptr<SignalNoiseKnobPrecise>	thrsKnob;		// threshold
	std::unique_ptr<SignalNoiseKnob>		funcKnob;		// ratio
	std::unique_ptr<SignalNoiseKnobPrecise>	gainKnob;		// gain
	std::unique_ptr<SignalNoiseKnob>		attkKnob;		// attack
	std::unique_ptr<SignalNoiseKnob>		relsKnob;		// release
	std::unique_ptr<SignalNoiseKnob>		kwdtKnob;		// knee width
	std::unique_ptr<SignalNoiseKnob>		kneeKnob;		// knee strength
	std::unique_ptr<SignalNoiseKnob>		compKnob;		// dry amount
	std::unique_ptr<SignalNoiseSwitchButton> fbckSwitch;	// FF/FB switch

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> thrsAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> funcAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attkAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relsAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kwdtAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> kprcAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> compAttachment;

	std::unique_ptr<SignalNoiseGR>	grMeter; // GR meter
	juce::Slider	modeSlider;	// mode [flat, A, B]
	juce::Slider	pushSlider; // 'thrust' [0, +9, +18]

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> modeAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> pushAttachment;

	int				_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(const ParamDesc&, juce::LookAndFeel*);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);

public:
	SignalNoiseCompressorGUI(SignalNoiseCompressor&);
	virtual ~SignalNoiseCompressorGUI();

	void paint (juce::Graphics&) override;
	void resized() override;
};
