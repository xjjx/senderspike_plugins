#include "SignalNoiseKnobLookAndFeel.h"

// Constructor
SignalNoiseKnobLookAndFeel::SignalNoiseKnobLookAndFeel(const juce::Image& filmstripImage)
	: knobStrip(filmstripImage)
{
	jassert(!knobStrip.isNull());
	frameCount = knobStrip.getHeight() / knobStrip.getWidth();
}

// Optional setter
void SignalNoiseKnobLookAndFeel::setImage(const juce::Image& filmstripImage)
{
	knobStrip = filmstripImage;
	jassert(!knobStrip.isNull());
	frameCount = knobStrip.getHeight() / knobStrip.getWidth();
}

// Override JUCE rotary slider draw
void SignalNoiseKnobLookAndFeel::drawRotarySlider(juce::Graphics& g,
	int x, int y, int width, int height,
	float sliderPos,
	float /* rotaryStartAngle */,
	float /* rotaryEndAngle */,
	juce::Slider& /* slider */)
{
	if (knobStrip.isNull() || frameCount <= 0)
		return;

	const int frameIndex = juce::jlimit(
		0, frameCount - 1,
		static_cast<int>(std::round(sliderPos * (frameCount - 1)))
	);

	const int frameSize = knobStrip.getWidth();

	g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);

	g.drawImage(knobStrip,
		x, y, width, height,
		0, frameIndex * frameSize,
		frameSize, frameSize);
}
