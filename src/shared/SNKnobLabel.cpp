#include "SNKnobLabel.h"

SNKnobLabel::SNKnobLabel()
{
	setJustificationType(juce::Justification::centred);
	setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	setFont(juce::Font(10.0f));
	setEditable(true, true, false);
}

void SNKnobLabel::attachKnob(SNKnobPrecise& knob)
{
	// Set initial value
	const float db = (float) knob.getValue();
	setText(juce::String(db, 1), juce::dontSendNotification);

	this->onTextChange = [this, &knob]
	{
		float db = getText().getFloatValue();
		knob.setValue(db);
	};
}
