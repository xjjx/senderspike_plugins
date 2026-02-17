#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseLimiterGR : public juce::Component
{
public:
	SignalNoiseLimiterGR(juce::Image meterImage);

	void setValue(float val); // incoming GR value

	void paint(juce::Graphics& g) override;

private:
	juce::Image ledImage;
	float value = {0.0f};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SignalNoiseLimiterGR)
};
