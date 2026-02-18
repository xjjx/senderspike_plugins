//------------------------------------------------------------------------------------
//
//	file:		sn_06e.cpp
//
//	purpose:	SignalNoiseOpamp op-amp effect
//				- TODO: switch to invert/keep phase of feedback
//
//	authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------

#include "SignalNoiseOpamp.h"
#include "SignalNoiseOpampGUI.h"

// ----------------------
// Constructor
// ----------------------
SignalNoiseOpamp::SignalNoiseOpamp()
: SignalNoiseFX(createLayout(gParams, SNE_SIZE))
{
	_erfL = 0.0;
	_erfR = 0.0;
	_norm = 1e-15;

//	for (int i = 0; i < SNE_SIZE; ++i)
//		parameters.addParameterListener (gParams[i].id, this);
}

bool SignalNoiseOpamp::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
		&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

// ----------------------
// Playback
// ----------------------
void SignalNoiseOpamp::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	_hpfL.setup(15.0, sampleRate);
	_hpfR.setup(15.0, sampleRate);
}

void SignalNoiseOpamp::parameterChanged (const juce::String& id, float /*newValue*/)
{
	return;
}

// ----------------------
// Audio processing
// ----------------------
template <typename Sample>
void SignalNoiseOpamp::processImpl(juce::AudioBuffer<Sample>& buffer)
{
	const int numSamples  = buffer.getNumSamples();
	const int numChannels = buffer.getNumChannels();
	const bool mono       = (numChannels == 1);

	Sample* inL  = buffer.getWritePointer(0);
	Sample* inR  = mono ? nullptr : buffer.getWritePointer(1);
	Sample* outL = inL;
	Sample* outR = inR;

	// Read parameters once per block
	const float gainParam	= *parameters.getRawParameterValue("gain");
	const float trimParam	= *parameters.getRawParameterValue("trim");
	const float volumeParam = *parameters.getRawParameterValue("volume");
	const float gainParamNorm = parameters.getParameter("gain")->getValue();

	const double drive = dB2lin(gainParam);
	const double trim  = dB2lin(trimParam);
	const double volu  = dB2lin(volumeParam);

	// Crossfade coefficients
	double fact = gainParamNorm * 0.55;
	double invf = 1.0 - fact;
	fact *= drive;
	invf *= drive;

	double L, R, eL, eR, iL, iR;

	_norm = -_norm;

	for (int n = 0; n < numSamples; ++n)
	{
		// Input
		L = *inL++;
		R = mono ? 0 : *inR++;

		// Trim
		L *= trim;
		R *= trim;

		// Copy for input meter
		iL = L;
		iR = R;

		// Previous nonlinear state
		eL = _erfL;
		eR = _erfR;

		// Nonlinear stage
		_erfL = std::erf(L + _norm);
		_erfR = std::erf(R + _norm);

		// Memory crossfade
		L = eL * fact + L * invf;
		R = eR * fact + R * invf;

		// Output stage
		L = _hpfL.run(L) * volu;
		R = _hpfR.run(R) * volu;

		// Write output
		(*outL++) = static_cast<Sample>(L);

		float inAbs, outAbs;
		if(mono)
		{
			inAbs  = std::abs(iL);
			outAbs = std::abs(L);
		}
		else
		{
			(*outR++) = static_cast<Sample>(R);
			inAbs  = (std::abs(iL) + std::abs(iR)) * 0.5;
			outAbs = (std::abs(L) + std::abs(L)) * 0.5;
		}

		inputLevel.store(inAbs);
		outputLevel.store(outAbs);
	}
}

void SignalNoiseOpamp::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseOpamp::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// ----------------------
// State
// ----------------------
void SignalNoiseOpamp::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SignalNoiseOpamp::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml)
		parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

// Editor
juce::AudioProcessorEditor* SignalNoiseOpamp::createEditor()
{
	return new SignalNoiseOpampGUI (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseOpamp();
}
