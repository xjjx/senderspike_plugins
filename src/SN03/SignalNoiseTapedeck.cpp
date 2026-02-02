//------------------------------------------------------------------------------------
//
//	file:		sn_03e.cpp
//
//	purpose:	SN03 tape recorder emulator effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <sn_03e.h>
#ifdef SN03G
#include <sn_03g.h>
#endif


//------------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------------

AudioEffect* createEffectInstance(audioMasterCallback cb)
{
	return new SignalNoiseTapedeck(cb);
}

//------------------------------------------------------------------------------------
// class SignalNoiseTapedeck
//------------------------------------------------------------------------------------

SignalNoiseTapedeck::SignalNoiseTapedeck(audioMasterCallback cb) : SignalNoiseFX(cb, 0, SNE_SIZE)
{
	_rand.seed();
		
	InitParams(gParam, SNE_SIZE);

	setUniqueID(SN03_UID);
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing(true);
	canDoubleReplacing(true);
	programsAreChunks(true);

#ifdef SN03G
	editor = new SignalNoiseTapedeckGUI(this);
#endif
}

//------------------------------------------------------------------------------------

SignalNoiseTapedeck::~SignalNoiseTapedeck()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::onSetSampleRate(float fs)
{
	setupTapeheads();
	setupEqualizer();

#ifdef SN03G
	((SignalNoiseTapedeckGUI*)editor)->setupMeterFilter(fs);
#endif
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::onSetParameter(VstInt32 at, float v)
{
	switch(at)
	{
	case SNE_RCLO:
	case SNE_RCHI:
	case SNE_RPLO:
	case SNE_RPHI:
	case SNE_LOON:
	case SNE_EQSC: setupEqualizer(); break;
	case SNE_HEAD:
	case SNE_BUMP:
	case SNE_ATTN:
	case SNE_HBON: setupTapeheads(); break;
#ifdef SN03G
	case SNE_ROOM:
		if(v < .25f)		((SignalNoiseTapedeckGUI*)editor)->setupMeterLevel(-12);
		else if(v < .5f)	((SignalNoiseTapedeckGUI*)editor)->setupMeterLevel(-14);
		else if(v < .75f)	((SignalNoiseTapedeckGUI*)editor)->setupMeterLevel(-18);
		else				((SignalNoiseTapedeckGUI*)editor)->setupMeterLevel(-20);
		break;
	case SNE_HOLD: ((SignalNoiseTapedeckGUI*)editor)->setupMeterUseHold(v > 0.5); break;
#endif
	}
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::setupTapeheads()
{
	double fs = sampleRate;
	double of = _param[SNE_ATTN].val * 2;
	double hd = _param[SNE_HEAD].val;
	double hv = _param[SNE_BUMP].val;
	double sc = 4 - hd;
	double dB = hv * sc + sc - of;
	double fc = hd * 40 + 65;
	double lf = hd * 10 + 10;
	
	if(_param[SNE_HBON].val > 0.5f)
	{
		_rep0L.setup(lf, fs);
		_rep0R.setup(lf, fs);
		_rep1L.setup_bw(PKF, dB, 3, fc, fs);
		_rep1R.setup_bw(PKF, dB, 3, fc, fs);
	}
	else
	{
		_rep0L.reset();
		_rep0R.reset();
		_rep1L.reset();
		_rep1R.reset();
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::setupEqualizer()
{
	static const float sv = 1.f / 3.f;

	double fc, fs = sampleRate;
	double eq = _param[SNE_EQSC].val;
	double hi1 = _param[SNE_RCHI].val * 28 - 14;
	double lo1 = _param[SNE_RCLO].val * 20 - 10;
	double hi2 = _param[SNE_RPHI].val * 28 - 14;
	double lo2 = _param[SNE_RPLO].val * 20 - 10;

	if(eq < sv)
	{
		fc = 3180;
	}
	else
	{
		if(_param[SNE_LOON].val < 0.5f)
		{
			lo1 = 0;
			lo2 = 0;
		}
		fc = (eq > 1 - sv) ? 9100 : 4550;
	}
	_pre1L.setup(lo1, 50, fs);
	_pre1R.setup(lo1, 50, fs);
	_pre2L.setup(hi1, fc, fs);
	_pre2R.setup(hi1, fc, fs);
	
	_dee1L.setup(lo2, 50, fs);
	_dee1R.setup(lo2, 50, fs);
	_dee2L.setup(hi2, fc, fs);
	_dee2R.setup(hi2, fc, fs);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::processReplacing(float** in, float** out, VstInt32 sz)
{
	float* inL = in[0];
	float* inR = in[1];
	float* outL = out[0];
	float* outR = out[1];

	double L, R, iG, oG, hG, vuL[2], vuR[2];

	iG = dB2lin(_param[SNE_TRIM].val * 48 - 24);
	oG = dB2lin(_param[SNE_GAIN].val * 48 - 24);
	hG = dB2lin(_param[SNE_HISS].val * 60 - 96);

	bool nois = _param[SNE_NOIS].val > 0.5;

#ifdef SN03G
	int id = _param[SNE_PATH].val > 0.5 ? 1 : 0;
#endif

	while(--sz >= 0)
	{
		vuL[0] = L = *inL++;
		vuR[0] = R = *inR++;
		L = L * iG;
		R = R * iG;

		//pre-emphasis
		L = _pre2L.run(_pre1L.run(L));
		R = _pre2R.run(_pre1R.run(R));

		//tape hiss
		if(nois)
		{
			L += _rand.white() * hG;
			R += _rand.white() * hG;
		}

		//tape hysteresis
		L = erf(L);
		R = erf(R);

		//repro head
		L = _rep1L.run(_rep0L.run(L));
		R = _rep1R.run(_rep0R.run(R));

		//de-emphasis
		L = _dee2L.run(_dee1L.run(L));
		R = _dee2R.run(_dee1R.run(R));

		(*outL++) = float(vuL[1] = L * oG);
		(*outR++) = float(vuR[1] = R * oG);

#ifdef SN03G
		if(_mono == false)
			((SignalNoiseTapedeckGUI*)editor)->trackMeter((fabs(vuL[id]) + fabs(vuR[id])) * 0.5);
		else
			((SignalNoiseTapedeckGUI*)editor)->trackMeter(fabs(vuL[id]));
#endif
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::processDoubleReplacing(double** in, double** out, VstInt32 sz)
{
	double* inL = in[0];
	double* inR = in[1];
	double* outL = out[0];
	double* outR = out[1];

	double L, R, iG, oG, hG, vuL[2], vuR[2];

	iG = dB2lin(_param[SNE_TRIM].val * 48 - 24);
	oG = dB2lin(_param[SNE_GAIN].val * 48 - 24);
	hG = dB2lin(_param[SNE_HISS].val * 60 - 96);

	bool nois = _param[SNE_NOIS].val > 0.5;

#ifdef SN03G
	int id = _param[SNE_PATH].val > 0.5 ? 1 : 0;
#endif

	while(--sz >= 0)
	{
		vuL[0] = L = *inL++;
		vuR[0] = R = *inR++;
		L = L * iG;
		R = R * iG;

		//pre-emphasis
		L = _pre2L.run(_pre1L.run(L));
		R = _pre2R.run(_pre1R.run(R));

		//tape hiss
		if(nois)
		{
			L += _rand.white() * hG;
			R += _rand.white() * hG;
		}

		//tape hysteresis
		L = erf(L);
		R = erf(R);

		//repro head
		L = _rep1L.run(_rep0L.run(L));
		R = _rep1R.run(_rep0R.run(R));

		//de-emphasis
		L = _dee2L.run(_dee1L.run(L));
		R = _dee2R.run(_dee1R.run(R));

		(*outL++) = vuL[1] = L * oG;
		(*outR++) = vuR[1] = R * oG;

#ifdef SN03G
		if(_mono == false)
			((SignalNoiseTapedeckGUI*)editor)->trackMeter((fabs(vuL[id]) + fabs(vuR[id])) * 0.5);
		else
			((SignalNoiseTapedeckGUI*)editor)->trackMeter(fabs(vuL[id]));
#endif
	}
}

//------------------------------------------------------------------------------------

