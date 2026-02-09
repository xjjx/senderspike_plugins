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

//------------------------------------------------------------------------------------
// const
//------------------------------------------------------------------------------------
//knob dimensions
#define SN01_KNOB1_W			80
#define SN01_KNOB2_W			60
#define SN01_KNOB1_Y			20
#define SN01_KNOB2_Y			130
#define SN01_KNOB1_H			SN01_KNOB1_W + SN01_KNOB1_Y
#define SN01_KNOB2_H			SN01_KNOB2_W + SN01_KNOB2_Y
#define SN01_KNOB1_X			135
#define SN01_KNOB2_X			80
#define SN01_KNOB_OFFSET		130
//VU meter size
#define SN01_NEEDL_X			570
#define SN01_NEEDL_Y			20
#define SN01_NEEDL_W			SN01_NEEDL_X + 180
#define SN01_NEEDL_H			SN01_NEEDL_Y + 80
//mode switches
#define SN01_MODES_X			580
#define SN01_MODES_Y			150
#define SN01_MODES_SZ			40
#define SN01_MODES_OFFSET		120
//nr. of image frames
#define SN01_KNOB1_FRAMES		97
#define SN01_KNOB2_FRAMES		97
#define SN01_NEEDL_FRAMES		100

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

//	SignalNoiseGR*		_grdb;	// GR meter
//	CHorizontalSwitch*	_mode;	// mode [flat, A, B]
//	CHorizontalSwitch*	_push;	// 'thrust' [0, +9, +18]
	int				_open;

	virtual std::unique_ptr<SignalNoiseKnobPrecise> setupKnobPrecise(const ParamDesc&, juce::LookAndFeel*);
	virtual std::unique_ptr<SignalNoiseKnob> setupKnob(const ParamDesc&, juce::LookAndFeel*);

public:
	SignalNoiseCompressorGUI(SignalNoiseCompressor&);
	virtual ~SignalNoiseCompressorGUI();

	void paint (juce::Graphics&) override;
	void resized() override;

//	virtual void setParameter(VstInt32 at, float v);
//	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
//	void trackMeter(double dB);
//	void setupMeter(double fs);
};
