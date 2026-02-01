#pragma once

#include "SignalNoiseKnobPrecise.h"

class SignalNoiseKnobLabel : public juce::Label
{
public:
	SignalNoiseKnobLabel();

	void attachKnob(SignalNoiseKnobPrecise& knob);
};
