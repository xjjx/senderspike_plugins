//------------------------------------------------------------------------------------
//
//	file:		sn_04g.h
//
//	purpose:	SN04 Channel EQ GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------

# pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnobLabel.h"
#include "SignalNoiseKnob.h"
#include "SignalNoiseEqualizer.h"
#include "SignalNoiseSwitchButton.h"
#include "SignalNoiseFilmstripLNF.h"

//------------------------------------------------------------------------------------
//knobs
#define SN04_KNOB_COL_1			40
#define SN04_KNOB_COL_2			120
#define SN04_KNOB_W				80
#define SN04_KNOB2_W			60
#define SN04_KNOB1_FRAMES		101
#define SN04_KNOB2_FRAMES		8
#define SN04_KNOB3_FRAMES		6
//leds
#define SN04_LED_W				28
//text fields
#define SN04_TEXT_W				30
#define SN04_TEXT_H				12
#define IDC_TX_HF_G				1001
#define IDC_TX_HF_W				1002
#define IDC_TX_MF_G				1003
#define IDC_TX_MF_W				1004
#define IDC_TX_LF_G				1005
#define IDC_TX_LF_W				1006
#define IDC_TX_GAIN				1007

//------------------------------------------------------------------------------------

enum band_e { HF = 0, MF, LF, LP, HP };

//------------------------------------------------------------------------------------
// HP/LP circular switch
//------------------------------------------------------------------------------------

/*
class SignalNoiseEqualizerArcSwitch : public CControl
{
private:
	AudioEffect*	_eff;	// associated effect
	float			_rad;	// inscribed circle
public:
//create & destroy
	SignalNoiseEqualizerArcSwitch(const CRect& rc, AudioEffect* eff);
	virtual ~SignalNoiseEqualizerArcSwitch();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void mouse(CDrawContext* ctx, CPoint& pos, long btn = -1);
//vtable
	CLASS_METHODS(SignalNoiseEqualizerArcSwitch, CControl)
};
*/

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseEqualizerGUI : public juce::AudioProcessorEditor,
                                private juce::Timer
{
private:
	SignalNoiseEqualizer& processor;
	void timerCallback() override;

	juce::Image background;

	SignalNoiseKnobLookAndFeel innerKnobLNF;
	SignalNoiseKnobLookAndFeel rimLNF;
	SignalNoiseKnobLookAndFeel gainLNF;
	SignalNoiseKnobLookAndFeel lpfLNF;
	SignalNoiseKnobLookAndFeel hpfLNF;
	SignalNoiseFilmstripLNF octLNF;

//HF
	std::unique_ptr<SignalNoiseKnob>			_hf_f;	// HSF freq
	std::unique_ptr<SignalNoiseKnobPrecise>		_hf_g;	// HSF gain
	std::unique_ptr<SignalNoiseKnobPrecise>		_hf_w;	// HSF bw
	std::unique_ptr<SignalNoiseSwitchButton>	_hf_t;	// HSF type
	std::unique_ptr<SignalNoiseSwitchButton>	_hf_m;	// HSF mode
	std::unique_ptr<SignalNoiseSwitchButton>	_hf_b;	// HSF mute
//	SignalNoiseOnOffLed*			_hfon;	// on/off led
//MF
	std::unique_ptr<SignalNoiseKnob>			_mf_f;	// PKF freq
	std::unique_ptr<SignalNoiseKnobPrecise>		_mf_g;	// PKF gain
	std::unique_ptr<SignalNoiseKnobPrecise>		_mf_w;	// PKF bw
	std::unique_ptr<SignalNoiseSwitchButton>	_mf_t;	// PKF type
	std::unique_ptr<SignalNoiseSwitchButton>	_mf_b;	// PKF mute
//	SignalNoiseOnOffLed*			_mfon;	// on/off led
//LF
	std::unique_ptr<SignalNoiseKnob>			_lf_f;	// LSF freq
	std::unique_ptr<SignalNoiseKnobPrecise>		_lf_g;	// LSF gain
	std::unique_ptr<SignalNoiseKnobPrecise>		_lf_w;	// LSF bw
	std::unique_ptr<SignalNoiseSwitchButton>	_lf_t;	// LSF type
	std::unique_ptr<SignalNoiseSwitchButton>	_lf_m;	// LSF mode
	std::unique_ptr<SignalNoiseSwitchButton>	_lf_b;	// LSF mute
//	SignalNoiseOnOffLed*			_lfon;	// on/off led
//LPF/HPF
	std::unique_ptr<SignalNoiseKnob>			_lpas;	// LPF freq
	std::unique_ptr<SignalNoiseKnob>			_hpas;	// HPF freq
	std::unique_ptr<SignalNoiseKnob>			_loct;	// LPF dB/oct
	std::unique_ptr<SignalNoiseKnob>			_hoct;	// HPF dB/oct
//	SignalNoiseOnOffLed*			_lpon;	// on/off led
//	SignalNoiseOnOffLed*			_hpon;	// on/off led
	std::unique_ptr<SignalNoiseSwitchButton>	_hp_b;	// HPF mute
	std::unique_ptr<SignalNoiseSwitchButton>	_lp_b;	// LPF mute
//output
	std::unique_ptr<SignalNoiseKnobPrecise>		_gain;	// output
	std::unique_ptr<SignalNoiseSwitchButton>	_iphs;	// invert phase
//	SignalNoisePeakLed*				_pkld;	// output peak

// text input
	SignalNoiseKnobLabel						_thfg;	// HF gain text
	SignalNoiseKnobLabel						_thfw;	// HF width text
	SignalNoiseKnobLabel						_tmfg;	// MF gain text
	SignalNoiseKnobLabel						_tmfw;	// MF width text
	SignalNoiseKnobLabel						_tlfg;	// LF gain text
	SignalNoiseKnobLabel						_tlfw;	// LF width text
	SignalNoiseKnobLabel						_tomg;	// output make-up gain text

//"analog" on/off
	std::unique_ptr<SignalNoiseSwitchButton> _mojo;	// analog on/off switch
//number clickers
//	SignalNoiseArcSwitch*			_hsfc;	// HF band
//	SignalNoiseArcSwitch*			_msfc;	// MF band
//	SignalNoiseArcSwitch*			_lsfc;	// LF band
//	SignalNoiseEqualizerArcSwitch*	_hplp;	// HPF/LPF
//global blinking timer
	dword							_time;	// last change time
	bool							_blnk;	// global blink state (sync)
//guard
	int								_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(
		const ParamDesc&,
		juce::LookAndFeel*,
		SignalNoiseKnobLabel& label
	);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);

public:
//create & destroy
	SignalNoiseEqualizerGUI(SignalNoiseEqualizer&);
	virtual ~SignalNoiseEqualizerGUI();

	void paint (juce::Graphics&) override;
	void resized() override;

//runtime - custom
//	void trackPeaks(double A);
//	void setLed(band_e b, bool on);
//	void setBlink(band_e b, bool on);
};
