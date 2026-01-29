#include <juce_audio_processors/juce_audio_processors.h>
#include "SN06Editor.h"
#include "SN06Processor.h"
#include "BinaryData.h"

SN06Editor::SN06Editor(SN06Processor& p)
	: AudioProcessorEditor(&p),
	  processor(p),
	  trimKnob(0.5),
	  gainKnob(0.5),
	  volumeKnob(0.5)
{
	// Load background image
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_bk_png,
		BinaryData::sn06g_bk_pngSize
	);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	// Knobs
	juce::Image knobLargeImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b1_png,
		BinaryData::sn01g_b1_pngSize
	);

	juce::Image knobScrewImage = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_b1_png,
		BinaryData::sn06g_b1_pngSize
	);

	jassert(!gainImage.isNull());
	jassert(!trimImage.isNull());

	largeLNF.setImage(knobLargeImage);
	screwLNF.setImage(knobScrewImage);

	trimKnob.setLookAndFeel(&screwLNF);
	gainKnob.setLookAndFeel(&largeLNF);
	volumeKnob.setLookAndFeel(&largeLNF);

	addAndMakeVisible(trimKnob);
	addAndMakeVisible(gainKnob);
	addAndMakeVisible(volumeKnob);

	// Knobs labels
	// Volume text label setup
	volumeLabel.setText("0.00", juce::dontSendNotification);
	volumeLabel.setJustificationType(juce::Justification::centred);
	volumeLabel.setColour(juce::Label::textColourId, juce::Colours::lightgrey);
	volumeLabel.setFont(juce::Font(10.0f));
	volumeLabel.setEditable(true, true, false);

	addAndMakeVisible(volumeLabel);

	volumeKnob.onValueChange = [this] {
		float val = volumeKnob.getValue();
		float displayVal = val * 64 - 48;
		volumeLabel.setText(
			juce::String(displayVal, 2),
			juce::dontSendNotification
		);
	};

	volumeLabel.onTextChange = [this] {
		float val = volumeLabel.getText().getFloatValue();
		float normalized = (val + 48.0f) / 64.0f;
		normalized = juce::jlimit(0.0f, 1.0f, normalized);

		// Update parameter directly
		processor.getParameters().getParameter("volume")
			->setValueNotifyingHost(normalized);

		// update knob to match
		volumeKnob.setValue(normalized);
	};

	// Attachments (THIS replaces all manual setValue calls)
	auto& params = processor.getParameters();

	gainAttachment	 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						params, "gain", gainKnob);
	trimAttachment	 = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						params, "trim", trimKnob);
	volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
						params, "volume", volumeKnob);

	addAndMakeVisible(inputMeter);
	addAndMakeVisible(outputMeter);
	addAndMakeVisible(peakLed);

	startTimerHz(30); // GUI refresh rate
}

SN06Editor::~SN06Editor()
{
	trimKnob.setLookAndFeel(nullptr);
	gainKnob.setLookAndFeel(nullptr);
	volumeKnob.setLookAndFeel(nullptr);
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
	trimKnob.setBounds(43, 35, 34, 34);
	gainKnob.setBounds(80, 100, 80, 80);
	volumeKnob.setBounds(80, 230, 80, 80);

	volumeLabel.setBounds(30, 314, 30, 14);

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
