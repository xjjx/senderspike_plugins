//------------------------------------------------------------------------------------
//
//	file:		sn_05g.h
//
//	purpose:	SN05 limiter GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_05G_H
#define _SN_05G_H


//------------------------------------------------------------------------------------

#include <sn_ctrl.h>

//------------------------------------------------------------------------------------
//text fields
#define SN05_TEXT_W				30
#define SN05_TEXT_H				12
#define IDC_TX_GAIN				1001
#define IDC_TX_CEIL				1002

//------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseLimiterGUI : public AEffGUIEditor, public CControlListener
{
private:
	SignalNoiseLimiterGR*	_grHL;	// GR meter limiter
	SignalNoiseLimiterGR*	_grHC;	// GR meter clipper
	SignalNoiseKnobP*		_gain;	// gain
	SignalNoiseKnobP*		_ceil;	// ceiling
	SignalNoiseKnob*		_hpfc;	// HPF frequency
	SignalNoiseKnob*		_atkh;	// Holters' attack
	SignalNoiseKnob*		_relh;	// Holters' release
	SignalNoiseKnob*		_rels;	// release
	SignalNoiseKnob*		_clip;	// soft clip %
	COnOffButton*			_mode;	// mode switch
	COnOffButton*			_hpon;	// HPF switch
	CTextEdit*				_txtg;	// gain text
	CTextEdit*				_txtc;	// ceil text
	int						_open;
public:
//create & destroy
	SignalNoiseLimiterGUI(AudioEffect* effect);
	virtual ~SignalNoiseLimiterGUI();
//runtime - from SDK
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(VstInt32 at, float v);
	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
	void trackMeter(double lim, double clp);
};

//------------------------------------------------------------------------------------


#endif // _SN_05G_H
