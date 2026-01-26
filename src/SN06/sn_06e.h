//------------------------------------------------------------------------------------
//
//	file:		sn_06e.h
//
//	purpose:	SN06 op-amp effect
//
//  authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_06E_H
#define _SN_06E_H


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <sn_vsti.h>

//------------------------------------------------------------------------------------
// effect
//------------------------------------------------------------------------------------

enum
{
	SNE_GAIN = 0,	// drive
	SNE_TRIM,		// input volume
	SNE_VOLU,		// output volume
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const param_t gParam[] = 
{
	{"Gain",	"dB",	0.25f},
	{"Input",	"dB",	0.50f},
	{"Volume",	"dB",	0.75f},
};

//------------------------------------------------------------------------------------

#define SN06_VER		1210
#define SN06_UID		'SN06'
#ifdef SN06G
#define SN06_NAM		"SN06-G OpAmp"
#else
#define SN06_NAM		"SN06 OpAmp"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseOpamp : public SignalNoiseFX
{
private:
	double	_norm;
	double	_erfL;
	double	_erfR;
	foHPF	_hpfL;
	foHPF	_hpfR;
private:
//callbacks - SN
	virtual void onSetSampleRate(float fs);
public:
//create & destroy
	SignalNoiseOpamp(audioMasterCallback cb);
	virtual ~SignalNoiseOpamp();
//process - SDK
	virtual void processReplacing(float** in, float** out, VstInt32 sz);
	virtual void processDoubleReplacing(double** in, double** out, VstInt32 sz);
//plugin info - SDK
	VST_DEFINE_PLUGINFO(SN06_NAM, SN06_VER, kPlugCategEffect);
};

//------------------------------------------------------------------------------------


#endif // _SN_06E_H
