//------------------------------------------------------------------------------------
//
//	file:		sn_04e.h
//
//	purpose:	SN04 Channel EQ effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_04E_H
#define _SN_04E_H


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <sn_vsti.h>

//------------------------------------------------------------------------------------
// effect
//------------------------------------------------------------------------------------

enum
{
	SNE_GAIN = 0,	// output
	SNE_IPHS,		// phase invert
	SNE_HF_F,		// HF freq
	SNE_HF_G,		// HF gain
	SNE_HF_Q,		// HF width
	SNE_HF_T,		// HF curve type
	SNE_HF_M,		// HF mode [HSF, PKF]
	SNE_MF_F,		// MF freq
	SNE_MF_G,		// MF gain
	SNE_MF_Q,		// MF width
	SNE_MF_T,		// MF curve type
	SNE_LF_F,		// LF freq
	SNE_LF_G,		// LF gain
	SNE_LF_Q,		// LF width
	SNE_LF_T,		// LF curve type
	SNE_LF_M,		// LF mode [LSF, PKF]
	SNE_LPAS,		// low pass
	SNE_HPAS,		// high pass
	SNE_LOCT,		// low pass dB/oct [6,12,18,24]
	SNE_HOCT,		// high pass dB/oct [6,12,18,24]
	SNE_MOJO,		// "analog"
	SNE_HF_B,		// bypass HF
	SNE_MF_B,		// bypass MF
	SNE_LF_B,		// bypass LF
	SNE_LP_B,		// bypass LPF
	SNE_HP_B,		// bypass HPF
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const param_t gParam[] = 
{
	{"Output",		"dB",		0.50f},
	{"Phase",		"n/y",		0.00f},
	{"HF Freq",		"Hz",		0.00f},
	{"HF Gain",		"dB",		0.50f},
	{"HF BW",		"oct",		0.50f},
	{"HF Type",		"typ",		0.00f},
	{"HF Mode",		"n/y",		0.00f},
	{"MF Freq",		"Hz",		0.00f},
	{"MF Gain",		"dB",		0.50f},
	{"MF BW",		"oct",		0.50f},
	{"MF Type",		"typ",		0.00f},
	{"LF Freq",		"Hz",		0.00f},
	{"LF Gain",		"dB",		0.50f},
	{"LF BW",		"oct",		0.50f},
	{"LF Type",		"typ",		0.00f},
	{"LF Mode",		"n/y",		0.00f},
	{"Lo-Pass",		"Hz",		0.00f},
	{"Hi-Pass",		"Hz",		0.00f},
	{"Lo Slope",	"dB/o",		0.00f},
	{"Hi Slope",	"dB/o",		0.00f},
	{"Analog",		"n/y",		0.00f},
	{"Mute HF",		"n/y",		0.00f},
	{"Mute MF",		"n/y",		0.00f},
	{"Mute LF",		"n/y",		0.00f},
	{"Mute LPF",	"n/y",		0.00f},
	{"Mute HPF",	"n/y",		0.00f},
};

//------------------------------------------------------------------------------------

#define SN04_VER		1210
#define SN04_UID		'SN04'
#ifdef SN04G
#define SN04_NAM		"SN04-G Channel EQ"
#else
#define SN04_NAM		"SN04 Channel EQ"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseEqualizer : public SignalNoiseFX 
{
private:
//bands		   L        R
	biquad	_hf_La,  _hf_Ra;
	foHSF	_hf_Lb,  _hf_Rb;
	biquad	_mf_Lp,  _mf_Rp;
	biquad	_lf_La,  _lf_Ra;
	biquad	_lf_Lb,  _lf_Rb;
	foLSF	_lf_Lc,  _lf_Rc;
//LPF
	foLPF	_lp06_L, _lp06_R;
	biquad	_lp12_L, _lp12_R;
	biquad	_lp24_L, _lp24_R;
//HPF
	foHPF	_hp06_L, _hp06_R;
	biquad	_hp12_L, _hp12_R;
	biquad	_hp24_L, _hp24_R;
//character
	foHSF	_hsfL,   _hsfR;
//other
	noise	_mojo;
	double	_norm;
private:
	void setupHF();
	void setupMF();
	void setupLF();
	void setupLP();
	void setupHP();
	void setupAnalog();
//callbacks - SN
	virtual void onSetSampleRate(float fs);
	virtual void onSetParameter(VstInt32 at, float v);
public:
//create & destroy
	SignalNoiseEqualizer(audioMasterCallback cb);
	virtual ~SignalNoiseEqualizer();
//process - SDK
	virtual void processReplacing(float** in, float** out, VstInt32 sz);
	virtual void processDoubleReplacing(double** in, double** out, VstInt32 sz);
//plugin info - SDK
	VST_DEFINE_PLUGINFO(SN04_NAM, SN04_VER, kPlugCategEffect);
};

//------------------------------------------------------------------------------------


#endif // _SN_04E_H
