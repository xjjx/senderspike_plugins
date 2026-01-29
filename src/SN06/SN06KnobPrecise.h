#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SN06KnobPrecise : public juce::Slider
{
public:
    explicit SN06KnobPrecise (double defaultValue);

    // legacy-compatible API
    void setRangePixels (float pixels);
    void setRangeAbsolute (float absolute);
    float getRangeAbsolute() const;

    void setLinkInversed (SN06KnobPrecise* other);

    void mouseDown (const juce::MouseEvent&) override;

private:
    void handleLinkedKnob();

    SN06KnobPrecise* linked = nullptr;

    float rangePixels = 400.0f;
    float rangeAbs    = 1.0f;

    double lastValue = 0.0;
};
