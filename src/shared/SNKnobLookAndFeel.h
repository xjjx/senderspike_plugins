#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SNKnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
	// Constructor
	explicit SNKnobLookAndFeel(const juce::Image& filmstripImage);

	// Optional: default constructor if you want to assign image later
	SNKnobLookAndFeel() = default;

	// Override JUCE rotary slider draw
	void drawRotarySlider(juce::Graphics& g,
	int x, int y, int width, int height,
	float sliderPos,
	float rotaryStartAngle,
	float rotaryEndAngle,
	juce::Slider& slider) override;

	// Optional setter if using default constructor
	void setImage(const juce::Image& filmstripImage);

private:
	juce::Image knobStrip;
	int frameCount = 0;
};
