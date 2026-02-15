//------------------------------------------------------------------------------------
//
//	file:		sn_03g.h
//
//	purpose:	SN03 tape recorder emulator GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnob.h"
#include "SignalNoiseTapedeck.h"
#include "SignalNoiseSwitchButton.h"
#include "SignalNoiseFilmstripLNF.h"


//------------------------------------------------------------------------------------
//knobs
#define SN03_KNOB1_W			80
#define SN03_KNOB2_W			60
#define SN03_KNOB1_FRAMES		97
#define SN03_KNOB2_FRAMES		97
//EQ switch
#define SN03_MODES_SZ			40
//VU meter
#define SN03_METER_W			190
#define SN03_METER_H			90
//text fields
#define SN03_TEXT_W				30
#define SN03_TEXT_H				12
#define IDC_TX_TRIM				1001
#define IDC_TX_GAIN				1002

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseTapedeckGUI : public juce::AudioProcessorEditor,
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
	juce::Slider								roomSwitch; // 12, 14, 18, 20
	std::unique_ptr<SignalNoiseSwitchButton>	pathSwitch;	// VU I/O switch
	std::unique_ptr<SignalNoiseSwitchButton>	holdSwitch;	// use peak hold
	std::unique_ptr<SignalNoiseSwitchButton>	hbonSwitch;	// bump on/off
	std::unique_ptr<SignalNoiseSwitchButton>	noisSwitch;	// noise on/off
	std::unique_ptr<SignalNoiseSwitchButton>	loonSwitch;	// force LO on

/*
	CVerticalSwitch*	_attn;	// bump attenuator [0,-1,-2]
	SignalNoiseVU*		_vumt;	// VU meter
	SignalNoisePeakLed*	_peak;	// sample peak led
	CTextEdit*			_txti;	// text input
	CTextEdit*			_txto;	// text output
*/
	int					_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(const ParamDesc&, juce::LookAndFeel*);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);
public:
//create & destroy
	SignalNoiseTapedeckGUI(SignalNoiseTapedeck&);
	virtual ~SignalNoiseTapedeckGUI();

	void paint (juce::Graphics&) override;
	void resized() override;

//runtime - from SDK
//	virtual void setParameter(VstInt32 at, float v);
//	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
//	void trackMeter(double A);
//	void setupMeterLevel(double nl);
//	void setupMeterFilter(double fs);
//	void setupMeterUseHold(bool on);
};
