//------------------------------------------------------------------------------------
//
//	file:		sn_03g.h
//
//	purpose:	SN03 tape recorder emulator GUI
//
//  authors:	2019 - 2021 Oto Spál
//				2026 -      Pawel ,Xj' Piatek (JUCE port)
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnobLabel.h"
#include "SignalNoiseKnob.h"
#include "SignalNoiseTapedeck.h"
#include "SignalNoiseSwitchButton.h"
#include "SignalNoiseFilmstripLNF.h"
#include "SignalNoiseVU.h"


//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseTapedeckGUI : public juce::AudioProcessorEditor,
                               public juce::AudioProcessorValueTreeState::Listener,
                               private juce::Timer
{
private:
	SignalNoiseTapedeck& processor;
	void timerCallback() override;

	juce::Image background;

	SignalNoiseKnobLookAndFeel largeLNF;
	SignalNoiseKnobLookAndFeel normalLNF;
	SignalNoiseFilmstripLNF roomLNF;
	SignalNoiseFilmstripLNF eqscLNF;
	SignalNoiseFilmstripLNF attnLNF;

	std::unique_ptr<SignalNoiseKnobPrecise>	trimKnob;   // input trim
	std::unique_ptr<SignalNoiseKnobPrecise>	gainKnob;   // output gain
	std::unique_ptr<SignalNoiseKnob>		rcloKnob;   // rec lo
	std::unique_ptr<SignalNoiseKnob>		rchiKnob;   // rec hi
	std::unique_ptr<SignalNoiseKnob>		rploKnob;   // rep lo
	std::unique_ptr<SignalNoiseKnob>		rphiKnob;   // rep hi
	std::unique_ptr<SignalNoiseKnob>		headKnob;   // head Hz
	std::unique_ptr<SignalNoiseKnob>		bumpKnob;   // head dB
	std::unique_ptr<SignalNoiseKnob>		hissKnob;   // hiss dB

	juce::Slider								eqscSwitch;	// EQ mode [NAB, IEC, AES]
	juce::Slider								roomSwitch;	// 12, 14, 18, 20
	juce::Slider								attnSwitch;	// bump attenuator [0,-1,-2]
	std::unique_ptr<SignalNoiseSwitchButton>	pathSwitch;	// VU I/O switch
	std::unique_ptr<SignalNoiseSwitchButton>	holdSwitch;	// use peak hold
	std::unique_ptr<SignalNoiseSwitchButton>	hbonSwitch;	// bump on/off
	std::unique_ptr<SignalNoiseSwitchButton>	noisSwitch;	// noise on/off
	std::unique_ptr<SignalNoiseSwitchButton>	loonSwitch;	// force LO on

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> eqscAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> roomAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attnAttachment;

	std::unique_ptr<SignalNoiseVU>				vuMeter;	// VU meter
/*
	SignalNoisePeakLed*	_peak;	// sample peak led
*/
    SignalNoiseKnobLabel trimLabel;
    SignalNoiseKnobLabel gainLabel;

	int					_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(const ParamDesc&, juce::LookAndFeel*);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);
public:
//create & destroy
	SignalNoiseTapedeckGUI(SignalNoiseTapedeck&);
	virtual ~SignalNoiseTapedeckGUI();

	void paint (juce::Graphics&) override;
	void resized() override;
	void parameterChanged(const juce::String& parameterID, float newValue) override;
};
