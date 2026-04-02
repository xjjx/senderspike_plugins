//------------------------------------------------------------------------------------
//
//	file:		sn_06e.h
//
//	purpose:	SignalNoiseOpamp op-amp effect
//
//	authors:	2020 Oto Spál
//				2026 -      Pawel ,Xj' Piatek (JUCE port)
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include <atomic>
#include "sn_core.h"
#include "SignalNoiseFX.h"

enum
{
	SNE_GAIN = 0,	// drive
	SNE_TRIM,	// input volume
	SNE_VOLU,	// output volume
	SNE_SIZE,	// num of params
};

static const ParamDesc gParams[] =
{
	{ ParamType::Decibel, "gain",   "Gain",   "dB", -8.0f, 24.0f, 0.0f },
	{ ParamType::Decibel, "trim",   "Trim",   "dB", -20.0f, 20.0f, 0.0f },
	{ ParamType::Decibel, "volume", "Volume", "dB", -48.0f, 16.0f, 0.0f }
};

class SignalNoiseOpamp : public SignalNoiseFX
{
public:
	SignalNoiseOpamp();
	~SignalNoiseOpamp() override = default;

	// JUCE overrides
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&) override;

	const juce::String getName() const override { return JucePlugin_Name; }

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }

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

	template <typename Sample>
	void processImpl(juce::AudioBuffer<Sample>& buffer);
};
