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

#include "ParameterInfo.h"


//------------------------------------------------------------------------------------
class SignalNoiseFX
	: public juce::AudioProcessor,
	  public juce::AudioProcessorValueTreeState::Listener
{
public:
	SignalNoiseFX(
		const juce::String& name,
		juce::AudioProcessorValueTreeState::ParameterLayout layout)
		: AudioProcessor(
			BusesProperties()
				.withInput	("Input",  juce::AudioChannelSet::stereo(), true)
				.withOutput ("Output", juce::AudioChannelSet::stereo(), true)
		  ),
		  parameters(*this, nullptr, "PARAMS", std::move(layout)),
		  pluginName(name)
	{}

	~SignalNoiseFX() override = default;

	// AudioProcessor boilerplate
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const juce::String getProgramName(int) override { return {}; }
	void changeProgramName(int, const juce::String&) override {}

	bool hasEditor() const override { return false; }

	void releaseResources() override {}

	const juce::AudioProcessorValueTreeState& getParameters() const { return parameters; }

	bool isBusesLayoutSupported(const BusesLayout& layouts) const override
	{
		return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
			&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
	}

	void setName(juce::String& name) { pluginName = name; }
	const juce::String getName() const override { return pluginName; }

protected:
	double sampleRate = 44100.0;
	juce::AudioProcessorValueTreeState parameters;
	juce::String pluginName;

	// helper to generate layout from gParams
	juce::AudioProcessorValueTreeState::ParameterLayout createLayout(const ParamDesc* paramArray, int numParams)
	{
		juce::AudioProcessorValueTreeState::ParameterLayout layout;

		for (int i = 0; i < numParams; ++i)
		{
			const auto& p = paramArray[i];

			layout.add(std::make_unique<juce::AudioParameterFloat>(
				p.id,
				p.name,
				juce::NormalisableRange<float>(0.0f, 1.0f),
				p.defaultNorm,
				p.unit
			));
		}

		return layout;
	}
};
