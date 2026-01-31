#include <juce_audio_processors/juce_audio_processors.h>
#include "SN06Editor.h"
#include "SN06Processor.h"
#include "BinaryData.h"

SN06Editor::SN06Editor(SN06Processor& p)
	: AudioProcessorEditor(&p),
	  processor(p)
{
	// Load background image
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_bk_png,
		BinaryData::sn06g_bk_pngSize
	);

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

	auto& params = processor.getParameters();

	gainLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_GAIN].paramID);
	trimLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_TRIM].paramID);
	volumeLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_VOLU].paramID);

	gainKnob   = setupKnobAndLabel(parameterInfos[SNE_GAIN], &largeLNF, *gainLabel);
	trimKnob   = setupKnobAndLabel(parameterInfos[SNE_TRIM], &screwLNF, *trimLabel);
	volumeKnob = setupKnobAndLabel(parameterInfos[SNE_VOLU], &largeLNF, *volumeLabel);

	addAndMakeVisible(inputMeter);
	addAndMakeVisible(outputMeter);
	addAndMakeVisible(peakLed);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());
	setResizable(true, true);
	getConstrainer()->setFixedAspectRatio(
		(float)background.getWidth() / (float)background.getHeight()
	);

	startTimerHz(30); // GUI refresh rate
}

SN06Editor::~SN06Editor()
{
	trimKnob->setLookAndFeel(nullptr);
	gainKnob->setLookAndFeel(nullptr);
	volumeKnob->setLookAndFeel(nullptr);
}

//---------------------------------------------------------
std::unique_ptr<SNKnobPrecise> SN06Editor::setupKnobAndLabel(
	const ParameterInfo& info,
	juce::LookAndFeel* lnF,
	ParameterLabel& label)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SNKnobPrecise>(info.defaultDb);
	knob->setLookAndFeel(lnF);

	addAndMakeVisible(*knob);
	addAndMakeVisible(label.getLabel());

	knob->attachToParameter(params, info.paramID);

    return knob;
}

//---------------------------------------------------------
void SN06Editor::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//---------------------------------------------------------
void SN06Editor::resized()
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
	trimLabel->getLabel().setBounds(scaledRect(44, 74, 30, 12));
	gainLabel->getLabel().setBounds(scaledRect(30, 184, 30, 14));
	volumeLabel->getLabel().setBounds(scaledRect(30, 314, 30, 14));

	// Meters
	inputMeter.setBounds(scaledRect(112, 42, 100, 5));
	outputMeter.setBounds(scaledRect(112, 52, 100, 5));

	// Peak LED
	peakLed.setBounds(scaledRect(180, 307, 10, 10));
}

void SN06Editor::timerCallback()
{
	inputMeter .setLevel(processor.getInputLevel());
	outputMeter.setLevel(processor.getOutputLevel());
	peakLed.setLevel(processor.getOutputLevel());

	repaint();
}
