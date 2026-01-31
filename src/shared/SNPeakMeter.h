#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SNPeakMeter : public juce::Component
{
public:
	SNPeakMeter();

	void setLevel(float val);
	void setRange(float dB);

	void paint(juce::Graphics& g);

private:
	juce::Image ledImage;
	float level = 0.0f; // linear 0..1
	float range = 72.0f;
	float fps = 30.0f; // must be in sync with timer
	float decayDbPerSecond = 24.0f;
};
