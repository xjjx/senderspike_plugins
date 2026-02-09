//------------------------------------------------------------------------------------
//
//	file:		sn_03g.h
//
//	purpose:	SN03 tape recorder emulator GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_03G_H
#define _SN_03G_H


//------------------------------------------------------------------------------------

#include <sn_ctrl.h>

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

class SignalNoiseTapedeckGUI : public AEffGUIEditor, public CControlListener
{
private:
	SignalNoiseKnobP*	_trim;	// input trim
	SignalNoiseKnobP*	_gain;	// output gain
	SignalNoiseKnob*	_rclo;	// rec lo
	SignalNoiseKnob*	_rchi;	// rec hi
	SignalNoiseKnob*	_rplo;	// rep lo
	SignalNoiseKnob*	_rphi;	// rep hi
	SignalNoiseKnob*	_head;	// head Hz
	SignalNoiseKnob*	_bump;	// head dB
	SignalNoiseKnob*	_hiss;	// hiss dB
	CHorizontalSwitch*	_mode;	// EQ mode [NAB, IEC, AES]
	CHorizontalSwitch*	_room;	// 12, 14, 18, 20
	CHorizontalSwitch*	_hold;	// use peak hold
	CHorizontalSwitch*	_path;	// VU I/O switch
	CVerticalSwitch*	_attn;	// bump attenuator [0,-1,-2]
	CHorizontalSwitch*	_nois;	// noise on/off
	CHorizontalSwitch*	_hbon;	// bump on/off
	COnOffButton*		_loon;	// force LO on
	SignalNoiseVU*		_vumt;	// VU meter
	SignalNoisePeakLed*	_peak;	// sample peak led
	CTextEdit*			_txti;	// text input
	CTextEdit*			_txto;	// text output
	int					_open;
public:
//create & destroy
	SignalNoiseTapedeckGUI(AudioEffect* effect);
	virtual ~SignalNoiseTapedeckGUI();
//runtime - from SDK
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(VstInt32 at, float v);
	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
	void trackMeter(double A);
	void setupMeterLevel(double nl);
	void setupMeterFilter(double fs);
	void setupMeterUseHold(bool on);
};

//------------------------------------------------------------------------------------


#endif // _SN_03G_H
