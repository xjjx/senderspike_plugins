//------------------------------------------------------------------------------------
//
//	file:		sn_05g.h
//
//	purpose:	SN05 limiter GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnobLabel.h"
#include "SignalNoiseKnob.h"
#include "SignalNoiseLimiter.h"
#include "SignalNoiseSwitchButton.h"
#include "SignalNoiseLimiterGR.h"

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseLimiterGUI : public juce::AudioProcessorEditor,
                              private juce::Timer
{
private:
	SignalNoiseLimiter& processor;
	void timerCallback() override;

	juce::Image background;

	std::unique_ptr<SignalNoiseLimiterGR>	meterLimiter;	// GR meter limiter
	std::unique_ptr<SignalNoiseLimiterGR>	meterClipper;	// GR meter clipper

	SignalNoiseKnobLookAndFeel largeLNF;
	SignalNoiseKnobLookAndFeel normalLNF;

	std::unique_ptr<SignalNoiseKnobPrecise>	gainKnob;
	std::unique_ptr<SignalNoiseKnobPrecise>	ceilKnob;

	std::unique_ptr<SignalNoiseKnob>		hpfcKnob;	// HPF frequency
	std::unique_ptr<SignalNoiseKnob>		atkhKnob;	// Holters' attack
	std::unique_ptr<SignalNoiseKnob>		relhKnob;	// Holters' release
	std::unique_ptr<SignalNoiseKnob>		relsKnob;	// release
	std::unique_ptr<SignalNoiseKnob>		clipKnob;	// soft clip %
	std::unique_ptr<SignalNoiseSwitchButton> modeSwitch;
	std::unique_ptr<SignalNoiseSwitchButton> hponSwitch;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfcAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> atkhAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relhAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relsAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clipAttachment;

    SignalNoiseKnobLabel gainLabel;
    SignalNoiseKnobLabel ceilLabel;
	int						_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(
		const ParamDesc&,
		juce::LookAndFeel*,
		SignalNoiseKnobLabel& label
	);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);

public:
	SignalNoiseLimiterGUI(SignalNoiseLimiter&);
	virtual ~SignalNoiseLimiterGUI();

	void paint (juce::Graphics&) override;
	void resized() override;
};
