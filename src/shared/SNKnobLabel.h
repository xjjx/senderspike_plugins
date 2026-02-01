#pragma once

#include "SNKnobPrecise.h"

class SNKnobLabel : public juce::Label
{
public:
	SNKnobLabel();

	void attachKnob(SNKnobPrecise& knob);
};
