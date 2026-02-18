//------------------------------------------------------------------------------------
//
//	file:		sn_05e.cpp
//
//	purpose:	SN05 limiter GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <stdio.h>
#include <math.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseLimiterGUI.h"
#include "BinaryData.h"

//------------------------------------------------------------------------------------
/*
static void snFormatValueGain(float val, char* str)
{
	sprintf(str, "%2.2f", val * 24);
}

//------------------------------------------------------------------------------------

static void snFormatValueCeil(float val, char* str)
{
	sprintf(str, "%2.2f", (1 - val) * -24);
}

//------------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------------

static CTextEdit* snCreateTextEdit(CCoord x, CCoord y, CControlListener* cl, long id)
{
	CColor clr = {168, 168, 168, 255};
	CRect rc(x, y, x + SN05_TEXT_W, y + SN05_TEXT_H);
	CTextEdit* tx = new CTextEdit(rc, cl, id);
	tx->setFont(kNormalFontVerySmall);
	tx->setTransparency(true);
	tx->setFontColor(clr);
	switch(id)
	{
	case IDC_TX_GAIN: tx->setStringConvert(snFormatValueGain); break;
	case IDC_TX_CEIL: tx->setStringConvert(snFormatValueCeil); break;
	}
	
	return tx;
}
*/

//------------------------------------------------------------------------------------
// GUI
//------------------------------------------------------------------------------------

SignalNoiseLimiterGUI::SignalNoiseLimiterGUI(SignalNoiseLimiter& p)
	: AudioProcessorEditor(&p), processor(p)
{
	// Load background image
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn05g_bk_png,
		BinaryData::sn05g_bk_pngSize
	);

	// GR meter image
	juce::Image meterImage = juce::ImageCache::getFromMemory(
		BinaryData::sn05g_gr_png,
		BinaryData::sn05g_gr_pngSize
	);

	// Knobs
	juce::Image knobLargeImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b1_png,
		BinaryData::sn01g_b1_pngSize
	);

	juce::Image knobNormalImage = juce::ImageCache::getFromMemory(
		BinaryData::sn05g_b1_png,
		BinaryData::sn05g_b1_pngSize
	);

	juce::Image switchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s3_png,
		BinaryData::sn01g_s3_pngSize
	);

	juce::Image hponImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_ld_png,
		BinaryData::sn04g_ld_pngSize
	);

	jassert(!meterImage.isNull());
	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());
	jassert(!switchImage.isNull());
	jassert(!hponImage.isNull());

	largeLNF.setImage(knobLargeImage);
	normalLNF.setImage(knobNormalImage);

	gainKnob = setupKnobPrecise(gParams[SNE_GAIN], &largeLNF, gainLabel);
	ceilKnob = setupKnobPrecise(gParams[SNE_CEIL], &largeLNF, ceilLabel);
	hpfcKnob = setupKnob(gParams[SNE_HPFC], &normalLNF);
	atkhKnob = setupKnob(gParams[SNE_ATKH], &normalLNF);
	relhKnob = setupKnob(gParams[SNE_RELH], &normalLNF);
	relsKnob = setupKnob(gParams[SNE_RELS], &normalLNF);
	clipKnob = setupKnob(gParams[SNE_CLIP], &normalLNF);

	auto& params = processor.getParameters();

	modeSwitch = std::make_unique<SignalNoiseSwitchButton>("modeSwitch", switchImage);
	modeSwitch->attachToParameter(params, gParams[SNE_MODE].id);
	addAndMakeVisible(*modeSwitch);

	hponSwitch = std::make_unique<SignalNoiseSwitchButton>("hponSwitch", hponImage);
	hponSwitch->attachToParameter(params, gParams[SNE_HPON].id);
	addAndMakeVisible(*hponSwitch);

	meterLimiter = std::make_unique<SignalNoiseLimiterGR>(meterImage);
	addAndMakeVisible(*meterLimiter);

	meterClipper = std::make_unique<SignalNoiseLimiterGR>(meterImage);
	addAndMakeVisible(*meterClipper);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseLimiterGUI::setupKnobPrecise(
	const ParamDesc& p,
	juce::LookAndFeel* lnF,
	SignalNoiseKnobLabel& label)
{
	auto& params = processor.getParameters();

	auto knob = std::make_unique<SignalNoiseKnobPrecise>(p.defaultValue);

	knob->setLookAndFeel(lnF);
	knob->attachToParameter(params, p.id);
	knob->attachLabel(&label);
	label.attachKnob(knob.get());
	addAndMakeVisible (*knob);
	addAndMakeVisible(label);

	return knob;
}

std::unique_ptr<SignalNoiseKnob> SignalNoiseLimiterGUI::setupKnob(
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

SignalNoiseLimiterGUI::~SignalNoiseLimiterGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGUI::resized()
{
	gainKnob->setBounds (30, 35, 80, 80);  // Gain
	ceilKnob->setBounds (400, 35, 80, 80); // Ceiling

	hpfcKnob->setBounds (155, 20, 40, 40); // HPF frequency
	atkhKnob->setBounds (235, 20, 40, 40); // Attack (H)
	relhKnob->setBounds (315, 20, 40, 40); // Release (H)
	clipKnob->setBounds (155, 93, 40, 40); // Soft clip %
	relsKnob->setBounds (315, 93, 40, 40); // Release (S)

	hponSwitch->setBounds(121, 8, 28, 28);
	modeSwitch->setBounds(235, 93, 40, 40);

	meterLimiter->setBounds(55, 179, 400, 10);
	meterClipper->setBounds(55, 191, 400, 10);

	gainLabel.setBounds(55, 135, 30, 14);
	ceilLabel.setBounds(425, 135, 30, 14);
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGUI::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//------------------------------------------------------------------------------------
/*
void SignalNoiseLimiterGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_GAIN:	if(_gain) _gain->setValue(effect->getParameter(at));
					if(_txtg) _txtg->setValue(effect->getParameter(at)); break;
	case SNE_CEIL:	if(_ceil) _ceil->setValue(effect->getParameter(at)); 
					if(_txtc) _txtc->setValue(effect->getParameter(at)); break;
	case SNE_HPFC:	if(_hpfc) _hpfc->setValue(effect->getParameter(at)); break;
	case SNE_ATKH:	if(_atkh) _atkh->setValue(effect->getParameter(at)); break;
	case SNE_RELH:	if(_relh) _relh->setValue(effect->getParameter(at)); break;
	case SNE_RELS:	if(_rels) _rels->setValue(effect->getParameter(at)); break;
	case SNE_CLIP:	if(_clip) _clip->setValue(effect->getParameter(at)); break;
	case SNE_MODE:	if(_mode) _mode->setValue(effect->getParameter(at)); break;
	case SNE_HPON:	if(_hpon) _hpon->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
{
	if(_open == 0)
		return;

	float v;
	char* c = 0;
	char t[256] = {0};
	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_GAIN:
	case SNE_CEIL:
	case SNE_ATKH:
	case SNE_RELH:
	case SNE_RELS:
	case SNE_MODE:
	case SNE_HPON:
	case SNE_HPFC:
	case SNE_CLIP:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	case IDC_TX_GAIN:
		_txtg->getText(t);
		v = strtof(t, &c) / 24.f;
		effect->setParameter(SNE_GAIN, clampf(v));
		break;
	case IDC_TX_CEIL: 
		_txtc->getText(t);
		v = (strtof(t, &c) + 24) / 24.f;
		effect->setParameter(SNE_CEIL, clampf(v));
		break;
	}
}
*/
//------------------------------------------------------------------------------------
void SignalNoiseLimiterGUI::timerCallback()
{
	meterLimiter->setValue(processor.getLimiterGR());
	meterClipper->setValue(processor.getClipperGR());
}
