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
