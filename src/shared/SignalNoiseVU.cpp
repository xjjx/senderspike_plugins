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

	setCalibration(18);
}

void SignalNoiseVU::setCalibration(int calibration)
{
	static constexpr float gTicks[VU_MAX_TICKS] =
	{
		-20, -10, -7, -5, -3, -2, -1, 0, 1, 2, 3, 3.5f
	};

	static constexpr uint32_t vut[VU_MAX_TICKS] = {
		4, 21, 32, 42, 56, 65, 74, 84, 97, 109, 123, 129
	};

	// 0 VU = -18 dBFS
	static constexpr float kNominalOffset = 2.92f;
	float kNominalLevel = calibration + kNominalOffset;

	for (int i = 0; i < VU_MAX_TICKS; ++i)
	{
		ticks[i].amp = std::pow(10.0f, (gTicks[i] - kNominalLevel) / 20.0f);
		ticks[i].frame = vut[i];
	}
}

void SignalNoiseVU::setLevel(float filteredLinear)
{
	vuLevel = linearToNormalized(filteredLinear);

	int currentFrame = int(vuLevel * (numFrames - 1));

	if (currentFrame > holdFrame)
	{
		holdFrame	= currentFrame;
		holdCounter = holdFrames; // reset countdown
	}
	else if (holdCounter > 0)
	{
		--holdCounter;
	}
	else
	{
		holdFrame = currentFrame; // timer expired, follow VU
	}

	peakLevel = float(holdFrame) / float(numFrames - 1);
	repaint();
}

float SignalNoiseVU::linearToNormalized(float v)
{
	v = juce::jmax(0.0f, v);

	int frame = 0;
	if (v < ticks[0].amp)
	{
		frame = int((v / ticks[0].amp) * ticks[0].frame);
	}
	else
	{
		bool found = false;
		for (int i = 1; i < VU_MAX_TICKS; ++i)
		{
			if (v < ticks[i].amp)
			{
				float frac = (v - ticks[i-1].amp) / (ticks[i].amp - ticks[i-1].amp);
				frame = int(ticks[i-1].frame + frac * (ticks[i].frame - ticks[i-1].frame));
				found = true;
				break;
			}
		}
		if (!found)
			frame = ticks[VU_MAX_TICKS - 1].frame;
	}

	return float(frame) / float(ticks[VU_MAX_TICKS - 1].frame);
}

void SignalNoiseVU::paint(juce::Graphics& g)
{
	if (!vuMap.isValid() || !peakMap.isValid())
		return;

	int frameHeight = vuMap.getHeight() / numFrames;

	// --- VU frame
	int vuFrame = juce::jlimit(0, numFrames - 1, int(vuLevel * (numFrames - 1)));

	g.drawImage(vuMap,
		0, 0, getWidth(), getHeight(),
		0, vuFrame * frameHeight, vuMap.getWidth(), frameHeight,
		false);

	// Draw VU needle first (background)
	g.drawImage(vuMap,
		0, 0, getWidth(), getHeight(),
		0, vuFrame * frameHeight, vuMap.getWidth(), frameHeight,
		false
	);

	// --- Peak frame
	int peakFrame = juce::jlimit(0, numFrames - 1, int(peakLevel * (numFrames - 1)));

	// Draw Peak needle on top
	g.drawImage(peakMap,
		0, 0, getWidth(), getHeight(),
		0, peakFrame * frameHeight, peakMap.getWidth(), frameHeight,
		false
	);
}
