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
#include "SignalNoiseTapedeckGUI.h"

//------------------------------------------------------------------------------------
// class SignalNoiseTapedeck
//------------------------------------------------------------------------------------

SignalNoiseTapedeck::SignalNoiseTapedeck()
    : SignalNoiseFX(createLayout(gParams, SNE_SIZE), gParams, SNE_SIZE)
{
	_rand.seed();

	for (int i = 0; i < SNE_SIZE; ++i)
	{
		if (i == SNE_HOLD || i == SNE_ROOM)
			continue;
		parameters.addParameterListener (gParams[i].id, this);
	}
}

SignalNoiseTapedeck::~SignalNoiseTapedeck()
{
	for (int i = 0; i < SNE_SIZE; ++i)
	{
		if (i == SNE_HOLD || i == SNE_ROOM)
			continue;
		parameters.removeParameterListener (gParams[i].id, this);
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	juce::FloatVectorOperations::disableDenormalisedNumberSupport();

	sampleRate = newSampleRate;

	setupTapeheads();
	setupEqualizer();
	vuFilter.prepare(sampleRate);
}

//------------------------------------------------------------------------------------

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
	}
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

void SignalNoiseTapedeck::setupTapeheads()
{
	double fs = sampleRate;
	int of = getParamChoice(SNE_ATTN); // 0,1,2
	double hd = getParamNorm(SNE_HEAD);
	double hv = getParamNorm(SNE_BUMP);
	double sc = 4 - hd;
	double dB = hv * sc + sc - of;
	double fc = hd * 40 + 65;
	double lf = hd * 10 + 10;
	
	if(getParamChoice(SNE_HBON) == 1) // 1 - On
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
	double fc, fs = sampleRate;
	int eq = getParamChoice(SNE_EQSC);
	double hi1 = getParamValue(SNE_RCHI);
	double lo1 = getParamValue(SNE_RCLO);
	double hi2 = getParamValue(SNE_RPHI);
	double lo2 = getParamValue(SNE_RPLO);

	if(eq == 0) // NAB
	{
		fc = 3180;
	}
	else
	{
		if(getParamChoice(SNE_LOON) == 0) // 0 - off
		{
			lo1 = 0;
			lo2 = 0;
		}
		fc = (eq == 2) ? 9100 : 4550; // AES : IEC
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
	juce::ScopedNoDenormals noDenormals;

	const int numSamples  = buffer.getNumSamples();
	const int numChannels = buffer.getNumChannels();
	const bool mono       = (numChannels == 1);

	Sample* inL  = buffer.getWritePointer(0);
	Sample* inR  = mono ? nullptr : buffer.getWritePointer(1);
	Sample* outL = inL;
	Sample* outR = inR;

	const float gainParam  = getParamValue(SNE_GAIN);
	const float trimParam  = getParamValue(SNE_TRIM);
	const float hissParam  = getParamValue(SNE_HISS);

	const double iG = dB2lin(trimParam);
	const double oG = dB2lin(gainParam);
	const double hG = dB2lin(hissParam);

	const bool nois = getParamChoice(SNE_NOIS) == 1; // 1 - On

	const int id = getParamChoice(SNE_PATH) == 0 ? 0 : 1; // 0 - Input, 1 - Output
	float vuMaxLevel = 0.0f;
	float lastFiltered = 0.0f;

	for (int n = 0; n < numSamples; ++n)
	{
		double L, R;
		Sample vuL[2], vuR[2];

		vuL[0] = L = *inL++;
		vuR[0] = R = mono ? 0 : *inR++;
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

		(*outL++) = vuL[1] = static_cast<Sample>(L * oG);
		(*outR++) = vuR[1] = static_cast<Sample>(R * oG);

		float vuAbs = mono ? std::abs(vuL[id]) : (std::abs(vuL[id]) + std::abs(vuR[id])) * 0.5;

		lastFiltered = (float) vuFilter.process(vuAbs);
		vuMaxLevel = std::max(vuMaxLevel, vuAbs);
	}
	vuLevel.store(lastFiltered);
	peakLevel.store(vuMaxLevel);
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
	return new SignalNoiseTapedeckGUI(*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseTapedeck();
}
