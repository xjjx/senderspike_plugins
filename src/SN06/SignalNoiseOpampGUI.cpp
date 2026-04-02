#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseOpampGUI.h"
#include "BinaryData.h"

SignalNoiseOpampGUI::SignalNoiseOpampGUI(SignalNoiseOpamp& p)
	: AudioProcessorEditor(&p),
	  processor(p)
{
	// Load background image
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_bk_png,
		BinaryData::sn06g_bk_pngSize
	);

	// Peak Meter
	juce::Image ledImage = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_vu_png,
		BinaryData::sn06g_vu_pngSize
	);

	inputMeter = std::make_unique<SignalNoisePeakMeter>(ledImage);
	outputMeter = std::make_unique<SignalNoisePeakMeter>(ledImage);

	addAndMakeVisible(*inputMeter);
	addAndMakeVisible(*outputMeter);
	addAndMakeVisible(peakLed);

	// Knobs
	juce::Image knobLargeImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b1_png,
		BinaryData::sn01g_b1_pngSize
	);

	juce::Image knobScrewImage = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_b1_png,
		BinaryData::sn06g_b1_pngSize
	);

	jassert(!knobLargeImage.isNull());
	jassert(!knobScrewImage.isNull());

	largeLNF.setImage(knobLargeImage);
	screwLNF.setImage(knobScrewImage);

	gainKnob   = setupKnobAndLabel(gParams[SNE_GAIN], &largeLNF, gainLabel);
	trimKnob   = setupKnobAndLabel(gParams[SNE_TRIM], &screwLNF, trimLabel);
	volumeKnob = setupKnobAndLabel(gParams[SNE_VOLU], &largeLNF, volumeLabel);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());
/*
	setResizable(true, true);
	getConstrainer()->setFixedAspectRatio(
		(float)background.getWidth() / (float)background.getHeight()
	);
*/

	startTimerHz(30); // GUI refresh rate
}

SignalNoiseOpampGUI::~SignalNoiseOpampGUI()
{
	trimKnob->setLookAndFeel(nullptr);
	gainKnob->setLookAndFeel(nullptr);
	volumeKnob->setLookAndFeel(nullptr);
}

//---------------------------------------------------------
std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseOpampGUI::setupKnobAndLabel(
	const ParamDesc& p,
	juce::LookAndFeel* lnF,
	SignalNoiseKnobLabel& label)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SignalNoiseKnobPrecise>(p.defaultValue);
	knob->setLookAndFeel(lnF);

	addAndMakeVisible(*knob);
	addAndMakeVisible(label);

	knob->attachToParameter(params, p.id);
	knob->attachLabel(&label);
	label.attachKnob(knob.get());

    return knob;
}

//---------------------------------------------------------
void SignalNoiseOpampGUI::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//---------------------------------------------------------
void SignalNoiseOpampGUI::resized()
{
	// Reference size (original editor background)
	const float refWidth  = (float)background.getWidth();
	const float refHeight = (float)background.getHeight();

	// Current editor size
	const float w = (float)getWidth();
	const float h = (float)getHeight();

	// Single scale factor to preserve aspect ratio
	const float scale = std::min(w / refWidth, h / refHeight);

	// Compute offsets to center the GUI if window is larger than original ratio
	const float offsetX = (w - refWidth * scale) / 2.0f;
	const float offsetY = (h - refHeight * scale) / 2.0f;

	// Helper lambda to scale and offset rectangles
	auto scaledRect = [&](int x, int y, int width, int height)
	{
		return juce::Rectangle<int>(
			static_cast<int>(offsetX + x * scale),
			static_cast<int>(offsetY + y * scale),
			static_cast<int>(width * scale),
			static_cast<int>(height * scale)
		);
	};

	// Knobs
	trimKnob->setBounds(scaledRect(45, 37, 30, 30));
	gainKnob->setBounds(scaledRect(80, 100, 80, 80));
	volumeKnob->setBounds(scaledRect(80, 230, 80, 80));

	// Labels
	trimLabel.setBounds(scaledRect(44, 74, 30, 12));
	gainLabel.setBounds(scaledRect(30, 184, 30, 14));
	volumeLabel.setBounds(scaledRect(30, 314, 30, 14));

	// Meters
	inputMeter->setBounds(scaledRect(112, 42, 100, 5));
	outputMeter->setBounds(scaledRect(112, 52, 100, 5));

	// Peak LED
	peakLed.setBounds(scaledRect(190, 316, 10, 10));
}

void SignalNoiseOpampGUI::timerCallback()
{
	inputMeter->setLevel(processor.getInputLevel());
	outputMeter->setLevel(processor.getOutputLevel());
	peakLed.setLevel(processor.getOutputLevel());
}
