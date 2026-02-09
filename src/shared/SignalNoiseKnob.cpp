#include "SignalNoiseKnob.h"

SignalNoiseKnob::SignalNoiseKnob (float defaultDb)
{
	setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	setVelocityBasedMode (false);
	setDoubleClickReturnValue (true, defaultDb);
}

//==================================================
void SignalNoiseKnob::mouseDown (const juce::MouseEvent& e)
{
	// Ctrl + Click = reset
	if (e.mods.isCtrlDown() || e.mods.isCommandDown())
	{
		setValue (getDoubleClickReturnValue(), juce::sendNotificationSync);
		return;
	}

	juce::Slider::mouseDown (e);
}
