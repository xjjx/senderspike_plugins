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

enum class ParamType
{
	Normalized,	// legacy 0..1
	Decibel,	// linear gain in dB
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
};

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
	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	bool isBusesLayoutSupported(const BusesLayout& layouts) const override
	{
		return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
			&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
	}

	void setName(juce::String& name) { pluginName = name; }
	const juce::String getName() const override { return pluginName; }

	void getStateInformation(juce::MemoryBlock& destData) override
	{
		auto state = parameters.copyState();
		std::unique_ptr<juce::XmlElement> xml(state.createXml());
		copyXmlToBinary(*xml, destData);
	}

	void setStateInformation(const void* data, int sizeInBytes) override
	{
		std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
		if (xml)
			parameters.replaceState(juce::ValueTree::fromXml(*xml));
	}

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

			juce::NormalisableRange<float> range;
			if (p.type == ParamType::Decibel)
			{
				float step = 0.01f;
				range = juce::NormalisableRange<float>(p.minValue, p.maxValue, step);
			}
			else
			{
				range = juce::NormalisableRange<float>(p.minValue, p.maxValue);
			}

			layout.add(std::make_unique<juce::AudioParameterFloat>(
				p.id,
				p.name,
				range,
				p.defaultValue,
				p.unit
			));
		}

		return layout;
	}
};
