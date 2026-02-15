#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseGR : public juce::Component
{
public:
	SignalNoiseGR(const juce::Image& needleImage, int numFrames);
	~SignalNoiseGR() override = default;

	void setLevel(float dB);		// GR

	void paint(juce::Graphics& g) override;

private:
	juce::Image		map;		// needle bitmap (vertical stack)
	int				numFrames;	// number of frames in bitmap
	float level;
};
