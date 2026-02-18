#include "SignalNoiseKnobLabel.h"

SignalNoiseKnobLabel::SignalNoiseKnobLabel()
{
	setJustificationType(juce::Justification::centred);
	setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	setFont(juce::Font(10.0f));
	setEditable(true, true, false);
}

void SignalNoiseKnobLabel::attachKnob(juce::Slider* k)
{
	// Set initial value
	knob = k;
	const float db = (float) knob->getValue();
	setText(juce::String(db, 2), juce::dontSendNotification);
}

void SignalNoiseKnobLabel::textWasEdited()
{
	if (!knob)
		return;

	float db = getText().getFloatValue();
	knob->setValue(db);
}
