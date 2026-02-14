//------------------------------------------------------------------------------------
//
//	file:		sn_vsti.cpp
//
//	purpose:	Signal Noise VST 2.4 plugin interface
//
//  authors:	2019 - 2025 Oto Spál
//
//------------------------------------------------------------------------------------

#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseFX.h"

bool SignalNoiseFX::isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout& layouts) const
{
	return layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo()
		&& layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void SignalNoiseFX::getStateInformation(juce::MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void SignalNoiseFX::setStateInformation(const void* data, int sizeInBytes)
{
	std::unique_ptr<juce::XmlElement> xml(getXmlFromBinary(data, sizeInBytes));
	if (xml)
		parameters.replaceState(juce::ValueTree::fromXml(*xml));
}

// helper to generate layout from gParams
juce::AudioProcessorValueTreeState::ParameterLayout
SignalNoiseFX::createLayout(const ParamDesc* paramArray, int numParams)
{
	juce::AudioProcessorValueTreeState::ParameterLayout layout;

	for (int i = 0; i < numParams; ++i)
	{
		const ParamDesc& p = paramArray[i];
		float step;
		juce::NormalisableRange<float> range;
		switch (p.type)
		{
		case ParamType::Decibel:
		case ParamType::Hertz:
			step = 0.01f;
			range = juce::NormalisableRange<float>(p.minValue, p.maxValue, step);
			break;
		case ParamType::Percent:
			step = 0.1f;
			range = juce::NormalisableRange<float>(p.minValue, p.maxValue, step);
			break;
		case ParamType::Normalized:
			range = juce::NormalisableRange<float>(p.minValue, p.maxValue);
			break;
		case ParamType::Cubic:
			// we cheat a little here, parameter stays normalized, but we treat it differently
			step = 0.001f;
			range = juce::NormalisableRange<float>(0.0f, 1.0f, step);
			break;
		case ParamType::Choice: // just to please compilier
			break;
		}

		if (p.type == ParamType::Choice)
		{
			juce::StringArray labels;
			for (auto s : p.choiceLabels)
				labels.add(s);

			int defaultIndex = static_cast<int>(p.defaultValue);
			layout.add(std::make_unique<juce::AudioParameterChoice>(
				p.id,
				p.name,
				labels,
				defaultIndex,
				p.unit
			));
		}
		else if (p.type == ParamType::Cubic)
		{
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				p.id,
				p.name,
				range,
				p.cubicToNorm(p.defaultValue),
				p.unit,
				juce::AudioProcessorParameter::genericParameter,
				// stringFromValue
				[p](float value, int) {
					float ms = p.normToCubic(value);
					return juce::String(ms, 3) + " ms";
				},
				// valueFromString
				[p](const juce::String& text) {
					float ms = text.getFloatValue();
					return p.cubicToNorm(ms);
				}
			));
		}
		else
		{
			layout.add(std::make_unique<juce::AudioParameterFloat>(
				p.id,
				p.name,
				range,
				p.defaultValue,
				p.unit
			));
		}
	}

	return layout;
}
