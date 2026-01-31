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

	gainLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_GAIN]);
	trimLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_TRIM]);
	volumeLabel = std::make_unique<ParameterLabel>(params, parameterInfos[SNE_VOLU]);

	gainKnob   = setupKnobAndLabel(parameterInfos[SNE_GAIN], &largeLNF, *gainLabel);
	trimKnob   = setupKnobAndLabel(parameterInfos[SNE_TRIM], &screwLNF, *trimLabel);
	volumeKnob = setupKnobAndLabel(parameterInfos[SNE_VOLU], &largeLNF, *volumeLabel);

	addAndMakeVisible(inputMeter);
	addAndMakeVisible(outputMeter);
	addAndMakeVisible(peakLed);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

SN06Editor::~SN06Editor()
{
	trimKnob->setLookAndFeel(nullptr);
	gainKnob->setLookAndFeel(nullptr);
	volumeKnob->setLookAndFeel(nullptr);
}

//---------------------------------------------------------
std::unique_ptr<SN06KnobPrecise> SN06Editor::setupKnobAndLabel(
	const ParameterInfo& info,
	juce::LookAndFeel* lnF,
	ParameterLabel& label)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SN06KnobPrecise>(info);
	knob->setLookAndFeel(lnF);

	addAndMakeVisible(*knob);
	addAndMakeVisible(label.getLabel());

	knob->attachToParameter(params);

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
	// Scale knobs relative to editor size
//	  float scaleX = (float)getWidth() / background.getWidth();
//	  float scaleY = (float)getHeight() / background.getHeight();

	// Temporary positions (we’ll match original layout later)
	trimKnob->setBounds(43, 35, 34, 34);
	gainKnob->setBounds(80, 100, 80, 80);
	volumeKnob->setBounds(80, 230, 80, 80);

	volumeLabel->getLabel().setBounds(30, 314, 30, 14);
	gainLabel->getLabel().setBounds(30, 184, 30, 14);
	trimLabel->getLabel().setBounds(44, 74, 30, 12);

	inputMeter.setBounds (112, 42, 100, 5);
	outputMeter.setBounds (112, 52, 100, 5);
	peakLed.setBounds (180, 307, 10, 10);
}

void SN06Editor::timerCallback()
{
	inputMeter .setLevel(processor.getInputLevel());
	outputMeter.setLevel(processor.getOutputLevel());
	peakLed.setLevel(processor.getOutputLevel());

	repaint();
}
