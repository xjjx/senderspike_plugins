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
#include "SignalNoiseLimiter.h"
#include "SignalNoiseSwitchButton.h"

//------------------------------------------------------------------------------------
//text fields
#define SN05_TEXT_W				30
#define SN05_TEXT_H				12
#define IDC_TX_GAIN				1001
#define IDC_TX_CEIL				1002

//------------------------------------------------------------------------------------
/*
class SignalNoiseLimiterGR : public CControl
{
private:
	double		_val;	// current value
	CBitmap*	_map;	// bitmap
public:
//create & destroy
	SignalNoiseLimiterGR(const CRect& rc, CBitmap* map, float fs);
	virtual ~SignalNoiseLimiterGR();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setVal(double dB);
//vtable
	CLASS_METHODS(SignalNoiseLimiterGR, CControl)
};
*/
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

//	SignalNoiseLimiterGR*	_grHL;	// GR meter limiter
//	SignalNoiseLimiterGR*	_grHC;	// GR meter clipper
//	SignalNoiseKnobP*		_gain;	// gain
//	SignalNoiseKnobP*		_ceil;	// ceiling

	SignalNoiseKnobLookAndFeel largeLNF;
	SignalNoiseKnobLookAndFeel normalLNF;

	std::unique_ptr<SignalNoiseKnobPrecise> gainKnob;
	std::unique_ptr<SignalNoiseKnobPrecise> ceilKnob;

	juce::Slider		hpfcKnob;	// HPF frequency
	juce::Slider		atkhKnob;	// Holters' attack
	juce::Slider		relhKnob;	// Holters' release
	juce::Slider		relsKnob;	// release
	juce::Slider		clipKnob;	// soft clip %
	std::unique_ptr<SignalNoiseSwitchButton> modeSwitch;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> hpfcAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> atkhAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relhAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> relsAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> clipAttachment;

	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> modeAttachment;

//	COnOffButton*			_mode;	// mode switch
//	COnOffButton*			_hpon;	// HPF switch
//	CTextEdit*				_txtg;	// gain text
//	CTextEdit*				_txtc;	// ceil text
	int						_open;
public:
//create & destroy
	SignalNoiseLimiterGUI(SignalNoiseLimiter&);
	virtual ~SignalNoiseLimiterGUI();

	void paint (juce::Graphics&) override;
	void resized() override;

//runtime - from SDK
//	virtual void setParameter(VstInt32 at, float v);
//	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
//	void trackMeter(double lim, double clp);
};
