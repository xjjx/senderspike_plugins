#include "SignalNoiseKnobPrecise.h"

SignalNoiseKnobPrecise::SignalNoiseKnobPrecise (float defaultDb)
{
	setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	setVelocityBasedMode (true);
	setDoubleClickReturnValue (true, defaultDb);

	setVelocityModeParameters(
		0.3,    // sensitivity, smaller = more precise
		0.3,    // threshold, allow slow drags to work
		0.05,   // offset, small base movement
		true    // skipAccelerationForSmallChanges
	);
}

//==================================================
void SignalNoiseKnobPrecise::mouseDown (const juce::MouseEvent& e)
{
	// Ctrl + Click = reset
	if (e.mods.isCtrlDown() || e.mods.isCommandDown())
	{
		setValue (getDoubleClickReturnValue(), juce::sendNotificationSync);
		return;
	}

	juce::Slider::mouseDown (e);
}

//==================================================
double SignalNoiseKnobPrecise::snapValue(double attemptedValue, DragMode dragMode)
{
	double db = attemptedValue;
	if (dragMode != Slider::notDragging) {
		bool shift = juce::ModifierKeys::getCurrentModifiers().isShiftDown();
		double step = shift ? 0.1 : 1.0;
		db = std::round(db / step) * step;
	}

	return db;
}

//==================================================
void SignalNoiseKnobPrecise::valueChanged()
{
	if (!label)
		return;

	const float db = (float) getValue();
	label->setText(juce::String(db, 2), juce::dontSendNotification);
};

double SignalNoiseKnobPrecise::valueToProportionOfLength(double value)
{
	auto proportion = juce::Slider::valueToProportionOfLength(value);
	return isReversed ? (1.0 - proportion) : proportion;
}

double SignalNoiseKnobPrecise::proportionOfLengthToValue(double proportion)
{
	auto p = isReversed ? (1.0 - proportion) : proportion;
	return juce::Slider::proportionOfLengthToValue(p);
}
