//------------------------------------------------------------------------------------
//
//	file:		sn_06g.h
//
//	purpose:	SN06 op-amp GUI
//
//  authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_06G_H
#define _SN_06G_H


//------------------------------------------------------------------------------------

#include <sn_ctrl.h>

//------------------------------------------------------------------------------------
//leds
#define SN06_LED_W				28
//text fields
#define SN06_TEXT_W				30
#define SN06_TEXT_H				14
#define IDC_TX_GAIN				1001
#define IDC_TX_VOLU				1002
#define IDC_TX_TRIM				1003

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------

class SignalNoiseOpampGUI : public AEffGUIEditor, public CControlListener
{
private:
	SignalNoisePeakLed*		_pkld;	// peak
	SignalNoiseKnobP*		_trim;	// input
	SignalNoiseKnobP*		_gain;	// drive
	SignalNoiseKnobP*		_volu;	// volume
	CTextEdit*				_txti;	// input text
	CTextEdit*				_txtg;	// drive text
	CTextEdit*				_txtv;	// volume text
	SignalNoisePeakMeter*	_vupI;	// peak meter 'in'
	SignalNoisePeakMeter*	_vupO;	// peak meter 'out'
	int						_open;
public:
//create & destroy
	SignalNoiseOpampGUI(AudioEffect* effect);
	virtual ~SignalNoiseOpampGUI();
//runtime - from SDK
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(VstInt32 at, float v);
	virtual void valueChanged(CDrawContext* ctx, CControl* ctrl);
//runtime - custom
	void trackMeters(double I, double O);
};

//------------------------------------------------------------------------------------


#endif // _SN_06G_H
