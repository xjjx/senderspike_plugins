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
#include "SignalNoiseCompressor.h"
#include "SignalNoiseCompressorGUI.h"

//------------------------------------------------------------------------------------
// class SignalNoiseCompressor
//------------------------------------------------------------------------------------

SignalNoiseCompressor::SignalNoiseCompressor()
    : SignalNoiseFX(createLayout(gParams, SNE_SIZE), gParams, SNE_SIZE)
{
	_TdBL = DC_OFFSET;
	_TdBR = DC_OFFSET;
	_atk = 0;
	_rls = 0;
	_fbL = DC_OFFSET;
	_fbR = DC_OFFSET;
	_rnd.seed();

	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.addParameterListener (gParams[i].id, this);
}

SignalNoiseCompressor::~SignalNoiseCompressor()
{
	for (int i = 0; i < SNE_SIZE; ++i)
		parameters.removeParameterListener (gParams[i].id, this);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::prepareToPlay(double /*newSampleRate*/, int /*samplesPerBlock*/)
{
	juce::FloatVectorOperations::disableDenormalisedNumberSupport();

	setupEnvelope();
	setupSidechain();
#ifdef SN01G
	((SignalNoiseCompressorGUI*)editor)->setupMeter(fs);
#endif
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::parameterChanged (const juce::String& id, float /*newValue*/)
{
	const int at = paramIdToIndex (id);
	if (at < 0)
		return;

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

void SignalNoiseCompressor::setupEnvelope()
{
	double attkn = getParamValue(SNE_ATTK);
	double relsn = getParamValue(SNE_RELS);

	double attk = gParams[SNE_ATTK].normToCubic(attkn);
	double rels = gParams[SNE_RELS].normToCubic(relsn);

	_atk = exp(-1000.0 / (attk * sampleRate));
	_rls = exp(-1000.0 / (rels * sampleRate));
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::setupSidechain()
{
	switch(getParamChoice(SNE_MODE))
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

inline double processCurve(double linked, double trsh,
                                           double func, double knee,
                                           double kh, double pushNoise)
{
	double kB = juce::Decibels::gainToDecibels(linked + DC_OFFSET);
	double dB = kB - trsh;

    dB += pushNoise;

	double y0 = 0.0;
	if (dB <= -kh)
		y0 = kB;
	else if (dB < kh)
		y0 = kB + func * ((dB + kh) * (dB + kh)) / (2.0 * knee);
	else
		y0 = kB + func * dB;

	return y0 - kB;
}

inline double runEnvelope(double dB, double& TdB, double atk, double rls)
{
	dB += DC_OFFSET;
	if (dB > TdB)
		TdB = (atk * TdB) + ((1.0 - atk) * dB);
	else
		TdB = (rls * TdB) + ((1.0 - rls) * dB);
	return TdB - DC_OFFSET;
}

template <typename Sample>
void SignalNoiseCompressor::processImpl(juce::AudioBuffer<Sample>& buffer)
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
	const float ratioParam = getParamNorm(SNE_FUNC);
	const float trshParam  = getParamValue(SNE_TRSH);
	const float kneeParam  = getParamNorm(SNE_KNEE);
	const float kwdtParam  = getParamValue(SNE_KWDT);
	const float compParam  = getParamNorm(SNE_COMP);
	const float linkAmount = getParamNorm(SNE_LINK);

	const double gain = dB2lin(gainParam);
	const int fbck = getParamChoice(SNE_FBCK);
	const int mode = getParamChoice(SNE_MODE);
	const int push = getParamChoice(SNE_PUSH);
	const int detMode = getParamChoice(SNE_DMODE);
	const double func = sqrt(ratioParam);
	const double trsh = trshParam - (9.0 * push);
	const double knee = kneeParam * kwdtParam;
	const float dry	= compParam;
	const double wet = (1.0 - dry) * gain;
	const double kh	= knee / 2.0;
	float maxMeterGr = 0.0f;

	for (int n = 0; n < numSamples; ++n)
	{
		double L = *inL++;
		double R = mono ? 0 : *inR++;

		// --- sidechain source ---
		//SC -> FF/FB + filter
		double fL = fbck ? _fbL : L;
		double fR = fbck ? _fbR : R;

		// --- sidechain filter ---
		if(mode)
		{
			fL = _hsL.run(_lsL.run(fL));
			fR = _hsR.run(_lsR.run(fR));
		}

		// --- rectifier -> thresh + push
		double rL = std::abs(fL);
		double rR = std::abs(fR);

		// --- stereo link / detection mode ---
		double env;
		switch (detMode)
		{
			case 1: env = 0.5 * (rL + rR);						break; // Average
			case 2: env = std::sqrt(0.5 * (rL * rL + rR * rR));	break; // Power
			default: env = juce::jmax(rL, rR);					break; // Max
		}

		double linkedL = mono ? rL : rL + linkAmount * (env - rL);
		double linkedR = mono ? rL : rR + linkAmount * (env - rR);

		// --- ratio + knee ---
		double noise = push ? _rnd.pink() * 2.0 * push : 0.0;
		double dBL = processCurve(linkedL, trsh, func, knee, kh, noise);
		double dBR = processCurve(linkedR, trsh, func, knee, kh, noise);

		// --- envelope ---
		dBL = runEnvelope(dBL, _TdBL, _atk, _rls);
		dBR = runEnvelope(dBR, _TdBR, _atk, _rls);

		// --- gain ---
		double grL = dB2lin(-dBL);
		double grR = dB2lin(-dBR);

		_fbL = L * grL;
		_fbR = R * grR;

		//output		
		(*outL++) = static_cast<Sample>((_fbL * wet) + (dry * L));
		if (!mono)
			(*outR++) = static_cast<Sample>((_fbR * wet) + (dry * R));

		maxMeterGr = std::max(maxMeterGr, (float)juce::jmax(dBL, dBR));
	}
	gainReduction.store(maxMeterGr);
}

void SignalNoiseCompressor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseCompressor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// Editor
juce::AudioProcessorEditor* SignalNoiseCompressor::createEditor()
{
	return new SignalNoiseCompressorGUI (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseCompressor();
}
