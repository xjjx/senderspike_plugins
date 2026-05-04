//------------------------------------------------------------------------------------
//
//	file:		sn_01g.cpp
//
//	purpose:	SN01 compressor GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <stdio.h>
#include <math.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseCompressorGUI.h"
#include "BinaryData.h"

//------------------------------------------------------------------------------------

SignalNoiseCompressorGUI::SignalNoiseCompressorGUI(SignalNoiseCompressor& p)
	: AudioProcessorEditor(&p), processor(p)
{
	// Load images
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_bk_png,
		BinaryData::sn01g_bk_pngSize
	);

	juce::Image knobLargeImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b1_png,
		BinaryData::sn01g_b1_pngSize
	);

	juce::Image knobNormalImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b2_png,
		BinaryData::sn01g_b2_pngSize
	);

	juce::Image knobScrewImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b3_png,
		BinaryData::sn01g_b3_pngSize
	);

	juce::Image smallKnobScrewImage = juce::ImageCache::getFromMemory(
		BinaryData::sn06g_b1_png,
		BinaryData::sn06g_b1_pngSize
	);

	juce::Image switch1Image = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s1_png,
		BinaryData::sn01g_s1_pngSize
	);

	juce::Image switch3Image = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s3_png,
		BinaryData::sn01g_s3_pngSize
	);

	juce::Image needleImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_gr_png,
		BinaryData::sn01g_gr_pngSize
	);

	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());
	jassert(!knobScrewImage.isNull());
	jassert(!smallKnobScrewImage.isNull());
	jassert(!switch1Image.isNull());
	jassert(!switch3Image.isNull());
	jassert(!needleImage.isNull());

	// Knobs
	largeLNF.setImage(knobLargeImage);
	normalLNF.setImage(knobNormalImage);
	screwLNF.setImage(knobScrewImage);
	smallScrewLNF.setImage(smallKnobScrewImage);

	thrsKnob = setupKnobPrecise(gParams[SNE_TRSH], &largeLNF); // threshold
	thrsKnob->setReversed(true);

	funcKnob = setupKnob(gParams[SNE_FUNC], &largeLNF); // ratio
	gainKnob = setupKnobPrecise(gParams[SNE_GAIN], &largeLNF); // make-up gain

	attkKnob = setupKnob(gParams[SNE_ATTK], &normalLNF); // attack
	relsKnob = setupKnob(gParams[SNE_RELS], &normalLNF); // release
	kwdtKnob = setupKnob(gParams[SNE_KWDT], &normalLNF); // knee width
	kneeKnob = setupKnob(gParams[SNE_KNEE], &normalLNF); // knee strength
	compKnob = setupKnob(gParams[SNE_COMP], &screwLNF); // dry/wet
	linkKnob = setupKnob(gParams[SNE_LINK], &smallScrewLNF); // link

	auto& params = processor.getParameters();

	fbckSwitch = std::make_unique<SignalNoiseSwitchButton>("fbckSwitch", switch3Image);
	fbckSwitch->attachToParameter(params, gParams[SNE_FBCK].id);
	addAndMakeVisible(*fbckSwitch);

	// Push switch
	switchLNF.setImage(switch1Image, 3);
	setup3waySwitch(&pushSlider, gParams[SNE_PUSH], &switchLNF, pushAttachment);

	// Mode switch
	setup3waySwitch(&modeSlider, gParams[SNE_MODE], &switchLNF, modeAttachment);

	// Stereo Link switch
	setup3waySwitch(&linkSlider, gParams[SNE_DMODE], &switchLNF, linkAttachment);

	// GR Meter
	grMeter = std::make_unique<SignalNoiseGR>(needleImage, 100);
	addAndMakeVisible(*grMeter);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

void SignalNoiseCompressorGUI::setup3waySwitch(
	juce::Slider* slider,
	const ParamDesc& p,
	juce::LookAndFeel* lnf,
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>& attachment)
{
	auto& params = processor.getParameters();

	slider->setSliderStyle (juce::Slider::LinearBar);
	slider->setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	slider->setRange (0, 2, 1);
//	slider->setSnapsToMousePosition (false);
	slider->setDoubleClickReturnValue (false, 0);
	slider->setVelocityBasedMode (false);
	slider->setMouseDragSensitivity (1000);
	slider->setLookAndFeel (lnf);
	addAndMakeVisible(slider);

	attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, p.id, *slider
	);
}

std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseCompressorGUI::setupKnobPrecise(
	const ParamDesc& p,
	juce::LookAndFeel* lnF)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SignalNoiseKnobPrecise>(p.defaultValue);

	knob->setLookAndFeel(lnF);
	knob->attachToParameter(params, p.id);
	addAndMakeVisible (*knob);

	return knob;
}

std::unique_ptr<SignalNoiseKnob> SignalNoiseCompressorGUI::setupKnob(
	const ParamDesc& p,
	juce::LookAndFeel* lnF)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SignalNoiseKnob>(p.defaultValue);

	knob->setLookAndFeel(lnF);
	knob->attachToParameter(params, p.id);
	addAndMakeVisible (*knob);

	return knob;
}

//------------------------------------------------------------------------------------

SignalNoiseCompressorGUI::~SignalNoiseCompressorGUI()
{
	pushSlider.setLookAndFeel (nullptr);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::resized()
{
	// big knobs -------------------------------------------
	thrsKnob->setBounds(135, 20, 80, 80);
	funcKnob->setBounds(265, 20, 80, 80);
	gainKnob->setBounds(395, 20, 80, 80);

	// small knobs -----------------------------------------
	attkKnob->setBounds(80, 130, 60, 60);
	relsKnob->setBounds(210, 130, 60, 60);
	kwdtKnob->setBounds(340, 130, 60, 60);
	kneeKnob->setBounds(470, 130, 60, 60);

	// screw knob ------------------------------------------
	compKnob->setBounds(640, 140, 40, 40);
	linkKnob->setBounds(510, 70, 30, 30);

	// GR meter --------------------------------------------
	grMeter->setBounds(570, 20, 180, 80);

	// switches --------------------------------------------
	modeSlider.setBounds(578, 150, 45, 45);
	pushSlider.setBounds(698, 150, 45, 45);
	linkSlider.setBounds(500, 20, 45, 45);

	fbckSwitch->setBounds(25, 90, 40, 40);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::timerCallback()
{
	grMeter->setLevel(processor.getGainReduction());
}
