//------------------------------------------------------------------------------------
//
//	file:		sn_03e.h
//
//	purpose:	SN03 tape recorder emulator effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_03E_H
#define _SN_03E_H


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <sn_vsti.h>

//------------------------------------------------------------------------------------

enum
{
	SNE_TRIM,		// input trim +/-24dB
	SNE_GAIN,		// output gain +/-24dB
	SNE_EQSC,		// EQ curve [NAB, IEC 15, AES]
	SNE_RCLO,		// rec EQ - bass +/-10dB
	SNE_RCHI,		// rec EQ - high +/-14dB
	SNE_RPLO,		// rep EQ - bass +/-10dB
	SNE_RPHI,		// rep EQ - high +/-14dB
	SNE_HEAD,		// head bump frequency
	SNE_BUMP,		// head bump magitude
	SNE_HISS,		// hiss gain
#ifdef SN03G
	SNE_ROOM,		// VU meter headroom
	SNE_HOLD,		// VU meter peak hold
	SNE_PATH,		// VU meter path
#endif
	SNE_ATTN,		// bump attenuator
	SNE_NOIS,		// noise on/off
	SNE_HBON,		// head bump on/off
	SNE_LOON,		// force LO on
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const param_t gParam[] = 
{
	{"Input",		"dB",	0.50f},
	{"Output",		"dB",	0.50f},
	{"EQ Curve",	"Typ",	0.50f},
	{"EQ RecLo",	"dB",	0.50f},
	{"EQ RecHi",	"dB",	0.50f},
	{"EQ RepLo",	"dB",	0.50f},
	{"EQ RepHi",	"dB",	0.50f},
	{"HB Freq",		"Hz",	0.00f},
	{"HB Strn",		"dB",	0.00f},
	{"Hiss dB",		"dB",	0.50f},
#ifdef SN03G
	{"VU Ref",		"dB",	0.625f},
	{"VU Hold",		"n/y",	0.0f},
	{"VU Path",		"I/O",	1.0f},
#endif
	{"HB Attn",		"dB",	1.00f},
	{"Hiss On",		"n/y",	1.00f},
	{"HB On",		"n/y",	1.00f},
	{"Anarchy",		"n/y",	0.00f},
};

//------------------------------------------------------------------------------------

#define SN03_VER		1310
#define SN03_UID		VST_FOURCC('S','N','0','3')
#ifdef SN03G
#define SN03_NAM		"SN03-G Tape Recorder"
#else
#define SN03_NAM		"SN03 Tape Recorder"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseTapedeck : public SignalNoiseFX 
{
private:
//repro head
	foHPF	_rep0L;		// repro - LF roll-off
	foHPF	_rep0R;		// repro - LF roll-off
	biquad	_rep1L;		// repro - bump
	biquad	_rep1R;		// repro - bump
//record EQ
	foLSF	_pre1L;		// pre-emphasis LF
	foLSF	_pre1R;		// pre-emphasis LF
	foHSF	_pre2L;		// pre-emphasis HF
	foHSF	_pre2R;		// pre-emphasis HF
//repro EQ
	foLSF	_dee1L;		// de-emphasis LF
	foLSF	_dee1R;		// de-emphasis LF
	foHSF	_dee2L;		// de-emphasis HF
	foHSF	_dee2R;		// de-emphasis HF
//other
	noise	_rand;		// noise generator
private:
	void setupTapeheads();
	void setupEqualizer();
//callbacks - SN
	virtual void onSetSampleRate(float fs);
	virtual void onSetParameter(VstInt32 at, float v);
public:
//create & destroy
	SignalNoiseTapedeck(audioMasterCallback cb);
	virtual ~SignalNoiseTapedeck();
//process - SDK
	virtual void processReplacing(float** in, float** out, VstInt32 sz);
	virtual void processDoubleReplacing(double** in, double** out, VstInt32 sz);
//plugin info - SDK
	VST_DEFINE_PLUGINFO(SN03_NAM, SN03_VER, kPlugCategEffect);
};

//------------------------------------------------------------------------------------


#endif // _SN_03E_H
