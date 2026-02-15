#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseFilmstripLNF : public juce::LookAndFeel_V4
{
public:
	// Constructor
	SignalNoiseFilmstripLNF() {};
	~SignalNoiseFilmstripLNF() = default;

	void drawLinearSlider(
		juce::Graphics& g,
		int x, int y, int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const juce::Slider::SliderStyle style,
		juce::Slider& slider) override;

	virtual void setImage(const juce::Image& filmstripImage, int frames);

private:
	juce::Image filmstrip;
	int frameHeight = 0;
};
