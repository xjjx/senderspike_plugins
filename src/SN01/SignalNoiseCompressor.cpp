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
//#include "SignalNoiseCompressorEditor.h"

//------------------------------------------------------------------------------------
// class SignalNoiseCompressor
//------------------------------------------------------------------------------------

SignalNoiseCompressor::SignalNoiseCompressor()
    : SignalNoiseFX(
          "SignalNoiseCompressor",
          createLayout(gParams, SNE_SIZE)
      )
{
	_TdB = DC_OFFSET;
	_atk = 0;
	_rls = 0;
	_fbL = DC_OFFSET;
	_fbR = DC_OFFSET;
	_rnd.seed();

#ifdef SN01G
	editor = new SignalNoiseCompressorGUI(this);
#endif
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::prepareToPlay(double newSampleRate, int /*samplesPerBlock*/)
{
	setupEnvelope();
	setupSidechain();
#ifdef SN01G
	((SignalNoiseCompressorGUI*)editor)->setupMeter(fs);
#endif
}

//------------------------------------------------------------------------------------
int SignalNoiseCompressor::paramIdToIndex (const juce::String& id)
{
	for (int i = 0; i < SNE_SIZE; ++i)
		if (id == gParams[i].id)
			return i;

	return -1;
}

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
	double attk = getParamNorm(SNE_ATTK);
	double rels = getParamNorm(SNE_RELS);
	attk = (attk * attk * attk * 29.97) + 0.03;
	rels = (rels * rels * rels * 1950.0) + 50.0;
	_atk = exp(-1000.0 / (attk * sampleRate));
	_rls = exp(-1000.0 / (rels * sampleRate));
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressor::setupSidechain()
{
	switch(GetSwitch(getParamNorm(SNE_MODE)))
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
void SignalNoiseCompressor::processImpl (Sample** in, Sample** out, int numSamples)
{
	const bool mono = (getTotalNumInputChannels() == 1);

	Sample* inL  = in[0];
	Sample* inR  = (mono ? nullptr : in[1]);
	Sample* outL = out[0];
	Sample* outR = (mono ? nullptr : out[1]);

	const float gainParam  = getParamNorm(SNE_GAIN);
	const float fbckParam  = getParamNorm(SNE_FBCK);
	const float ratioParam = getParamNorm(SNE_FUNC);
	const float trshParam  = getParamNorm(SNE_TRSH);
	const float kneeParam  = getParamNorm(SNE_KNEE);
	const float kwdtParam  = getParamNorm(SNE_KWDT);
	const float compParam  = getParamNorm(SNE_COMP);
	const float modeParam  = getParamNorm(SNE_MODE);
	const float pushParam  = getParamNorm(SNE_PUSH);

	const double gain = dB2lin(gainParam * 24.0);
	const int fbck = fbckParam > 0.5 ? 1 : 0;
	const int mode = GetSwitch(modeParam);
	const int push = GetSwitch(pushParam);
	const double func = sqrt(ratioParam);
	const double trsh = trshParam * -40.0 - (9.0 * push);
	const double knee = kneeParam * kwdtParam * 24.0;
	const float dry	= compParam;
	const double wet = (1.0 - dry) * gain;
	const double kh	= knee / 2.0;

	for (int n = 0; n < numSamples; ++n)
	{
		double L = *inL++;
		double R = mono ? L : *inR++;

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
		double kB = lin2dB(getmax(rL, rR) + DC_OFFSET);
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
		(*outL++) = (Sample)((_fbL * wet) + (dry * L));
		if (mono)
			(*outR++) = (Sample)((_fbR * wet) + (dry * R));

#ifdef SN01G
		((SignalNoiseCompressorGUI*)editor)->trackMeter(dB);
#endif
	}
}

void SignalNoiseCompressor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	float* in[2]  = { buffer.getWritePointer(0), buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr };
	float* out[2] = { in[0], in[1] };
	processImpl(in, out, buffer.getNumSamples());
}

void SignalNoiseCompressor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	double* in[2]  = { buffer.getWritePointer(0), buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr };
	double* out[2] = { in[0], in[1] };
	processImpl(in, out, buffer.getNumSamples());
}

// ----------------------
// State
// ----------------------
void SignalNoiseCompressor::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SignalNoiseCompressor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml)
		parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

// Editor
juce::AudioProcessorEditor* SignalNoiseCompressor::createEditor()
{
//	return new SignalNoiseOpampEditor (*this);
	return nullptr;
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseCompressor();
}
