#include "SNKnobPrecise.h"

SNKnobPrecise::SNKnobPrecise (const ParameterInfo& i)
	: info (i)
{
	double defaultValue = dbToNormalized(info.defaultDb);

	setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	setVelocityBasedMode (true);
	setDoubleClickReturnValue (true, defaultValue);

	setVelocityModeParameters(
		0.3,    // sensitivity, smaller = more precise
		0.3,    // threshold, allow slow drags to work
		0.05,   // offset, small base movement
		true    // skipAccelerationForSmallChanges
	);
}

//==================================================
void SNKnobPrecise::mouseDown (const juce::MouseEvent& e)
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
double SNKnobPrecise::snapDb(double db)
{
	double step = juce::ModifierKeys::getCurrentModifiers().isShiftDown() ? 0.1 : 1.0;
	return std::round(db / step) * step;
}

//==================================================
double SNKnobPrecise::snapValue(double attemptedValue, DragMode dragMode)
{
	double db = normalizedToDb(attemptedValue);
	if (dragMode != Slider::notDragging) {
		bool shift = juce::ModifierKeys::getCurrentModifiers().isShiftDown();
		double step = shift ? 0.1 : 1.0;
		db = std::round(db / step) * step;
	}

	return dbToNormalized(db);
}
