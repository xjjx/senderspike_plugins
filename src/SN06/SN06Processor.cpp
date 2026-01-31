//------------------------------------------------------------------------------------
//
//	file:		sn_06e.cpp
//
//	purpose:	SN06 op-amp effect
//				- TODO: switch to invert/keep phase of feedback
//
//	authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------

#include "SN06Processor.h"
#include "SN06Editor.h"

// ----------------------
// Constructor
// ----------------------
SN06Processor::SN06Processor()
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

	// Now add parameters using ParameterInfos
	for (auto& info : parameterInfos)
	{
		// Define the range in dB instead of normalized
		juce::NormalisableRange<float> range(info.minDb, info.maxDb);

		// Create parameter with default in dB
		parameters.createAndAddParameter(
			std::make_unique<juce::AudioParameterFloat>(
				info.paramID,		  // parameter ID
				info.paramID,		  // name shown in host
				range,				  // now in dB
				info.defaultDb		  // default in dB
			)
		);
	}

	parameters.state = juce::ValueTree("PARAMETERS"); // finalize
}

bool SN06Processor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
	return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
		&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

// ----------------------
// Playback
// ----------------------
void SN06Processor::prepareToPlay(double sampleRate, int /*samplesPerBlock*/)
{
	_hpfL.setup(15.0, sampleRate);
	_hpfR.setup(15.0, sampleRate);
}

// ----------------------
// Audio processing
// ----------------------
template <typename Sample>
void SN06Processor::processImpl(Sample** in, Sample** out, int numSamples)
{
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
		L = *in[0]++;
		R = (!_mono && in[1] != nullptr) ? *in[1]++ : 0.0;

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
		*out[0]++ = static_cast<Sample>(L);
		if (out[1] != nullptr)
			*out[1]++ = static_cast<Sample>(R);

		float inAbs, outAbs;
		if(_mono)
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

void SN06Processor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
	float* in[2]  = { buffer.getWritePointer(0), buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr };
	float* out[2] = { in[0], in[1] };
	processImpl(in, out, buffer.getNumSamples());
}

void SN06Processor::processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&)
{
	double* in[2]  = { buffer.getWritePointer(0), buffer.getNumChannels() > 1 ? buffer.getWritePointer(1) : nullptr };
	double* out[2] = { in[0], in[1] };
	processImpl(in, out, buffer.getNumSamples());
}

// ----------------------
// State
// ----------------------
void SN06Processor::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SN06Processor::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml)
		parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

// Editor
juce::AudioProcessorEditor* SN06Processor::createEditor()
{
	return new SN06Editor (*this);
}

// ----------------------
// JUCE Plugin entry point
// ----------------------
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new SN06Processor();
}
