#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class SN06PeakLed : public juce::Component
{
public:
	SN06PeakLed()
	{
		setSize(12, 12); // default size
	}

	void setOn(bool shouldBeOn)
	{
		on = shouldBeOn;
		repaint();
	}

	void setLevel(float newLevel)
	{
		on = newLevel > 0.01f;
		repaint();
	}

	void paint (juce::Graphics& g) override
	{
		auto b = getLocalBounds().toFloat();
		float r = b.getWidth() * 0.5f;

		if (on)
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
	bool on = false;
};
