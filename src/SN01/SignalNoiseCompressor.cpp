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
	_TdB = DC_OFFSET;
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

template <typename Sample>
void SignalNoiseCompressor::processImpl(juce::AudioBuffer<Sample>& buffer)
{
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

	const double gain = dB2lin(gainParam);
	const int fbck = getParamChoice(SNE_FBCK);
	const int mode = getParamChoice(SNE_MODE);
	const int push = getParamChoice(SNE_PUSH);
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

		//SC -> FF/FB + filter
		double fL, fR;
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
		double rL = std::abs(fL);
		double rR = std::abs(fR);
		double kB = juce::Decibels::gainToDecibels(juce::jmax(rL, rR) + DC_OFFSET);
		double dB = kB - trsh;
		if(push) dB += _rnd.pink() * 2.0 * push;

		//ratio + knee
		double y0 = 0;		
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
		double gr = dB2lin(-dB);

		_fbL = L * gr;
		_fbR = R * gr;

		//output		
		(*outL++) = static_cast<Sample>((_fbL * wet) + (dry * L));
		if (!mono)
			(*outR++) = static_cast<Sample>((_fbR * wet) + (dry * R));

		maxMeterGr = std::max(maxMeterGr, (float)dB);
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
