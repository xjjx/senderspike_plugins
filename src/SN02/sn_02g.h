//------------------------------------------------------------------------------------
//
//	file:		sn_02g.h
//
//	purpose:	SN02 VU meter
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_02G_H
#define _SN_02G_H


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <sn_ctrl.h>
#include <sn_vsti.h>

//------------------------------------------------------------------------------------
// effect
//------------------------------------------------------------------------------------

enum
{
	SNE_ROOM = 0,		// headroom
	SNE_MODE,			// mono, LR, M/S
	SNE_HOLD,			// hold needle (1s)
	SNE_XTND,			// GUI size
	SNE_SIZE,			// num of params
};

//------------------------------------------------------------------------------------

static const param_t gParam[] = 
{
	{"Headroom",	"dB",	0.625f},
	{"Mode",		"",		0.5f},
	{"Hold",		"",		0.0f},
	{"Xtnd",		"",		0.0f},
};

//------------------------------------------------------------------------------------

#define SN02_VER		1210
#define SN02_UID		VST_FOURCC('S','N','0','2')
#define SN02_NAM		"SN02-G VU Meter"

//------------------------------------------------------------------------------------

class SignalNoiseVUMeter : public SignalNoiseFX 
{
private:
	char _name[256];	// instance name
private:
//callbacks - SN
	virtual void onSetSampleRate(float fs);
	virtual void onSetParameter(VstInt32 at, float v);
public:
//create & destroy
	SignalNoiseVUMeter(audioMasterCallback cb);
	virtual ~SignalNoiseVUMeter();
//access
	char* getInstName() { return _name; }
//vtable - SDK
	virtual VstInt32 getChunk(void** data, bool preset);
	virtual VstInt32 setChunk(void* data, VstInt32 sz, bool preset);
	virtual void processReplacing(float** in, float** out, VstInt32 sz);
	virtual void processDoubleReplacing(double** in, double** out, VstInt32 sz);
//plugin info - SDK
	VST_DEFINE_PLUGINFO(SN02_NAM, SN02_VER, kPlugCategAnalysis);
};

//------------------------------------------------------------------------------------
// editor
//------------------------------------------------------------------------------------
//normal GUI
#define SN02_Ws					250
#define SN02_Hs					303
#define SN02_METER_Ws			190
#define SN02_METER_Hs			90
#define SN02_METER_Ys			53
#define SN02_METER_OffS			130
#define SN02_TAPE_Ws			210
#define SN02_TAPE_Hs			20
//xtended GUI
#define SN02_METER_Wx			280
#define SN02_METER_Hx			130
#define SN02_METER_Yx			60
#define SN02_METER_OffX			180
#define SN02_TAPE_Wx			300
#define SN02_TAPE_Hx			30
//switches
#define SN02_SWITCH_Y			1
#define SN02_SWITCH_H			20
//buttons
#define SN02_XTND_X				70
#define SN02_XTND_W				50
#define SN02_XTND_H				SN02_SWITCH_H
#define SN02_HOLD_X				43
#define SN02_HOLD_Y				SN02_SWITCH_Y
#define SN02_HOLD_W				40
#define SN02_HOLD_H				SN02_SWITCH_H

//------------------------------------------------------------------------------------

#define IDC_NAME				1001
#define IDR_FONT_NAME			"jr!hand"

//------------------------------------------------------------------------------------

class SignalNoiseVUMeterGUI : public AEffGUIEditor, public IControlListener
{
private:
	SignalNoiseVU*			_vucL;	// VU meter L (top)
	SignalNoiseVU*			_vucR;	// VU meter R (btm)
	CHorizontalSwitch*		_room;	// 12, 14, 18, 20
	CHorizontalSwitch*		_mode;	// mono, LR, M/S
	COnOffButton*			_hold;	// use peak hold
	COnOffButton*			_xtnd;	// GUI size switch
	SignalNoiseUserLabel*	_name;	// instance name
	SignalNoisePeakLed*		_ledL;	// sample peak L
	SignalNoisePeakLed*		_ledR;	// sample peak R
	int						_open;
public:
//create & destroy
	SignalNoiseVUMeterGUI(AudioEffect* effect);
	virtual ~SignalNoiseVUMeterGUI();
//runtime - from SDK
	virtual bool open(void* ptr);
	virtual void close();
	virtual void setParameter(VstInt32 at, float v);
	virtual void valueChanged(CControl* ctrl);
//runtime - custom
	void trackMeter(double L, double R);
	void setupMeterLevel(double nl);
	void setupMeterFilter(double fs);
	void setupMeterUseHold(bool on);
	void setInstName(char* txt);
	void toggleUserInterface();
};

//------------------------------------------------------------------------------------


#endif // _SN_02G_H
