#include "SignalNoiseFilmstripLNF.h"

void SignalNoiseFilmstripLNF::setImage(const juce::Image& filmstripImage, int nframes)
{
	filmstrip = filmstripImage;
	jassert(!filmstrip.isNull());

	frameHeight = filmstrip.getHeight() / nframes;
}

void SignalNoiseFilmstripLNF::drawLinearSlider(
	juce::Graphics& g,
	int x, int y, int width, int height,
	float sliderPos,
	float minSliderPos,
	float maxSliderPos,
	const juce::Slider::SliderStyle style,
	juce::Slider& slider)
{
	if (filmstrip.isNull())
		return;

	// Calculate frame
	int frameIndex = (int)slider.getValue();

	if (reverse)
	{
		int numFrames = filmstrip.getHeight() / frameHeight;
		frameIndex = (numFrames - 1) - frameIndex;
	}

	g.drawImage(
		filmstrip,
		x, y, width, height,					 // destination
		0, frameIndex * frameHeight,			 // source Y
		filmstrip.getWidth(), frameHeight		 // source size
	);
}
