#include "SignalNoiseVU.h"

SignalNoiseVU::SignalNoiseVU(const juce::Image& vuNeedleImage,
                             const juce::Image& peakNeedleImage,
                             int numFrames_)
: vuMap(vuNeedleImage),
  peakMap(peakNeedleImage),
  numFrames(numFrames_)
{
	// Optional safety
	jassert(numFrames > 0);

	// Optional: initialise levels
	vuLevel = 0.0f;
	peakLevel = 0.0f;
	peakHoldFrames = 0;
}

void SignalNoiseVU::setLevel(float level)
{
	level = std::max(level * 0.90f, level);
	level = juce::jlimit(0.0f, 1.0f, level);

    // --- VU level
	vuLevel = level;

	// --- Peak hold logic
	if (level > peakLevel)
	{
		peakLevel = level;
		peakHoldFrames = peakHoldTime;
	}
	else
	{
		if (peakHoldFrames > 0)
		{
			--peakHoldFrames;
		}
		else
		{
			peakLevel -= peakDecay;
			if (peakLevel < level)
				peakLevel = level;
		}
	}

	repaint();
}

void SignalNoiseVU::paint(juce::Graphics& g)
{
	if (!vuMap.isValid() || !peakMap.isValid())
		return;

	int frameHeight = vuMap.getHeight() / numFrames;

	// --- VU frame
	int vuFrame = juce::jlimit(0, numFrames - 1, int(vuLevel * (numFrames - 1)));

	// --- Peak frame
	int peakFrame = juce::jlimit(0, numFrames - 1, int(peakLevel * (numFrames - 1)));

	// Draw VU needle first (background)
	g.drawImage(vuMap,
		0, 0, getWidth(), getHeight(),
		0, vuFrame * frameHeight, vuMap.getWidth(), frameHeight,
		false);

	// Draw Peak needle on top
	g.drawImage(peakMap,
		0, 0, getWidth(), getHeight(),
		0, peakFrame * frameHeight, peakMap.getWidth(), frameHeight,
		false);
}
