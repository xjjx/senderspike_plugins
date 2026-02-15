#include "SignalNoiseGR.h"
#include <cmath>

SignalNoiseGR::SignalNoiseGR(const juce::Image& needleImage, int numFrames_)
	: map(needleImage),
	  numFrames(numFrames_)
{}

void SignalNoiseGR::setLevel(float dB)
{
	level = juce::jlimit(0.0f, 20.0f, dB);
//	repaint();
}

void SignalNoiseGR::paint(juce::Graphics& g)
{
	if (!map.isValid())
		return;

	int frameHeight = map.getHeight() / numFrames;
	int frame = int((level / 20.0f) * (numFrames - 1));

	// Draw the correct frame from the filmstrip
	g.drawImage(map,
		0, 0, getWidth(), getHeight(),// destination
		0, frame * frameHeight, map.getWidth(), frameHeight, // source
		true // optional: fillAlphaChannelWithCurrentBrush
	);
}
