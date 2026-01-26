//------------------------------------------------------------------------------------
//
//	file:		sn_06e.cpp
//
//	purpose:	SN06 op-amp effect
//				- TODO: switch to invert/keep phase of feedback
//
//  authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <sn_06e.h>
#ifdef SN06G
#include <sn_06g.h>
#endif


//------------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------------

AudioEffect* createEffectInstance(audioMasterCallback cb)
{
	return new SignalNoiseOpamp(cb);
}

//------------------------------------------------------------------------------------
// class SignalNoiseOpamp
//------------------------------------------------------------------------------------

SignalNoiseOpamp::SignalNoiseOpamp(audioMasterCallback cb) : SignalNoiseFX(cb, 0, SNE_SIZE)
{
	_erfL = 0;
	_erfR = 0;
	_norm = 1e-15;

	InitParams(gParam, SNE_SIZE);
	
	setUniqueID(SN06_UID);
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing(true);
	canDoubleReplacing(true);
	programsAreChunks(true);

#ifdef SN06G
	editor = new SignalNoiseOpampGUI(this);
#endif
}

//------------------------------------------------------------------------------------

SignalNoiseOpamp::~SignalNoiseOpamp()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseOpamp::onSetSampleRate(float fs)
{
	_hpfL.setup(15, fs);
	_hpfR.setup(15, fs);
}

//------------------------------------------------------------------------------------

void SignalNoiseOpamp::processReplacing(float** in, float** out, VstInt32 sz)
{
	float* inL = in[0];
	float* inR = in[1];
	float* outL = out[0];
	float* outR = out[1];

	double L, R, eL, eR, iL, iR;
	double gain, trim, fact, invf, volu;

	trim = dB2lin(_param[SNE_TRIM].val * 40.0 - 20.0);
	volu = dB2lin(_param[SNE_VOLU].val * 64.0 - 48.0);
	gain = dB2lin(_param[SNE_GAIN].val * 32.0 - 8.0);
	fact = _param[SNE_GAIN].val * 0.55;
	invf = 1 - fact;
	fact *= gain;
	invf *= gain;

	_norm = -_norm;

	while(--sz >= 0)
	{
		iL = L = *inL++; 
		iR = R = *inR++; 
		L = L * trim;
		R = R * trim;

		if(_mono) R = 0;

		eL = _erfL;
		eR = _erfR;
		_erfL = erf(L + _norm);
		_erfR = erf(R + _norm);
		L = (eL * fact + L * invf);
		R = (eR * fact + R * invf);

		L = _hpfL.run(L) * volu;
		R = _hpfR.run(R) * volu;

		(*outL++) = float(L);
		(*outR++) = float(R);

#ifdef SN06G
		if(_mono)
		{
			((SignalNoiseOpampGUI*)editor)->trackMeters(fabs(iL), fabs(L));
		}
		else
		{
			((SignalNoiseOpampGUI*)editor)->trackMeters(
				(fabs(iL) + fabs(iR)) * 0.5, (fabs(L) + fabs(R)) * 0.5);
		}
#endif
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseOpamp::processDoubleReplacing(double** in, double** out, VstInt32 sz)
{
	double* inL = in[0];
	double* inR = in[1];
	double* outL = out[0];
	double* outR = out[1];

	double L, R, eL, eR, iL, iR;
	double gain, trim, fact, invf, volu;

	trim = dB2lin(_param[SNE_TRIM].val * 40.0 - 20.0);
	volu = dB2lin(_param[SNE_VOLU].val * 64.0 - 48.0);
	gain = dB2lin(_param[SNE_GAIN].val * 32.0 - 8.0);
	fact = _param[SNE_GAIN].val * 0.55;
	invf = 1 - fact;
	fact *= gain;
	invf *= gain;

	_norm = -_norm;

	while(--sz >= 0)
	{
		iL = L = *inL++;
		iR = R = *inR++; 
		L = L * trim;
		R = R * trim;

		if(_mono) R = 0;

		eL = _erfL;
		eR = _erfR;
		_erfL = erf(L + _norm);
		_erfR = erf(R + _norm);
		L = (eL * fact + L * invf);
		R = (eR * fact + R * invf);

		L = _hpfL.run(L) * volu;
		R = _hpfR.run(R) * volu;

		(*outL++) = L;
		(*outR++) = R;

#ifdef SN06G
		if(_mono)
		{
			((SignalNoiseOpampGUI*)editor)->trackMeters(fabs(iL), fabs(L));
		}
		else
		{
			((SignalNoiseOpampGUI*)editor)->trackMeters(
				(fabs(iL) + fabs(iR)) * 0.5, (fabs(L) + fabs(R)) * 0.5);
		}
#endif
	}
}

//------------------------------------------------------------------------------------
