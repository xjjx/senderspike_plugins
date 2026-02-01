#include "SignalNoisePeakMeter.h"
#include "BinaryData.h"

SignalNoisePeakMeter::SignalNoisePeakMeter()
{
	// Load bitmap once
	ledImage = juce::ImageCache::getFromMemory(
		SN::sn06g_vu_png,
		SN::sn06g_vu_pngSize
	);
}

void SignalNoisePeakMeter::setLevel(float val)
{
//		DBG("new val meter =" + juce::String(val, 4) + " level = " + juce::String(level, 4));

	// decay
	float decay = juce::Decibels::decibelsToGain(-decayDbPerSecond / fps);

	level = std::max (val, level * decay);

	// clamp 0..1
	level = juce::jlimit(0.0f, 1.0f, level);
//	DBG("new max val meter =" + juce::String(val, 4));
}

void SignalNoisePeakMeter::setRange(float dB)
{
	range = dB;
}

void SignalNoisePeakMeter::paint(juce::Graphics& g)
{
	if (!ledImage.isValid())
		return;

	float dB = juce::Decibels::gainToDecibels(level, -range);
	float dBlevel = juce::jlimit(0.0f, 1.0f, (dB + range) / range);

	int imgW = ledImage.getWidth();
	int imgH = ledImage.getHeight() / 2;

	auto bounds = getLocalBounds();
	int destX = bounds.getX();
	int destY = bounds.getY();
	int destW = bounds.getWidth();
	int destH = bounds.getHeight();

	// draw unlit (bottom half of image)
	g.drawImage(ledImage,
		destX, destY, destW, destH,	// destination rectangle
		0, imgH, imgW, imgH,		// source rectangle (bottom half)
		false);				// disable alpha

	// draw lit portion (top half) based on level
	int fillWidth = static_cast<int>(imgW * dBlevel);
	if (fillWidth <= 0)
		return;

	g.drawImage(ledImage,
		destX, destY, fillWidth, destH,	// destination
		0, 0, fillWidth, imgH,		// source (top half)
		false);				// disable alpha
}
