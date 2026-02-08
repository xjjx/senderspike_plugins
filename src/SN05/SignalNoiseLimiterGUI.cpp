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

//------------------------------------------------------------------------------------
// GR meter
//------------------------------------------------------------------------------------

SignalNoiseLimiterGR::SignalNoiseLimiterGR(const CRect& rc, CBitmap* map, float fs) : CControl(rc, 0, 0)
{
	_val = 0;
	_map = map;

	if(_map)
		_map->remember();
}

//------------------------------------------------------------------------------------

SignalNoiseLimiterGR::~SignalNoiseLimiterGR()
{
	if(_map)
		_map->forget();
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGR::setVal(double dB)
{
	if(dB > _val)
		_val = dB;
	setDirty(true);
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

	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());

	largeLNF.setImage(knobLargeImage);
	normalLNF.setImage(knobNormalImage);

	auto& params = processor.getParameters();

	gainKnob = std::make_unique<SignalNoiseKnobPrecise>(0);
	gainKnob->setLookAndFeel(&largeLNF);
	gainKnob->attachToParameter(params, gParams[SNE_GAIN].id);
	addAndMakeVisible(*gainKnob);

	ceilKnob = std::make_unique<SignalNoiseKnobPrecise>(0);
	ceilKnob->setLookAndFeel(&largeLNF);
	ceilKnob->attachToParameter(params, gParams[SNE_CEIL].id);
	addAndMakeVisible(*ceilKnob);

	addAndMakeVisible (hpfcKnob);
	hpfcKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	hpfcKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
	hpfcKnob.setLookAndFeel(&normalLNF);
	hpfcAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_HPFC].id, hpfcKnob);

	addAndMakeVisible (atkhKnob);
	atkhKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	atkhKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
	atkhKnob.setLookAndFeel(&normalLNF);
	atkhAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_ATKH].id, atkhKnob);

	addAndMakeVisible (relhKnob);
	relhKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	relhKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
	relhKnob.setLookAndFeel(&normalLNF);
	relhAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_RELH].id, relhKnob);

	addAndMakeVisible (relsKnob);
	relsKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	relsKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
	relsKnob.setLookAndFeel(&normalLNF);
	relsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_RELS].id, relsKnob);

	addAndMakeVisible (clipKnob);
	clipKnob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
	clipKnob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
	clipKnob.setLookAndFeel(&normalLNF);
	clipAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_CLIP].id, clipKnob);

	modeSwitch = std::make_unique<SignalNoiseSwitchButton>("modeSwitch", switchImage);
	addAndMakeVisible(*modeSwitch);

	modeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
		params, gParams[SNE_MODE].id, *modeSwitch
	);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
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

	hpfcKnob.setBounds (155, 20, 40, 40); // HPF frequency
	atkhKnob.setBounds (235, 20, 40, 40); // Attack (H)
	relhKnob.setBounds (315, 20, 40, 40); // Release (H)
	clipKnob.setBounds (155, 93, 40, 40); // Soft clip %
	relsKnob.setBounds (315, 93, 40, 40); // Release (S)

	modeSwitch->setBounds(235, 93, 40, 40);
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

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGUI::trackMeter(double lim, double clp)
{
	if(_open == 0)
		return;
	_grHL->setVal(lim);
	_grHC->setVal(clp);
}

//------------------------------------------------------------------------------------
*/
void SignalNoiseLimiterGUI::timerCallback()
{
//	inputMeter .setLevel(processor.getInputLevel());
///	outputMeter.setLevel(processor.getOutputLevel());
//	peakLed.setLevel(processor.getOutputLevel());

	repaint();
}
