#include "SN06KnobPrecise.h"

SN06KnobPrecise::SN06KnobPrecise (double defaultValue)
{
    setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

    setDoubleClickReturnValue (true, defaultValue);

    // === JUCE-native behaviour (DO NOT bypass) ===
    setVelocityBasedMode (true);

    setVelocityModeParameters (
        0.9,   // sensitivity
        1,     // threshold
        0.08,  // offset
        false
    );

    setMouseDragSensitivity ((int) rangePixels);

    lastValue = defaultValue;

    // Linked inverse knob logic (JUCE-safe)
    onValueChange = [this]
    {
        handleLinkedKnob();
        lastValue = getValue();
    };
}

//==================================================
void SN06KnobPrecise::setRangePixels (float pixels)
{
    rangePixels = pixels;
    setMouseDragSensitivity ((int) rangePixels);
}

void SN06KnobPrecise::setRangeAbsolute (float absolute)
{
    rangeAbs = absolute;
}

float SN06KnobPrecise::getRangeAbsolute() const
{
    return rangeAbs;
}

void SN06KnobPrecise::setLinkInversed (SN06KnobPrecise* other)
{
    linked = other;
}

//==================================================
void SN06KnobPrecise::mouseDown (const juce::MouseEvent& e)
{
    // Ctrl / Cmd = reset (legacy behavior)
    if (e.mods.isCtrlDown() || e.mods.isCommandDown())
    {
        setValue (getDoubleClickReturnValue(),
                  juce::sendNotificationSync);
        return;
    }

    lastValue = getValue();
    juce::Slider::mouseDown (e);
}

//==================================================
void SN06KnobPrecise::handleLinkedKnob()
{
    if (linked == nullptr)
        return;

    // Alt = inverse linked knob
    if (! juce::ModifierKeys::getCurrentModifiers().isAltDown())
        return;

    const double delta = getValue() - lastValue;

    linked->setValue (
        juce::jlimit (linked->getMinimum(),
                      linked->getMaximum(),
                      linked->getValue() - delta),
        juce::sendNotificationSync);
}
