//------------------------------------------------------------------------------------
//
//	file:		sn_06e.cpp
//
//	purpose:	SN06 op-amp effect
//				- TODO: switch to invert/keep phase of feedback
//
//  authors:	2020 Oto Spál
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
  parameters(*this, nullptr, "PARAMETERS",
    {
        std::make_unique<juce::AudioParameterFloat>("gain", "Gain", 0.0f, 1.0f, 0.25f),
        std::make_unique<juce::AudioParameterFloat>("trim", "Trim", 0.0f, 1.0f, 0.50f),
        std::make_unique<juce::AudioParameterFloat>("volume", "Volume", 0.0f, 1.0f, 0.75f)
    })
{
    _erfL = 0.0;
    _erfR = 0.0;
    _norm = 1e-15;
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
	const float gainParam   = *parameters.getRawParameterValue("gain");
	const float trimParam   = *parameters.getRawParameterValue("trim");
	const float volumeParam = *parameters.getRawParameterValue("volume");

	// Parameter mapping (unchanged)
	const double trim  = dB2lin(trimParam   * 40.0 - 20.0);
	const double volu  = dB2lin(volumeParam * 64.0 - 48.0);
	const double drive = dB2lin(gainParam   * 32.0 - 8.0); // <-- RESTORED DRIVE

	// Crossfade coefficients (NO gain here)
	const double fact = gainParam * 0.55;
	const double invf = 1.0 - fact;

	double L, R, eL, eR, iL, iR;

	_norm = -_norm;

	for (int n = 0; n < numSamples; ++n)
	{
		// Input
		iL = L = *in[0]++;
		iR = R = (in[1] != nullptr) ? *in[1]++ : 0.0;

		// Trim
		L *= trim;
		R *= trim;

		if (_mono)
		    R = 0.0;

		// Previous nonlinear state
		eL = _erfL;
		eR = _erfR;

		// Nonlinear stage
		_erfL = std::erf((L * drive) + _norm);
		_erfR = std::erf((R * drive) + _norm);

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

		inputLevel.store(std::max(inputLevel.load(), inAbs));
		outputLevel.store(std::max(outputLevel.load(), outAbs));
		peakLevel.store(std::max(peakLevel.load(), outAbs));
	}

	inputLevel.store(inputLevel.load() * 0.95f);
	outputLevel.store(outputLevel.load() * 0.95f);
	peakLevel.store(peakLevel.load() * 0.90f);
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
