//------------------------------------------------------------------------------------
//
//	file:		sn_06e.h
//
//	purpose:	SignalNoiseOpamp op-amp effect
//
//	authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <atomic>
#include "ParameterInfo.h"
#include "sn_core.h"

enum
{
	SNE_GAIN = 0,	// drive
	SNE_TRIM,	// input volume
	SNE_VOLU,	// output volume
	SNE_SIZE,	// num of params
};

static const std::vector<ParameterInfo> parameterInfos = {
	{ -8.0f, 24.0f, 0.0f, "gain", "dB" },
	{ -20.0f, 20.0f, 0.0f, "trim", "dB" },
	{ -48.0f, 16.0f, 0.0f, "volume", "dB" }
};

class SignalNoiseOpampProcessor : public juce::AudioProcessor
{
public:
	SignalNoiseOpampProcessor();
	~SignalNoiseOpampProcessor() override = default;

	// JUCE overrides
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override {}

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }
	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	const juce::String getName() const override { return "SignalNoiseOpamp"; }
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const juce::String getProgramName(int) override { return {}; }
	void changeProgramName(int, const juce::String&) override {}
	bool isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout& layouts) const override;

	void getStateInformation(juce::MemoryBlock&) override;
	void setStateInformation(const void*, int) override;

	float getInputLevel()  const noexcept { return inputLevel.load(); }
	float getOutputLevel() const noexcept { return outputLevel.load(); }

private:
	// ================= DSP STATE =================
	double _norm;
	double _erfL;
	double _erfR;
	bool   _mono = false;

	foHPF _hpfL;
	foHPF _hpfR;

	// ================= METERS =================
	std::atomic<float> inputLevel  { 0.0f };
	std::atomic<float> outputLevel { 0.0f };

	// ================= PARAMETERS =================
	juce::AudioProcessorValueTreeState parameters;

	template <typename Sample>
	void processImpl(Sample** in, Sample** out, int numSamples);
};
