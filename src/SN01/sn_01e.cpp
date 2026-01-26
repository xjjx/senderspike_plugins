//------------------------------------------------------------------------------------
//
//	file:		sn_01e.cpp
//
//	purpose:	SN01 compressor effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <stdlib.h>
#include <sn_01e.h>
#ifdef SN01G
#include <sn_01g.h>
#endif


//------------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------------

AudioEffect* createEffectInstance(audioMasterCallback cb)
{
	return new SignalNoiseCompressor(cb);
}

//------------------------------------------------------------------------------------
// class SignalNoiseCompressor
//------------------------------------------------------------------------------------

SignalNoiseCompressor::SignalNoiseCompressor(audioMasterCallback cb) : SignalNoiseFX(cb, 0, SNE_SIZE)
{
	_TdB = DC_OFFSET;
	_atk = 0;
	_rls = 0;
	_fbL = DC_OFFSET;
	_fbR = DC_OFFSET;
	_rnd.seed();

	InitParams(gParam, SNE_SIZE);

	setUniqueID(SN01_UID);
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing(true);
	canDoubleReplacing(true);
	programsAreChunks(true);

#ifdef SN01G
	editor = new SignalNoiseCompressorGUI(this);
#endif
}

//------------------------------------------------------------------------------------

SignalNoiseCompressor::~SignalNoiseCompressor()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::onSetSampleRate(float fs)
{
	setupEnvelope();
	setupSidechain();
#ifdef SN01G
	((SignalNoiseCompressorGUI*)editor)->setupMeter(fs);
#endif
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::onSetParameter(VstInt32 at, float v)
{
	switch(at)
	{
	case SNE_ATTK: 
	case SNE_RELS: setupEnvelope();		break;
	case SNE_MODE: setupSidechain();	break;
	}
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

static int GetSwitch(const float& v)
{
	static const float sv = 1.f / 3.f;

	if(v < sv) return 0;
	if(v > 1 - sv) return 2;
	return 1;
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::setupEnvelope()
{
	double attk = _param[SNE_ATTK].val;
	double rels = _param[SNE_RELS].val;
	attk = (attk * attk * attk * 29.97) + 0.03;
	rels = (rels * rels * rels * 1950.0) + 50.0;
	_atk = exp(-1000.0 / (attk * sampleRate));
	_rls = exp(-1000.0 / (rels * sampleRate));
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::setupSidechain()
{
	switch(GetSwitch(_param[SNE_MODE].val))
	{
	case 1:
		_lsL.setup_q(LSF, -14.0, 0.49, 40.0, sampleRate);
		_hsL.setup_q(HSF,  12.0, 0.71, 4200.0, sampleRate);
		_lsR.setup_q(LSF, -14.0, 0.49, 40.0, sampleRate);
		_hsR.setup_q(HSF,  12.0, 0.71, 4200.0, sampleRate);
		break;
	case 2:
		_lsL.setup_q(LSF, -14.0, 0.37, 84.0, sampleRate);
		_hsL.setup_q(HSF,  12.0, 0.37, 2600.0, sampleRate);
		_lsR.setup_q(LSF, -14.0, 0.37, 84.0, sampleRate);
		_hsR.setup_q(HSF,  12.0, 0.37, 2600.0, sampleRate);
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::processReplacing(float** in, float** out, VstInt32 sz)
{
	float* inL = in[0];
	float* inR = in[1];
	float* outL = out[0];
	float* outR = out[1];

	int mode, push, fbck;
	double trsh, func, gain, knee, dry, wet;
	double L, R, fL, fR, rL, rR, kB, dB, y0, gr, kh;

	gain	= dB2lin(_param[SNE_GAIN].val * 24.0);
	fbck	= _param[SNE_FBCK].val > 0.5 ? 1 : 0;
	mode	= GetSwitch(_param[SNE_MODE].val);
	push	= GetSwitch(_param[SNE_PUSH].val);
	func	= sqrt(_param[SNE_FUNC].val);
	trsh	= _param[SNE_TRSH].val * -40.0 - (9.0 * push);
	knee	= _param[SNE_KNEE].val * _param[SNE_KWDT].val * 24.0;
	dry		= _param[SNE_COMP].val;
	wet		= (1.0 - dry) * gain;
	kh		= knee / 2.0;

	while(--sz >= 0)
	{
		L = *inL++;
		R = *inR++;

		//SC -> FF/FB + filter
		if(fbck)
		{
			fL = _fbL;
			fR = _fbR;
		}
		else
		{
			fL = L;
			fR = R;
		}
		if(mode)
		{
			fL = _hsL.run(_lsL.run(fL));
			fR = _hsR.run(_lsR.run(fR));
		}

		//rectifier -> thresh + push
		rL = fabs(fL);
		rR = fabs(fR);
		kB = lin2dB(getmax(rL, rR) + DC_OFFSET);
		dB = kB - trsh;
		if(push) dB += _rnd.pink() * 2.0 * push;

		//ratio + knee
		y0 = 0;		
		if(dB <= -kh)
			y0 = kB;
		else if((dB > -kh) && (dB < kh))
			y0 = kB + func * ((dB + kh) * (dB + kh)) / (2 * knee);
		else if(dB >= kh)
			y0 = kB + func * dB;
		dB = y0 - kB;

		//envelope
		dB += DC_OFFSET;
		if(dB > _TdB)
			_TdB = (_atk * _TdB) + ((1.0 - _atk) * dB);
		else
			_TdB = (_rls * _TdB) + ((1.0 - _rls) * dB);
		dB = _TdB - DC_OFFSET;

		//gain
		gr = dB2lin(-dB);

		_fbL = L * gr;
		_fbR = R * gr;

		//output		
		(*outL++) = float((_fbL * wet) + (dry * L));
		(*outR++) = float((_fbR * wet) + (dry * R));

#ifdef SN01G
		((SignalNoiseCompressorGUI*)editor)->trackMeter(dB);
#endif
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::processDoubleReplacing(double** in, double** out, VstInt32 sz)
{
	double* inL = in[0];
	double* inR = in[1];
	double* outL = out[0];
	double* outR = out[1];

	int mode, push, fbck;
	double trsh, func, gain, knee, dry, wet;
	double L, R, fL, fR, rL, rR, kB, dB, y0, gr, kh;

	gain	= dB2lin(_param[SNE_GAIN].val * 24.0);
	fbck	= _param[SNE_FBCK].val > 0.5 ? 1 : 0;
	mode	= GetSwitch(_param[SNE_MODE].val);
	push	= GetSwitch(_param[SNE_PUSH].val);
	func	= sqrt(_param[SNE_FUNC].val);
	trsh	= _param[SNE_TRSH].val * -40.0 - (9.0 * push);
	knee	= _param[SNE_KNEE].val * _param[SNE_KWDT].val * 24.0;
	dry		= _param[SNE_COMP].val;
	wet		= (1.0 - dry) * gain;
	kh		= knee / 2.0;

	while(--sz >= 0)
	{
		L = *inL++;
		R = *inR++;

		//SC -> FF/FB + filter
		if(fbck)
		{
			fL = _fbL;
			fR = _fbR;
		}
		else
		{
			fL = L;
			fR = R;
		}
		if(mode)
		{
			fL = _hsL.run(_lsL.run(fL));
			fR = _hsR.run(_lsR.run(fR));
		}

		//rectifier -> thresh + push
		rL = fabs(fL);
		rR = fabs(fR);
		kB = lin2dB(getmax(rL, rR) + DC_OFFSET);
		dB = kB - trsh;
		if(push) dB += _rnd.pink() * 2.0 * push;

		//ratio + knee
		y0 = 0;		
		if(dB <= -kh)
			y0 = kB;
		else if((dB > -kh) && (dB < kh))
			y0 = kB + func * ((dB + kh) * (dB + kh)) / (2 * knee);
		else if(dB >= kh)
			y0 = kB + func * dB;
		dB = y0 - kB;

		//envelope
		dB += DC_OFFSET;
		if(dB > _TdB)
			_TdB = (_atk * _TdB) + ((1.0 - _atk) * dB);
		else
			_TdB = (_rls * _TdB) + ((1.0 - _rls) * dB);
		dB = _TdB - DC_OFFSET;

		//gain
		gr = dB2lin(-dB);

		_fbL = L * gr;
		_fbR = R * gr;

		//output		
		(*outL++) = ((_fbL * wet) + (dry * L));
		(*outR++) = ((_fbR * wet) + (dry * R));

#ifdef SN01G
		((SignalNoiseCompressorGUI*)editor)->trackMeter(dB);
#endif
	}
}

//------------------------------------------------------------------------------------
