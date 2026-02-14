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
#include "SignalNoiseLimiter.h"
#include "SignalNoiseLimiterGUI.h"

#ifdef SN05G
#include <sn_05g.h>
#endif


//------------------------------------------------------------------------------------
// class SignalNoiseLimiter
//------------------------------------------------------------------------------------

SignalNoiseLimiter::SignalNoiseLimiter()
    : SignalNoiseFX(createLayout(gParams, SNE_SIZE))
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

	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.addParameterListener (gParams[i].id, this);
}

SignalNoiseLimiter::~SignalNoiseLimiter()
{
	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.removeParameterListener (gParams[i].id, this);
}

//------------------------------------------------------------------------------------
void SignalNoiseLimiter::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	sampleRate = newSampleRate;

	setupLimiter();
	setupClipper();
	setupSidechain();
}

//------------------------------------------------------------------------------------
int SignalNoiseLimiter::paramIdToIndex (const juce::String& id)
{
	for (int i = 0; i < SNE_SIZE; ++i)
		if (id == gParams[i].id)
			return i;

	return -1;
}

void SignalNoiseLimiter::parameterChanged (const juce::String& id, float /*newValue*/)
{
	const int at = paramIdToIndex (id);
	if (at < 0)
		return;

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
	float attkn = getParamValue(SNE_ATKH);
	float relsn = getParamValue(SNE_RELH);

	float attk = gParams[SNE_ATKH].normToCubic(attkn);
	float rels = gParams[SNE_RELH].normToCubic(relsn);

	DBG("Norm Attack: " + juce::String(attkn, 3) + " Release:" + juce::String(relsn, 3));
	DBG("Val: Attack: " + juce::String(attk, 2) + " Release:" + juce::String(rels, 2));
	DBG("min;" + juce::String(gParams[SNE_RELH].minValue,3) + " max:" + juce::String(gParams[SNE_RELH].maxValue,3));

	double t = (1.0 / sampleRate) * -2.2;

	_atH = 1 - exp(t / (.001 * attk));
	_rlH = 1 - exp(t / (.001 * rels));
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::setupClipper()
{
	float relsn = getParamValue(SNE_RELS);
	float rels = gParams[SNE_RELS].normToCubic(relsn);

	if(getParamChoice(SNE_MODE) == 1) // 0 - Clipper
		rels *= 0.1f;

	_atk = exp(-1000.0 / (.001 * sampleRate));
	_rls = exp(-1000.0 / (rels * sampleRate));
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiter::setupSidechain()
{
	double fc = getParamValue(SNE_HPFC) * 1975 + 15;

	_hL1.setup_q(HPF, 0, qf18, fc, sampleRate);
	_hR1.setup_q(HPF, 0, qf18, fc, sampleRate);
	_hL2.setup(fc, sampleRate);
	_hR2.setup(fc, sampleRate);
}

//------------------------------------------------------------------------------------
template <typename Sample>
void SignalNoiseLimiter::processImpl(juce::AudioBuffer<Sample>& buffer)
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
	const float gainParam = getParamValue(SNE_GAIN);
	const float ceilParam = getParamValue(SNE_CEIL);
	const float clipParam = getParamNorm(SNE_CLIP);

	const double gain = dB2lin(gainParam);
	const double trsh = ceilParam;
	const double ceil = dB2lin(trsh);
	const bool mode = getParamChoice(SNE_MODE) == 0; // 0 - limiter
	const bool hpon = getParamChoice(SNE_HPON) == 1;
	const double wet  = clipParam;
	const double dry  = 1 - wet;

	for (int n = 0; n < numSamples; ++n)
	{
		double L, R, gr, ec, fL, fR;
		double grh = 0;

		L = *inL++;
		R = mono ? 0 : *inR++;
		L *= gain;
		R *= gain;

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
		double dB = gr - trsh;

		if(dB < 0.0) dB = 0.0;

		dB += DC_OFFSET;
		if(dB > _env)
			_env = dB + _atk * (_env - dB);
		else
			_env = dB + _rls * (_env - dB);
		dB = _env - DC_OFFSET;
			
		gr = dB2lin(-dB);
				
		(*outL++) = static_cast<Sample>(L * gr);
		(*outR++) = static_cast<Sample>(R * gr);

		gainReduction.store(grh);
// TODO: store dB
	}
}

void SignalNoiseLimiter::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseLimiter::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// Editor
juce::AudioProcessorEditor* SignalNoiseLimiter::createEditor()
{
	return new SignalNoiseLimiterGUI (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseLimiter();
}
