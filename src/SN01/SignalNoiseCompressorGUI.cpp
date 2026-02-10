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

	juce::Image switch1Image = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s1_png,
		BinaryData::sn01g_s1_pngSize
	);

	juce::Image switch3Image = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s3_png,
		BinaryData::sn01g_s3_pngSize
	);

	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());
	jassert(!knobScrewImage.isNull());
	jassert(!switch1Image.isNull());
	jassert(!switch3Image.isNull());

	// Knobs
	largeLNF.setImage(knobLargeImage);
	normalLNF.setImage(knobNormalImage);
	screwLNF.setImage(knobScrewImage);

	thrsKnob = setupKnobPrecise(gParams[SNE_TRSH], &largeLNF); // threshold
	funcKnob = setupKnob(gParams[SNE_FUNC], &largeLNF); // ratio
	gainKnob = setupKnobPrecise(gParams[SNE_GAIN], &largeLNF); // make-up gain

	attkKnob = setupKnob(gParams[SNE_ATTK], &normalLNF); // attack
	relsKnob = setupKnob(gParams[SNE_RELS], &normalLNF); // release
	kwdtKnob = setupKnob(gParams[SNE_KWDT], &normalLNF); // knee width
	kneeKnob = setupKnob(gParams[SNE_KNEE], &normalLNF); // knee strength
	compKnob = setupKnob(gParams[SNE_COMP], &screwLNF); // dry/wet

	auto& params = processor.getParameters();

	fbckSwitch = std::make_unique<SignalNoiseSwitchButton>("fbckSwitch", switch3Image);
	fbckSwitch->attachToParameter(params, gParams[SNE_FBCK].id);
	addAndMakeVisible(*fbckSwitch);

	// Push switch
	switchLNF.setImage(switch1Image);

	pushSlider.setSliderStyle (juce::Slider::LinearHorizontal); // or LinearHorizontal
	pushSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	pushSlider.setRange (0, 2, 1);
//	pushSlider.setSnapsToMousePosition (false);
	pushSlider.setDoubleClickReturnValue (false, 0);
	pushSlider.setVelocityBasedMode (false);
	pushSlider.setMouseDragSensitivity (1000);
	pushSlider.setLookAndFeel (&switchLNF);
	addAndMakeVisible(pushSlider);

	pushAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_PUSH].id, pushSlider
	);

	// Mode switch
	modeSlider.setSliderStyle (juce::Slider::LinearHorizontal); // or LinearHorizontal
	modeSlider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);

	modeSlider.setRange (0, 2, 1);
//	pushSlider.setSnapsToMousePosition (false);
	modeSlider.setDoubleClickReturnValue (false, 0);
	modeSlider.setVelocityBasedMode (false);
	modeSlider.setMouseDragSensitivity (1000);
	modeSlider.setLookAndFeel (&switchLNF);
	addAndMakeVisible(modeSlider);

	modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_MODE].id, pushSlider
	);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
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

/*
	// GR meter --------------------------------------------

	rc(SN01_NEEDL_X, SN01_NEEDL_Y, SN01_NEEDL_W, SN01_NEEDL_H);
	_grdb = new SignalNoiseGR(rc, needl, SN01_NEEDL_FRAMES, effect->getSampleRate());
	frm->addView(_grdb);
*/
	// switches --------------------------------------------
	modeSlider.setBounds(578, 150, 45, 45);
	pushSlider.setBounds(698, 150, 45, 45);

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
/*
void SignalNoiseCompressorGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_TRSH: if(_thrs) _thrs->setValue(effect->getParameter(at)); break;
	case SNE_FUNC: if(_func) _func->setValue(effect->getParameter(at)); break;
	case SNE_GAIN: if(_gain) _gain->setValue(effect->getParameter(at)); break;
	case SNE_ATTK: if(_attk) _attk->setValue(effect->getParameter(at)); break;
	case SNE_RELS: if(_rels) _rels->setValue(effect->getParameter(at)); break;
	case SNE_KNEE: if(_kprc) _kprc->setValue(effect->getParameter(at)); break;
	case SNE_KWDT: if(_kwdt) _kwdt->setValue(effect->getParameter(at)); break;
	case SNE_COMP: if(_comp) _comp->setValue(effect->getParameter(at)); break;
	case SNE_MODE: if(_mode) _mode->setValue(effect->getParameter(at)); break;
	case SNE_PUSH: if(_push) _push->setValue(effect->getParameter(at)); break;
	case SNE_FBCK: if(_fbck) _fbck->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
{
	if(_open == 0)
		return;

	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_TRSH:
	case SNE_FUNC:
	case SNE_GAIN:
	case SNE_ATTK:
	case SNE_RELS:
	case SNE_KNEE:
	case SNE_KWDT:
	case SNE_COMP:
	case SNE_MODE:
	case SNE_PUSH:
	case SNE_FBCK:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::trackMeter(double dB)
{
	if(_open == 0)
		return;
	_grdb->setVal(dB);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::setupMeter(double fs)
{
	if(_open == 0)
		return;
	_grdb->setFilter(fs);
}
*/

void SignalNoiseCompressorGUI::timerCallback()
{
//	inputMeter .setLevel(processor.getInputLevel());
///	outputMeter.setLevel(processor.getOutputLevel());
//	peakLed.setLevel(processor.getOutputLevel());

	repaint();
}
