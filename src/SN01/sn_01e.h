//------------------------------------------------------------------------------------
//
//	file:		sn_01e.h
//
//	purpose:	SN01 compressor effect
//
//  authors:	2019 - 2026 Oto Spál
//				based on (c) 2012 D. Giannoulis, M. Massberg, and Joshua D. Reiss
//				'Digital Dynamic Range Compressor Design—A Tutorial and Analysis'
//				original paper @ https://www.eecs.qmul.ac.uk/~josh/documents/2012/
//				GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
//
//------------------------------------------------------------------------------------


#ifndef _SN_01E_H
#define _SN_01E_H


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <sn_vsti.h>

//------------------------------------------------------------------------------------

enum
{
	SNE_TRSH = 0,	// threshold
	SNE_FUNC,		// ratio
	SNE_GAIN,		// make-up
	SNE_ATTK,		// attack
	SNE_RELS,		// release
	SNE_KWDT,		// knee width
	SNE_KNEE,		// knee strength
	SNE_MODE,		// SC mode
	SNE_PUSH,		// 'thrust'
	SNE_COMP,		// dry/wet
	SNE_FBCK,		// FF/FB switch
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const param_t gParam[] = 
{
	{"Thresh",		"dB",		0.5f},
	{"Ratio",		"num",		0.5f},
	{"Gain",		"dB",		0.0f},
	{"Attack",		"ms",		0.375f},
	{"Release",		"ms",		0.5f},
	{"Knee dB",		"dB",		0.375f},
	{"Knee %",		"%",		0.0f},
	{"SC Mode",		"typ",		0.0f},
	{"Push",		"typ",		0.0f},
	{"Wet/Dry",		"%",		0.0f},
	{"F.Back",		"n/y",		0.0f},
};

//------------------------------------------------------------------------------------

#define SN01_VER		1210
#define SN01_UID		'SN01'
#ifdef SN01G
#define SN01_NAM		"SN01-G Compressor"
#else
#define SN01_NAM		"SN01 Compressor"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseCompressor : public SignalNoiseFX 
{
private:
	double	_TdB;	// envelope filter
	double	_atk;	// attack coefficient
	double	_rls;	// release coefficient
	double	_fbL;	// feedback line L
	double	_fbR;	// feedback line R
	noise	_rnd;	// noise generator
	biquad	_lsL;	// SC low shelf L
	biquad	_hsL;	// SC high shelf L
	biquad	_lsR;	// SC low shelf R
	biquad	_hsR;	// SC high shelf R
private:
	void setupEnvelope();
	void setupSidechain();
//callbacks - SN
	virtual void onSetSampleRate(float fs);
	virtual void onSetParameter(VstInt32 at, float v);
public:
//create & destroy
	SignalNoiseCompressor(audioMasterCallback cb);
	virtual ~SignalNoiseCompressor();
//process - SDK
	virtual void processReplacing(float** in, float** out, VstInt32 sz);
	virtual void processDoubleReplacing(double** in, double** out, VstInt32 sz);
//plugin info - SDK
	VST_DEFINE_PLUGINFO(SN01_NAM, SN01_VER, kPlugCategEffect);
};

//------------------------------------------------------------------------------------


#endif // _SN_01E_H
