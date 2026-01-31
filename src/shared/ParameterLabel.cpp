#include "ParameterLabel.h"

ParameterLabel::ParameterLabel(
   juce::AudioProcessorValueTreeState& state,
   const juce::String& pid)
	: params(state), paramId(pid)
{
	configureLabel();
	updateFromParameter();
	setupCallbacks();
}

void ParameterLabel::configureLabel()
{
	label.setJustificationType(juce::Justification::centred);
	label.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	label.setFont(juce::Font(10.0f));
	label.setEditable(true, true, false);
}

void ParameterLabel::setupCallbacks()
{
	label.onTextChange = [this]()
	{
		auto* p = params.getParameter(paramId);
		if (!p)
			return;

		float db = label.getText().getFloatValue();
		float norm = p->getNormalisableRange().convertTo0to1(db);
		p->setValueNotifyingHost(norm);
	};

	// Listen for parameter changes
	params.addParameterListener(paramId, this);
}

void ParameterLabel::updateFromParameter()
{
	if (auto* v = params.getRawParameterValue(paramId))
	{
		float db = *v;
		label.setText(juce::String(db, 2), juce::dontSendNotification);
	}
}

void ParameterLabel::parameterChanged(const juce::String& paramID, float newValue)
{
	if (paramID == paramId)
		label.setText(juce::String(newValue, 2), juce::dontSendNotification);
}
