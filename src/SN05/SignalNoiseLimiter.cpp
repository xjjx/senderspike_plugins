//------------------------------------------------------------------------------------
//
//	file:		sn_05e.cpp
//
//	purpose:	SN05 limiter effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <sn_05e.h>
#ifdef SN05G
#include <sn_05g.h>
#endif


//------------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------------

AudioEffect* createEffectInstance(audioMasterCallback cb)
{
	return new SignalNoiseLimiter(cb);
}

//------------------------------------------------------------------------------------
// class SignalNoiseLimiter
//------------------------------------------------------------------------------------

SignalNoiseLimiter::SignalNoiseLimiter(audioMasterCallback cb) : SignalNoiseFX(cb, 0, SNE_SIZE)
{
	for(int i = 0; i < 5; i++)
	{
		_dlL[i] = 0;
		_dlR[i] = 0;
	}
	_max = 0;
	_grH = 1;
	_atH = 0;
	_rlH = 0;
	_env = DC_OFFSET;
	_atk = 0;
	_rls = 0;

	InitParams(gParam, SNE_SIZE);
	
	setUniqueID(SN05_UID);
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing(true);
	canDoubleReplacing(true);
	programsAreChunks(true);

#ifdef SN05G
	editor = new SignalNoiseLimiterGUI(this);
#endif
}

//------------------------------------------------------------------------------------

SignalNoiseLimiter::~SignalNoiseLimiter()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::onSetSampleRate(float fs)
{
	setupLimiter();
	setupClipper();
	setupSidechain();
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::onSetParameter(VstInt32 at, float v)
{
	switch(at)
	{
	case SNE_ATKH:
	case SNE_RELH:
		setupLimiter();
		break;
	case SNE_RELS:
	case SNE_MODE:
		setupClipper();
		break;
	case SNE_HPON:
	case SNE_HPFC:
		setupSidechain();
		break;
	}
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

void SignalNoiseLimiter::setupLimiter()
{
	double attk = _param[SNE_ATKH].val;
	double rels = _param[SNE_RELH].val;
	double t = (1.0 / sampleRate) * -2.2;

	attk = (attk * attk * attk * 249.98) + 0.02;
	rels = (rels * rels * rels * 1290.0) + 10;
	_atH = 1 - exp(t / (.001 * attk));
	_rlH = 1 - exp(t / (.001 * rels));
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::setupClipper()
{
	double rels = _param[SNE_RELS].val;

	if(_param[SNE_MODE].val < 0.5)
		rels = (rels * rels * rels * 499.0) + 1.0;
	else
		rels = (rels * rels * rels * 49.9) + 0.1;

	_atk = exp(-1000.0 / (.001 * sampleRate));
	_rls = exp(-1000.0 / (rels * sampleRate));
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::setupSidechain()
{
	double fc = _param[SNE_HPFC].val * 1975 + 15;

	_hL1.setup_q(HPF, 0, qf18, fc, sampleRate);
	_hR1.setup_q(HPF, 0, qf18, fc, sampleRate);
	_hL2.setup(fc, sampleRate);
	_hR2.setup(fc, sampleRate);
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::processReplacing(float** in, float** out, VstInt32 sz)
{
	float* inL = in[0];
	float* inR = in[1];
	float* outL = out[0];
	float* outR = out[1];

	bool mode, hpon;
	double L, R, fL, fR, dB, gr, ec;
	double trsh, gain, ceil, wet, dry, grh;

	gain = dB2lin(_param[SNE_GAIN].val * 24.0);
	trsh = (1 - _param[SNE_CEIL].val) * -24.0;
	ceil = dB2lin(trsh);
	mode = _param[SNE_MODE].val < 0.5;
	hpon = _param[SNE_HPON].val > 0.5;
	wet  = _param[SNE_CLIP].val;
	dry  = 1 - wet;

	while(--sz >= 0)
	{
		grh = 0;

		L = *inL++ * gain;
		R = *inR++ * gain;

		if(_mono) R = 0;

		//Holters' limiter 
		if(mode)
		{
			//SC filter
			if(hpon)
			{
				fL = _hL1.run(_hL2.run(L));
				fR = _hR1.run(_hR2.run(R));
			}
			else
			{
				fL = L;
				fR = R;
			}

			//limiter
			fL = fabs(fL);
			fR = fabs(fR);
			gr = getmax(fL, fR) + DC_OFFSET;
			ec = gr > _max ? _atH : _rlH;
			_max = (1 - ec) * _max + ec * gr;

			ec = ceil / _max;
			gr = 1 > ec ? ec : 1;
			ec = gr < _grH ? _atH : _rlH;
			_grH = (1 - ec) * _grH + ec * gr;
		
			fL = _grH * _dlL[4];
			fR = _grH * _dlR[4];

			_dlL[4] = _dlL[3];
			_dlL[3] = _dlL[2];
			_dlL[2] = _dlL[1];
			_dlL[1] = _dlL[0];
			_dlL[0] = L;

			_dlR[4] = _dlR[3];
			_dlR[3] = _dlR[2];
			_dlR[2] = _dlR[1];
			_dlR[1] = _dlR[0];
			_dlR[0] = R;

			L = fL;
			R = fR;

			grh = -lin2dB(_grH);
		}

		//soft clipper
		if(wet)
		{
			L = erf(L) * wet + L * dry;
			R = erf(R) * wet + R * dry;
		}

		//brickwall (hard clip)
		fL = fabs(L);
		fR = fabs(R);
		gr = lin2dB(getmax(fL, fR) + DC_OFFSET);
		dB = gr - trsh;

		if(dB < 0.0) dB = 0.0;

		dB += DC_OFFSET;
		if(dB > _env)
			_env = dB + _atk * (_env - dB);
		else
			_env = dB + _rls * (_env - dB);
		dB = _env - DC_OFFSET;
			
		gr = dB2lin(-dB);
				
		(*outL++) = float(L * gr);
		(*outR++) = float(R * gr);

#ifdef SN05G
		((SignalNoiseLimiterGUI*)editor)->trackMeter(grh, dB);
#endif
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::processDoubleReplacing(double** in, double** out, VstInt32 sz)
{
	double* inL = in[0];
	double* inR = in[1];
	double* outL = out[0];
	double* outR = out[1];

	bool mode, hpon;
	double L, R, fL, fR, dB, gr, ec;
	double trsh, gain, ceil, wet, dry, grh;

	gain = dB2lin(_param[SNE_GAIN].val * 24.0);
	trsh = (1 - _param[SNE_CEIL].val) * -24.0;
	ceil = dB2lin(trsh);
	mode = _param[SNE_MODE].val < 0.5;
	hpon = _param[SNE_HPON].val > 0.5;
	wet  = _param[SNE_CLIP].val;
	dry  = 1 - wet;

	while(--sz >= 0)
	{
		grh = 0;

		L = *inL++ * gain;
		R = *inR++ * gain;

		if(_mono) R = 0;

		//Holters' limiter 
		if(mode)
		{
			//SC filter
			if(hpon)
			{
				fL = _hL1.run(_hL2.run(L));
				fR = _hR1.run(_hR2.run(R));
			}
			else
			{
				fL = L;
				fR = R;
			}

			//limiter
			fL = fabs(fL);
			fR = fabs(fR);
			gr = getmax(fL, fR) + DC_OFFSET;
			ec = gr > _max ? _atH : _rlH;
			_max = (1 - ec) * _max + ec * gr;

			ec = ceil / _max;
			gr = 1 > ec ? ec : 1;
			ec = gr < _grH ? _atH : _rlH;
			_grH = (1 - ec) * _grH + ec * gr;
		
			fL = _grH * _dlL[4];
			fR = _grH * _dlR[4];

			_dlL[4] = _dlL[3];
			_dlL[3] = _dlL[2];
			_dlL[2] = _dlL[1];
			_dlL[1] = _dlL[0];
			_dlL[0] = L;

			_dlR[4] = _dlR[3];
			_dlR[3] = _dlR[2];
			_dlR[2] = _dlR[1];
			_dlR[1] = _dlR[0];
			_dlR[0] = R;

			L = fL;
			R = fR;

			grh = -lin2dB(_grH);
		}

		//soft clipper
		if(wet)
		{
			L = erf(L) * wet + L * dry;
			R = erf(R) * wet + R * dry;
		}

		//brickwall (hard clip)
		fL = fabs(L);
		fR = fabs(R);
		gr = lin2dB(getmax(fL, fR) + DC_OFFSET);
		dB = gr - trsh;

		if(dB < 0.0) dB = 0.0;

		dB += DC_OFFSET;
		if(dB > _env)
			_env = dB + _atk * (_env - dB);
		else
			_env = dB + _rls * (_env - dB);
		dB = _env - DC_OFFSET;
			
		gr = dB2lin(-dB);
				
		(*outL++) = L * gr;
		(*outR++) = R * gr;

#ifdef SN05G
		((SignalNoiseLimiterGUI*)editor)->trackMeter(grh, dB);
#endif
	}
}

//------------------------------------------------------------------------------------

