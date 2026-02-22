#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SignalNoiseVU : public juce::Component
{
public:
	SignalNoiseVU(const juce::Image& vuImage, const juce::Image& peakMap, int numFrames);
	~SignalNoiseVU() override = default;

	void setLevel(float level);
	void setCalibration(int c);

	void paint(juce::Graphics& g) override;

private:
	juce::Image vuMap;
	juce::Image peakMap;
	int numFrames;	// number of frames in bitmap

	float vuLevel = 0.0f;
	float peakLevel = 0.0f;
	int holdFrame   = 0;
	int holdCounter = 0;
	int holdFrames  = 30; // ~1 second at 30Hz

	static constexpr int VU_MAX_TICKS = 12;

	struct VUTick
	{
		float amp;   // amplitude threshold
		int frame;   // corresponding frame index
	};

	std::array<VUTick, VU_MAX_TICKS> ticks;

	// helper function
	float linearToNormalized(float v);
};
