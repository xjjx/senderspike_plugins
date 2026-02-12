//------------------------------------------------------------------------------------
//
//	file:		sn_04e.cpp
//
//	purpose:	SN04 Channel EQ effect
//
//	authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include "SignalNoiseEqualizer.h"
#include "SignalNoiseEqualizerGUI.h"

//------------------------------------------------------------------------------------
// class SignalNoiseEqualizer
//------------------------------------------------------------------------------------

SignalNoiseEqualizer::SignalNoiseEqualizer()
	: SignalNoiseFX(createLayout(gParams, SNE_SIZE))
{
	_norm = 1e-15;
	_mojo.seed();

	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.addParameterListener (gParams[i].id, this);
}

SignalNoiseEqualizer::~SignalNoiseEqualizer()
{
	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.removeParameterListener (gParams[i].id, this);
}

// ----------------------
// Playback
// ----------------------
void SignalNoiseEqualizer::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	sampleRate = newSampleRate;

	setupHF();
	setupMF();
	setupLF();
	setupLP();
	setupHP();
	setupAnalog();
}

//------------------------------------------------------------------------------------
int SignalNoiseEqualizer::paramIdToIndex (const juce::String& id)
{
	for (int i = 0; i < SNE_SIZE; ++i)
		if (id == gParams[i].id)
			return i;

	return -1;
}

void SignalNoiseEqualizer::parameterChanged (const juce::String& id,
                                                      float /*newValue*/)
{
	const int at = paramIdToIndex (id);
	if (at < 0)
		return;

	switch(at)
	{
	case SNE_HF_F:
	case SNE_HF_G:
	case SNE_HF_Q:
	case SNE_HF_T:
	case SNE_HF_M: setupHF();		break;
	case SNE_MF_F:
	case SNE_MF_G:
	case SNE_MF_Q:
	case SNE_MF_T: setupMF();		break;
	case SNE_LF_F:
	case SNE_LF_G:
	case SNE_LF_Q:
	case SNE_LF_T:
	case SNE_LF_M: setupLF();		break;
	case SNE_LPAS:
	case SNE_LOCT: setupLP();		break;
	case SNE_HPAS:
	case SNE_HOCT: setupHP();		break;
	case SNE_MOJO: setupAnalog();	break;
	}
#ifdef SN04G
	switch(at)
	{
	case SNE_HF_F: ((SignalNoiseEqualizerGUI*)editor)->setLed(HF, v >= 0.142);	break;
	case SNE_MF_F: ((SignalNoiseEqualizerGUI*)editor)->setLed(MF, v >= 0.142);	break;
	case SNE_LF_F: ((SignalNoiseEqualizerGUI*)editor)->setLed(LF, v >= 0.142);	break;
	case SNE_LPAS: ((SignalNoiseEqualizerGUI*)editor)->setLed(LP, v >= 0.2);		break;
	case SNE_HPAS: ((SignalNoiseEqualizerGUI*)editor)->setLed(HP, v >= 0.2);		break;
	case SNE_HF_B: ((SignalNoiseEqualizerGUI*)editor)->setBlink(HF, v >= 0.5);	break;
	case SNE_MF_B: ((SignalNoiseEqualizerGUI*)editor)->setBlink(MF, v >= 0.5);	break;
	case SNE_LF_B: ((SignalNoiseEqualizerGUI*)editor)->setBlink(LF, v >= 0.5);	break;
	case SNE_LP_B: ((SignalNoiseEqualizerGUI*)editor)->setBlink(LP, v >= 0.5);	break;
	case SNE_HP_B: ((SignalNoiseEqualizerGUI*)editor)->setBlink(HP, v >= 0.5);	break;
	}
#endif
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

static double gHF[] = { 0, 1100, 2200, 3900, 5800, 8200, 10000, 12000 };
static double gMF[] = { 0,  220,  360,  700, 1600, 3200,  4800,  7200 };
static double gLF[] = { 0,   35,   45,   60,  110,  220,   300,   400 };
static double nLF[] = { 0,  120,  150,  200,  300,  440,   600,   800 };

static double gLP[] = { 0, 18000, 14000, 10000, 8000, 6000 };
static double gHP[] = { 0,    15,    45,    70,  160,  360 };

//------------------------------------------------------------------------------------

static double gHFs[] = { 0, 4.53, 4.53, 4.53, 3.65, 1.9, 1.9, 1.6 };
static double gHFr[] = { 0, 2.26, 2.26, 2.26, 1.80, 0.6, 0.6, 0.5 };

static double gMFs[] = { 0, 4.53, 4.53, 4.53, 4.53, 4.53, 3.65, 2.0 };
static double gMFr[] = { 0, 2.26, 2.26, 2.26, 2.26, 2.26, 1.80, 0.6 };

//------------------------------------------------------------------------------------

static double snGetBellBW(float f, float g, double db, double s, double r)
{
	double vm = g < 0.5 ? 0.5f - g : g - 0.5f;
	double fm = f < 0.5 ? 0.5f - f : f - 0.5f;
	double mx = s - (vm * r * 2);

	fm *= 1.25;

	if(db < 0)
	{
		double rg = 1 - db / -18;
		mx *= 0.163 + rg * 0.28;
	}

	return f < 0.5 ? mx - fm * mx : mx + fm * mx;
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupHF()
{
	double f, v, q;
	float Gdb = getParamValue(SNE_HF_G);
	float G = getParamNorm(SNE_HF_G);
	float Q = getParamValue(SNE_HF_Q);
	int fi = getParamChoice(SNE_HF_F);
	biquad_e typ = getParamChoice(SNE_HF_M) == 1 ? PKF : HSF; // 1 - Bell

	if(!fi)
	{
		_hf_La.reset(); 
		_hf_Ra.reset(); 
		_hf_Lb.reset(); 
		_hf_Rb.reset(); 
		return;
	}

	f = gHF[fi];
	v = Gdb;

	if(typ == HSF)
	{
		if(getParamChoice(SNE_HF_T) == 0) // 0 - A
		{
			//API
			_hf_Lb.setup(v, f, sampleRate);
			_hf_Rb.setup(v, f, sampleRate);
			_hf_La.reset(); 
			_hf_Ra.reset();
		}
		else
		{
			//Neve
			_hf_Lb.setup(v, f, sampleRate);
			_hf_Rb.setup(v, f, sampleRate);
			f = f * 0.121;
			q = (1 - G) * 0.2 + 2;
			v = -(G * 6 - 3);
			_hf_La.setup_bw(PKF, v, q, f, sampleRate);
			_hf_Ra.setup_bw(PKF, v, q, f, sampleRate);
		}
	}
	else
	{
		if(getParamChoice(SNE_HF_T) == 1) // 1 - B
			q = snGetBellBW(Q, G, v, gHFs[fi], gHFr[fi]);
		else
			q = snGetBellBW(Q, G, 0, gHFs[fi], gHFr[fi]);

		_hf_La.setup_bw(PKF, v, q, f, sampleRate);
		_hf_Ra.setup_bw(PKF, v, q, f, sampleRate);
		_hf_Lb.reset();
		_hf_Rb.reset();
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupMF()
{
	double f, v, q;
	float gainDb = getParamValue(SNE_MF_G);
	float gain = getParamNorm(SNE_MF_G);
	float fact = getParamValue(SNE_MF_Q);
	int fi = getParamChoice(SNE_MF_F);

	if(!fi)
	{
		_mf_Lp.reset(); 
		_mf_Rp.reset();
		return;
	}

	f = gMF[fi];
	v = gainDb;

	if(getParamChoice(SNE_MF_T) == 1) // 1 - B
		q = snGetBellBW(fact, gain, v, gMFs[fi], gMFr[fi]);
	else
		q = snGetBellBW(fact, gain, 0, gMFs[fi], gMFr[fi]);

	_mf_Lp.setup_bw(PKF, v, q, f, sampleRate);
	_mf_Rp.setup_bw(PKF, v, q, f, sampleRate);
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupLF()
{
	double f, v, q, n;
	float Q = getParamValue(SNE_LF_Q);
	float G = getParamNorm(SNE_LF_G);
	float Gdb = getParamValue(SNE_LF_G);
	int fi = getParamChoice(SNE_LF_F);
	biquad_e typ = getParamValue(SNE_LF_M) == 1 ? PKF : LSF; // 1 - Bell

	if(!fi)
	{
		_lf_La.reset(); 
		_lf_Ra.reset(); 
		_lf_Lb.reset(); 
		_lf_Rb.reset();
		_lf_Lc.reset();
		_lf_Rc.reset();
		return;
	}

	f = gLF[fi];
	v = Gdb;

	if(typ == LSF)
	{
		if(getParamChoice(SNE_LF_T) == 0) // 0 - A
		{
			//550A
			_lf_Lc.setup(v, f, sampleRate);
			_lf_Rc.setup(v, f, sampleRate);
			_lf_La.reset(); 
			_lf_Ra.reset();
			_lf_Lb.reset(); 
			_lf_Rb.reset();
		}
		else
		{
			//Neve
			n = nLF[fi];
			_lf_La.setup_bw(LSF, v, 2.35, n, sampleRate);
			_lf_Ra.setup_bw(LSF, v, 2.35, n, sampleRate);

			f = n * 5;
			n = fi * 0.1; 
			v = -(G * (3 - n * 2) - (1.5 - n));
			_lf_Lb.setup_bw(PKF, v, 3, f, sampleRate);
			_lf_Rb.setup_bw(PKF, v, 3, f, sampleRate);

			_lf_Lc.reset();
			_lf_Rc.reset();
		}
	}
	else
	{
		if(getParamChoice(SNE_LF_T) == 1) // 1 - B
			q = snGetBellBW(Q, G, v, 4.53, 2.26);
		else
			q = snGetBellBW(Q, G, 0, 4.53, 2.26);

		_lf_La.setup_bw(PKF, v, q, f, sampleRate);
		_lf_Ra.setup_bw(PKF, v, q, f, sampleRate);
		_lf_Lb.reset();
		_lf_Rb.reset();
		_lf_Lc.reset();
		_lf_Rc.reset();
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupLP()
{
	double fc;
	int fi = getParamChoice(SNE_LPAS);
	int ti = getParamChoice(SNE_LOCT);

	if(fi)
	{
		fc = gLP[fi];

		switch(ti)
		{
		case 0:
			_lp06_L.setup(fc, sampleRate);
			_lp06_R.setup(fc, sampleRate);
			break;
		case 1:
			_lp12_L.setup_q(LPF, 0, qf12, fc, sampleRate);
			_lp12_R.setup_q(LPF, 0, qf12, fc, sampleRate);
			break;
		case 2:
			_lp06_L.setup(fc, sampleRate);
			_lp06_R.setup(fc, sampleRate);
			_lp12_L.setup_q(LPF, 0, qf18, fc, sampleRate);
			_lp12_R.setup_q(LPF, 0, qf18, fc, sampleRate);
			break;
		default:
			_lp12_L.setup_q(LPF, 0, qf24a, fc, sampleRate);
			_lp12_R.setup_q(LPF, 0, qf24a, fc, sampleRate);
			_lp24_L.setup_q(LPF, 0, qf24b, fc, sampleRate);
			_lp24_R.setup_q(LPF, 0, qf24b, fc, sampleRate);
			break;
		}
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupHP()
{
	double fc;
	int fi = getParamChoice(SNE_HPAS);
	int ti = getParamChoice(SNE_HOCT);
	
	if(fi)
	{
		fc = gHP[fi];

		switch(ti)
		{
		case 0:
			_hp06_L.setup(fc, sampleRate);
			_hp06_R.setup(fc, sampleRate);
			break;
		case 1:
			_hp12_L.setup_q(HPF, 0, qf12, fc, sampleRate);
			_hp12_R.setup_q(HPF, 0, qf12, fc, sampleRate);
			break;
		case 2:
			_hp06_L.setup(fc, sampleRate);
			_hp06_R.setup(fc, sampleRate);
			_hp12_L.setup_q(HPF, 0, qf18, fc, sampleRate);
			_hp12_R.setup_q(HPF, 0, qf18, fc, sampleRate);
			break;
		default:
			_hp12_L.setup_q(HPF, 0, qf24a, fc, sampleRate);
			_hp12_R.setup_q(HPF, 0, qf24a, fc, sampleRate);
			_hp24_L.setup_q(HPF, 0, qf24b, fc, sampleRate);
			_hp24_R.setup_q(HPF, 0, qf24b, fc, sampleRate);
			break;
		}
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizer::setupAnalog()
{
	if(getParamChoice(SNE_MOJO) == 1) // 1 - On
	{
		_hsfL.setup(-2, 15000, sampleRate);
		_hsfR.setup(-2, 15000, sampleRate);
	}
	else
	{
		_hsfL.reset();
		_hsfR.reset();
	}
}

//------------------------------------------------------------------------------------

template <typename Sample>
void SignalNoiseEqualizer::processImpl(juce::AudioBuffer<Sample>& buffer)
{
	const int numSamples  = buffer.getNumSamples();
	const int numChannels = buffer.getNumChannels();
	const bool mono       = (numChannels == 1);

	Sample* inL  = buffer.getWritePointer(0);
	Sample* inR  = mono ? nullptr : buffer.getWritePointer(1);
	Sample* outL = inL;
	Sample* outR = inR;

	// ----------------------
	// Read parameters once
	// ----------------------
	const float gainParam  = getParamValue(SNE_GAIN);

	const float hpasParam = getParamValue(SNE_HPAS);
	const float lpasParam = getParamValue(SNE_LPAS);

	const Sample dB = (Sample) dB2lin(gainParam);
	const Sample ph = getParamChoice(SNE_IPHS) == 1 ? (Sample) -1 : (Sample) 1;

	const bool hf = getParamChoice(SNE_HF_B) == 0; // 0 - Off
	const bool mf = getParamChoice(SNE_MF_B) == 0; // 0 - Off
	const bool lf = getParamChoice(SNE_LF_B) == 0; // 0 - Off
	const bool hp = hpasParam >= 0.2f && getParamChoice(SNE_HP_B) == 0; // 0 - Off
	const bool lp = lpasParam >= 0.2f && getParamChoice(SNE_LP_B) == 0; // 0 - Off
	const bool dm = getParamChoice(SNE_MOJO) == 1; // 1 - On

	const int hi = getParamChoice(SNE_HOCT);
	const int li = getParamChoice(SNE_LOCT);

	_norm = -_norm;

	for (int n = 0; n < numSamples; ++n)
	{
		double L = *inL++;
		double R = mono ? 0 : *inR++;

		//"analog"
		if(dm)
		{
			L = _hsfL.run(L + _mojo.white() * 0.00001);
			R = _hsfR.run(R + _mojo.white() * 0.00001);
		}

		//bands
		if(lf)
		{
			L = _lf_La.run(_lf_Lb.run(_lf_Lc.run(L)));
			R = _lf_Ra.run(_lf_Rb.run(_lf_Rc.run(R)));
		}
		if(mf)
		{
			L = _mf_Lp.run(L + _norm);
			R = _mf_Rp.run(R + _norm);
		}
		if(hf)
		{
			L = _hf_La.run(_hf_Lb.run(L + _norm) + _norm);
			R = _hf_Ra.run(_hf_Rb.run(R + _norm) + _norm);
		}

		//HP/LP
		if(hp)
		{
			switch(hi)
			{
			case 0:
				L = _hp06_L.run(L);
				R = _hp06_R.run(R);
				break;
			case 1:
				L = _hp12_L.run(L);
				R = _hp12_R.run(R);
				break;
			case 2:
				L = _hp12_L.run(_hp06_L.run(L));
				R = _hp12_R.run(_hp06_R.run(R));
				break;
			case 3:
				L = _hp24_L.run(_hp12_L.run(L));
				R = _hp24_R.run(_hp12_R.run(R));
				break;
			}
		}
		if(lp)
		{
			switch(li)
			{
			case 0:
				L = _lp06_L.run(L + _norm);
				R = _lp06_R.run(R + _norm);
				break;
			case 1:
				L = _lp12_L.run(L + _norm);
				R = _lp12_R.run(R + _norm);
				break;
			case 2:
				L = _lp12_L.run(_lp06_L.run(L + _norm) + _norm);
				R = _lp12_R.run(_lp06_R.run(R + _norm) + _norm);
				break;
			case 3:
				L = _lp24_L.run(_lp12_L.run(L + _norm) + _norm);
				R = _lp24_R.run(_lp12_R.run(R + _norm) + _norm);
				break;
			}
		}
		
		//output
		L *= dB;
		R *= dB;

		if(mono)
		{
			const Sample m = static_cast<Sample>(L * ph);
			(*outL++) = m;
			(*outR++) = m;
#ifdef SN04G
			((SignalNoiseEqualizerGUI*)editor)->trackPeaks(fabs(L));
#endif
		}
		else
		{
			(*outL++) = static_cast<Sample>(L * ph);
			(*outR++) = static_cast<Sample>(R * ph);
#ifdef SN04G
			((SignalNoiseEqualizerGUI*)editor)->trackPeaks((fabs(L) + fabs(R)) * 0.5);
#endif
		}
	}
}

void SignalNoiseEqualizer::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseEqualizer::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// Editor
juce::AudioProcessorEditor* SignalNoiseEqualizer::createEditor()
{
	return new SignalNoiseEqualizerGUI (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseEqualizer();
}
