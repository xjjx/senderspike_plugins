//------------------------------------------------------------------------------------
//
//	file:		sn_01g.h
//
//	purpose:	SN01 compressor GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_01G_H
#define _SN_01G_H


//------------------------------------------------------------------------------------

#include <sn_ctrl.h>

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

class SignalNoiseCompressorGUI : public AEffGUIEditor, public CControlListener
{
private:
	CAnimKnob*			_thrs;	// threshold
	CAnimKnob*			_func;	// ratio
	CAnimKnob*			_gain;	// gain
	CAnimKnob*			_attk;	// attack
	CAnimKnob*			_rels;	// release
	CAnimKnob*			_kwdt;	// knee width
	CAnimKnob*			_kprc;	// knee strength
	CAnimKnob*			_comp;	// dry amount
	COnOffButton*		_fbck;	// FF/FB switch
	SignalNoiseGR*		_grdb;	// GR meter
	CHorizontalSwitch*	_mode;	// mode [flat, A, B]
	CHorizontalSwitch*	_push;	// 'thrust' [0, +9, +18]
	int					_open;
public:
//create & destroy
	SignalNoiseCompressorGUI(AudioEffect* effect);
	virtual ~SignalNoiseCompressorGUI();
//runtime - from SDK
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(VstInt32 at, float v);
	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
	void trackMeter(double dB);
	void setupMeter(double fs);
};

//------------------------------------------------------------------------------------


#endif // _SN_01G_H
