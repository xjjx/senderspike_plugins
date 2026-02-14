//------------------------------------------------------------------------------------
//
//	file:		sn_vsti.h
//
//	purpose:	Signal Noise VST 2.4 plugin interface
//				- define SNFX_NO_SIGNAL_DEFAULT_GUI to bypass VSTGUI SDK when using
//				  SignalNoiseFX interface
//
//	authors:	2019 - 2025 Oto Spál
//
//------------------------------------------------------------------------------------


#pragma once

#include <vector>
#include <juce_audio_processors/juce_audio_processors.h>

enum class ParamType
{
	Normalized,	// legacy 0..1
	Decibel,	// linear gain in dB
	Hertz,		// linear frequency in Hz
	Bandwidth,	// linear in oct
	Percent,
	Cubic,
	Choice		// generic stepped parameter (EQ frequencies, etc)
};

struct ParamDesc
{
	ParamType type;		// type of parameter
	const char* id;		// unique host/preset ID
	const char* name;	// display name
	const char* unit;	// "dB", "ms", "", etc

	// only for continuous params
	float minValue = 0.0f;
	float maxValue = 1.0f;

	float defaultValue = 0.0f;
	std::vector<const char*> choiceLabels{};	// non-empty -> choice param

	float normToCubic(float normalized) const {
		float x3 = normalized * normalized * normalized;
		return minValue + ((maxValue - minValue) * x3);
	}

	float cubicToNorm(float cubic) const {
		float norm = std::cbrt((cubic - minValue) / (maxValue - minValue));
		return juce::jlimit(0.0f, 1.0f, norm);
	}
};

//------------------------------------------------------------------------------------
class SignalNoiseFX
	: public juce::AudioProcessor,
	  public juce::AudioProcessorValueTreeState::Listener
{
public:
	SignalNoiseFX(
		juce::AudioProcessorValueTreeState::ParameterLayout layout)
		: AudioProcessor(
			BusesProperties()
				.withInput	("Input",  juce::AudioChannelSet::stereo(), true)
				.withOutput ("Output", juce::AudioChannelSet::stereo(), true)
		  ),
		  parameters(*this, nullptr, "PARAMS", std::move(layout))
	{}

	~SignalNoiseFX() override = default;

	// AudioProcessor boilerplate
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }
	bool supportsDoublePrecisionProcessing() const override { return true; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const juce::String getProgramName(int) override { return {}; }
	void changeProgramName(int, const juce::String&) override {}

	bool hasEditor() const override { return false; }

	void releaseResources() override {}

	const juce::AudioProcessorValueTreeState& getParameters() const { return parameters; }
	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	bool isBusesLayoutSupported(const BusesLayout& layouts) const override;

	void getStateInformation(juce::MemoryBlock& destData) override;
	void setStateInformation(const void* data, int sizeInBytes) override;

protected:
	double sampleRate = 44100.0;
	juce::AudioProcessorValueTreeState parameters;

	juce::AudioProcessorValueTreeState::ParameterLayout
	createLayout(const ParamDesc* paramArray, int numParams);
};
