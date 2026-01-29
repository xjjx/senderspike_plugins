#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SN06KnobLookAndFeel : public juce::LookAndFeel_V4
{
public:
	// Constructor
	explicit SN06KnobLookAndFeel(const juce::Image& filmstripImage);

	// Optional: default constructor if you want to assign image later
	SN06KnobLookAndFeel() = default;

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
