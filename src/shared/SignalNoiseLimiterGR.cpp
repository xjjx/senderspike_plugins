#include "SignalNoiseLimiterGR.h"

SignalNoiseLimiterGR::SignalNoiseLimiterGR(juce::Image meterImage)
	: ledImage(meterImage)
{}

//--------------------------------------------

void SignalNoiseLimiterGR::setValue(float val)
{
//		DBG("new val meter =" + juce::String(val, 4) + " level = " + juce::String(level, 4));

	// decay
//	float decay = juce::Decibels::decibelsToGain(-decayDbPerSecond / fps);

//	level = std::max (val, level * decay);

	// clamp 0..1
	float maxGR = 20.0f;
	value = juce::jlimit(0.0f, 1.0f, val / maxGR);
//	DBG("new max val meter =" + juce::String(val, 4));

	repaint();
}

//--------------------------------------------

void SignalNoiseLimiterGR::paint(juce::Graphics& g)
{
	if (!ledImage.isValid())
		return;

	int imgW = ledImage.getWidth();
	int imgH = ledImage.getHeight() / 2; // two rows: ON / OFF

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
	int fillWidth = static_cast<int>(imgW * value);
	if (fillWidth <= 0)
		return;

	int ds = destW - fillWidth;
	int ss = imgW - fillWidth;

	g.drawImage(ledImage,
		ds, destY, fillWidth, destH,	// destination
		ss, 0, fillWidth, imgH,		// source (top half)
		false);				// disable alpha
}
