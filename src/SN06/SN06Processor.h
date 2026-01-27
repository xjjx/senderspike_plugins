//------------------------------------------------------------------------------------
//
//	file:		sn_06e.h
//
//	purpose:	SN06 op-amp effect
//
//  authors:	2020 Oto Spál
//
//------------------------------------------------------------------------------------

#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <cmath>
#include "sn_core.h"

enum
{
	SNE_GAIN = 0,	// drive
	SNE_TRIM,		// input volume
	SNE_VOLU,		// output volume
	SNE_SIZE,		// num of params
};

class SN06Processor : public juce::AudioProcessor
{
public:
	SN06Processor();
	~SN06Processor() override = default;

	// JUCE overrides
	void prepareToPlay(double sampleRate, int samplesPerBlock) override;
	void releaseResources() override {}

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }

	const juce::String getName() const override { return "SN06"; }
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

private:
    // ================= DSP STATE =================
    double _norm;
    double _erfL;
    double _erfR;
    bool   _mono = false;

    foHPF _hpfL;
    foHPF _hpfR;

    // ================= PARAMETERS =================
    juce::AudioProcessorValueTreeState parameters;

    template <typename Sample>
    void processImpl(Sample** in, Sample** out, int numSamples);
};
