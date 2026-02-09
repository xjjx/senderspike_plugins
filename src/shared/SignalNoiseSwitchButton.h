#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseSwitchButton : public juce::ToggleButton
{
public:
	SignalNoiseSwitchButton(const juce::String& buttonName, juce::Image spriteImage)
		: juce::ToggleButton(buttonName), image(spriteImage)
	{}

	void paintButton(juce::Graphics& g, bool, bool)
	{
		auto bounds = getLocalBounds().toFloat();

		float sx = 0.0f;
		float sy = getToggleState() ? (float)(image.getHeight()/2) : 0.0f;
		float sw = (float)image.getWidth();
		float sh = (float)(image.getHeight()/2);

		g.drawImage(image,
					0.0f, 0.0f, bounds.getWidth(), bounds.getHeight(), // destination
					sx, sy, sw, sh);									 // source
	}

	void attachToParameter(juce::AudioProcessorValueTreeState& params, const juce::String& paramId)
	{
		attachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
			params, paramId, *this
		);
	}

private:
	juce::Image image;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> attachment;
};
