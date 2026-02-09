#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SwitchLookAndFeel : public juce::LookAndFeel_V4
{
public:
	// Constructor
	SwitchLookAndFeel() {};
	~SwitchLookAndFeel() = default;

	void setImage(const juce::Image& filmstripImage)
	{
		switchImage = filmstripImage;
		jassert(!switchImage.isNull());
	}

	void drawLinearSlider (juce::Graphics& g,
						   int x, int y, int width, int height,
						   float /* sliderPos */,
						   float /* minSliderPos */,
						   float /* maxSliderPos */,
						   const juce::Slider::SliderStyle,
						   juce::Slider& slider) override
	{
		if (switchImage.isNull())
			return;

		const int frameSize = switchImage.getWidth();
		int numFrames = switchImage.getHeight() / frameSize;

		int frameIndex = (int) slider.getValue();
		frameIndex = juce::jlimit (0, numFrames - 1, frameIndex);

		g.drawImage (
			switchImage,
			x, y, width, height,		// destination
			0, frameIndex * frameSize,	// source
			frameSize, frameSize);
	}

private:
	juce::Image switchImage;
};
