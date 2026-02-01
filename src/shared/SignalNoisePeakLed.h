#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoisePeakLed : public juce::Component
{
public:
	SignalNoisePeakLed()
	{
		setSize(12, 12); // default size
	}

	void setLevel(float newLevel)
	{
		level = std::max(level * 0.90f, newLevel);
	}

	void paint (juce::Graphics& g) override
	{
		auto b = getLocalBounds().toFloat();
		float r = b.getWidth() * 0.5f;

		if (level > 1.0f)
		{
			juce::ColourGradient glow(
				juce::Colours::red.withAlpha(0.9f),
				b.getCentreX(), b.getCentreY(),
				juce::Colours::transparentBlack,
				b.getCentreX() + r, b.getCentreY(),
				true
			);
			g.setGradientFill(glow);
		}
		else
		{
			juce::ColourGradient glow(
				juce::Colours::black.withBrightness(0.3f),
				b.getCentreX(), b.getCentreY(),
				juce::Colours::black,
				b.getCentreX() + r, b.getCentreY(),
				true
			);
			g.setGradientFill(glow);
		}

		g.fillEllipse(b);
	}

private:
	float level = 0.0f;
};
