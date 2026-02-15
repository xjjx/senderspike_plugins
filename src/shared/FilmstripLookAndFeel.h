class FilmstripLookAndFeel : public juce::LookAndFeel_V4
{
public:
	FilmstripLookAndFeel(const juce::Image& img, int frames)
		: filmstrip(img), numFrames(frames)
	{}

	void drawLinearSlider(
		juce::Graphics& g,
		int x, int y, int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const juce::Slider::SliderStyle style,
		juce::Slider& slider) override
	{
		if (filmstrip.isNull())
			return;

		// Calculate frame
		int frameIndex = (int)slider.getValue();

		frameIndex = juce::jlimit(0, numFrames - 1, frameIndex);

		int frameHeight = filmstrip.getHeight() / numFrames;

		g.drawImage(
			filmstrip,
			x, y, width, height,					 // destination
			0, frameIndex * frameHeight,			 // source Y
			filmstrip.getWidth(), frameHeight		 // source size
		);
	}

private:
	juce::Image filmstrip;
	int numFrames = 1;
};
