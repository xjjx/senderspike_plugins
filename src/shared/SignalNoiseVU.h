#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseVU : public juce::Component
{
public:
	SignalNoiseVU(const juce::Image& vuImage, const juce::Image& peakMap, int numFrames);
	~SignalNoiseVU() override = default;

	void setLevel(float level);		// VU

	void paint(juce::Graphics& g) override;

private:
	juce::Image vuMap;
	juce::Image peakMap;
	int				numFrames;	// number of frames in bitmap

	float vuLevel = 0.0f;
	float peakLevel = 0.0f;

	int peakHoldFrames = 0;
	const int peakHoldTime = 15;   // ~0.5s at 30Hz
	const float peakDecay = 0.5f;  // dB per frame
};
