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
#include "SignalNoiseTapedeck.h"
//#include "SignalNoiseTapedeckEditor.h"

#ifdef SN03G
#include <sn_03g.h>
#endif

//------------------------------------------------------------------------------------
// class SignalNoiseTapedeck
//------------------------------------------------------------------------------------

SignalNoiseTapedeck::SignalNoiseTapedeck()
    : SignalNoiseFX(
          "SignalNoiseTapedeck",
          createLayout(gParams, SNE_SIZE)
	  )
{
	_rand.seed();

	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.addParameterListener (gParams[i].id, this);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	sampleRate = newSampleRate;

	setupTapeheads();
	setupEqualizer();

#ifdef SN03G
	((SignalNoiseTapedeckGUI*)editor)->setupMeterFilter(fs);
#endif
}

//------------------------------------------------------------------------------------
int SignalNoiseTapedeck::paramIdToIndex (const juce::String& id)
{
	for (int i = 0; i < SNE_SIZE; ++i)
		if (id == gParams[i].id)
			return i;

	return -1;
}

void SignalNoiseTapedeck::parameterChanged (const juce::String& id, float /*newValue*/)
{
	const int at = paramIdToIndex (id);
	if (at < 0)
		return;

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
	double of = getParamNorm(SNE_ATTN) * 2;
	double hd = getParamNorm(SNE_HEAD);
	double hv = getParamNorm(SNE_BUMP);
	double sc = 4 - hd;
	double dB = hv * sc + sc - of;
	double fc = hd * 40 + 65;
	double lf = hd * 10 + 10;
	
	if(getParamNorm(SNE_HBON) > 0.5f)
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
	double eq = getParamNorm(SNE_EQSC);
	double hi1 = getParamNorm(SNE_RCHI) * 28 - 14;
	double lo1 = getParamNorm(SNE_RCLO) * 20 - 10;
	double hi2 = getParamNorm(SNE_RPHI) * 28 - 14;
	double lo2 = getParamNorm(SNE_RPLO) * 20 - 10;

	if(eq < sv)
	{
		fc = 3180;
	}
	else
	{
		if(getParamNorm(SNE_LOON) < 0.5f)
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

template <typename Sample>
void SignalNoiseTapedeck::processImpl(juce::AudioBuffer<Sample>& buffer)
{
	const int numSamples  = buffer.getNumSamples();
	const int numChannels = buffer.getNumChannels();
	const bool mono       = (numChannels == 1);

	Sample* inL  = buffer.getWritePointer(0);
	Sample* inR  = mono ? nullptr : buffer.getWritePointer(1);
	Sample* outL = inL;
	Sample* outR = inR;

	const float gainParam  = getParamNorm(SNE_GAIN);
	const float trimParam  = getParamNorm(SNE_TRIM);
	const float hissParam  = getParamNorm(SNE_HISS);
	const float noiseParam = getParamNorm(SNE_NOIS);
	const float pathParam  = getParamNorm(SNE_PATH);

	const double iG = dB2lin(trimParam * 48 - 24);
	const double oG = dB2lin(gainParam * 48 - 24);
	const double hG = dB2lin(hissParam * 60 - 96);

	const bool nois = noiseParam > 0.5;

	const int id = pathParam > 0.5 ? 1 : 0;

	for (int n = 0; n < numSamples; ++n)
	{
		double L, R, vuL[2], vuR[2];

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

		float vuAbs = mono ? std::abs(vuL[id]) : (std::abs(vuL[id]) + std::abs(vuR[id])) * 0.5;
		vuLevel.store(vuAbs);
	}
}

void SignalNoiseTapedeck::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseTapedeck::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// Editor
juce::AudioProcessorEditor* SignalNoiseTapedeck::createEditor()
{
//	return new SignalNoiseOpampEditor (*this);
	return nullptr;
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseTapedeck();
}
