#include "ParameterLabel.h"

ParameterLabel::ParameterLabel(
   juce::AudioProcessorValueTreeState& state,
   const ParameterInfo& pInfo)
	: params(state),
	  info(pInfo)
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
//	addAndMakeVisible(label);
}

void ParameterLabel::setupCallbacks()
{
	label.onTextChange = [this]()
	{
		float db = label.getText().getFloatValue();
		float norm = info.dbToNormalized(db);

		if (auto* p = params.getParameter(info.paramID))
			p->setValueNotifyingHost(juce::jlimit(0.0f, 1.0f, norm));
	};

	// Listen for parameter changes
	params.addParameterListener(info.paramID, this);
}

void ParameterLabel::updateFromParameter()
{
	if (auto* v = params.getRawParameterValue(info.paramID))
	{
		float norm = *v;
		float db = info.normalizedToDb(norm);

		label.setText(juce::String(db, 2), juce::dontSendNotification);
	}
}

void ParameterLabel::parameterChanged(const juce::String& paramID, float newValue)
{
	if (paramID == juce::String(info.paramID))
	{
		float display = info.normalizedToDb(newValue);
		label.setText(juce::String(display, 2), juce::dontSendNotification);
	}
}
