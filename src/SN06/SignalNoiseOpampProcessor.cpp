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

#include "SignalNoiseOpampProcessor.h"
#include "SignalNoiseOpampEditor.h"

// ----------------------
// Constructor
// ----------------------
SignalNoiseOpampProcessor::SignalNoiseOpampProcessor()
: AudioProcessor(
	BusesProperties()
		.withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
		.withOutput ("Output", juce::AudioChannelSet::stereo(), true)
  ),
  parameters(*this, nullptr) // minimal constructor
{
	_erfL = 0.0;
	_erfR = 0.0;
	_norm = 1e-15;
	float step = 0.01f;

	// Now add parameters using ParameterInfos
	for (auto& info : parameterInfos)
	{
		// Define the range in dB instead of normalized
		juce::NormalisableRange<float> range(info.minDb, info.maxDb, step);

		// Create parameter with default in dB
		parameters.createAndAddParameter(
			std::make_unique<juce::AudioParameterFloat>(
				info.paramID,	// parameter ID
				info.paramID,	// name shown in host
				range,			// now in dB
				info.defaultDb,	// default in dB
				info.unit		// unit	
			)
		);
	}

	parameters.state = juce::ValueTree("PARAMETERS"); // finalize
}

bool SignalNoiseOpampProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
		&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

// ----------------------
// Playback
// ----------------------
void SignalNoiseOpampProcessor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	_hpfL.setup(15.0, sampleRate);
	_hpfR.setup(15.0, sampleRate);
}

// ----------------------
// Audio processing
// ----------------------
template <typename Sample>
void SignalNoiseOpampProcessor::processImpl(juce::AudioBuffer<Sample>& buffer)
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

	const auto& gainInfo   = parameterInfos[SNE_GAIN]; // gain
//	const auto& trimInfo   = parameterInfos[SNE_TRIM]; // trim
//	const auto& volumeInfo = parameterInfos[SNE_VOLU]; // volume

	const double drive = dB2lin(gainParam);
	const double trim  = dB2lin(trimParam);
	const double volu  = dB2lin(volumeParam);

	// Crossfade coefficients
	double fact = gainInfo.dbToNormalized(gainParam) * 0.55;
	double invf = 1.0 - fact;
	fact *= drive;
	invf *= drive;

	double L, R, eL, eR, iL, iR;

	_norm = -_norm;

	for (int n = 0; n < numSamples; ++n)
	{
		// Input
		L = *inL++;
		R = mono ? L : *inR++;

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
		if (mono)
			(*outR++) = static_cast<Sample>(R);

		float inAbs, outAbs;
		if(mono)
		{
			inAbs  = std::abs(iL);
			outAbs = std::abs(L);
		}
		else
		{
			inAbs  = (std::abs(iL) + std::abs(iR)) * 0.5;
			outAbs = (std::abs(L) + std::abs(L)) * 0.5;
		}

		inputLevel.store(inAbs);
		outputLevel.store(outAbs);
	}
}

void SignalNoiseOpampProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	processImpl<float>(buffer);
}

void SignalNoiseOpampProcessor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	processImpl<double>(buffer);
}

// ----------------------
// State
// ----------------------
void SignalNoiseOpampProcessor::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SignalNoiseOpampProcessor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml)
		parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

// Editor
juce::AudioProcessorEditor* SignalNoiseOpampProcessor::createEditor()
{
	return new SignalNoiseOpampEditor (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SignalNoiseOpampProcessor();
}
