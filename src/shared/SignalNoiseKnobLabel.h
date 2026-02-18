#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseKnobLabel : public juce::Label
{
private:
	juce::Slider* knob = nullptr;

public:
	SignalNoiseKnobLabel();
	void textWasEdited() override;

	void attachKnob(juce::Slider* k);
};
