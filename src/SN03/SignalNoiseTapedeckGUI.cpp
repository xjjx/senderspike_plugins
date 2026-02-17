//------------------------------------------------------------------------------------
//
//	file:		sn_03g.cpp
//
//	purpose:	SN03 tape recorder emulator GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <stdio.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseTapedeckGUI.h"
#include "BinaryData.h"

//------------------------------------------------------------------------------------
/*
static void snFormatValueGain(float val, char* str)
{
	sprintf(str, "%2.2f", val * 48 - 24);
}

//------------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------------

static CTextEdit* snCreateTextEdit(CCoord x, CCoord y, CControlListener* cl, long id)
{
	CColor clr = {168, 168, 168, 255};
	CRect rc(x, y, x + SN03_TEXT_W, y + SN03_TEXT_H);
	CTextEdit* tx = new CTextEdit(rc, cl, id);
	tx->setFont(kNormalFontVerySmall);
	tx->setTransparency(true);
	tx->setFontColor(clr);
	switch(id)
	{
	case IDC_TX_GAIN:
	case IDC_TX_TRIM: tx->setStringConvert(snFormatValueGain); break;
	}
	
	return tx;
}
*/

//------------------------------------------------------------------------------------
// tape deck editor
//------------------------------------------------------------------------------------

SignalNoiseTapedeckGUI::SignalNoiseTapedeckGUI(SignalNoiseTapedeck& p)
: AudioProcessorEditor(&p), processor(p)
{
	// Load images
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn03g_bk_png,
		BinaryData::sn03g_bk_pngSize
	);

	juce::Image knobLargeImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b1_png,
		BinaryData::sn01g_b1_pngSize
	);

	juce::Image knobNormalImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_b2_png,
		BinaryData::sn01g_b2_pngSize
	);

	juce::Image roomImage = juce::ImageCache::getFromMemory(
		BinaryData::sn03g_b1_png,
		BinaryData::sn03g_b1_pngSize
	);

	juce::Image eqscImage = juce::ImageCache::getFromMemory(
		BinaryData::sn01g_s1_png,
		BinaryData::sn01g_s1_pngSize
	);

	juce::Image attnImage = juce::ImageCache::getFromMemory(
		BinaryData::sn03g_s1_png,
		BinaryData::sn03g_s1_pngSize
	);

	juce::Image switchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn03g_b2_png,
		BinaryData::sn03g_b2_pngSize
	);

	juce::Image loonImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s3_png,
		BinaryData::sn04g_s3_pngSize
	);

	juce::Image vuNeedleImage = juce::ImageCache::getFromMemory(
		BinaryData::sn02g_v1_png,
		BinaryData::sn02g_v1_pngSize
	);

	juce::Image peakNeedleImage = juce::ImageCache::getFromMemory(
		BinaryData::sn02g_p1_png,
		BinaryData::sn02g_p1_pngSize
	);

	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());
	jassert(!roomImage.isNull());
	jassert(!switchImage.isNull());
	jassert(!loonImage.isNull());
	jassert(!eqscImage.isNull());
	jassert(!attnImage.isNull());
	jassert(!vuNeedleImage.isNull());
	jassert(!peakNeedleImage.isNull());

	// Knobs
	largeLNF.setImage(knobLargeImage);
	normalLNF.setImage(knobNormalImage);

	trimKnob = setupKnobPrecise(gParams[SNE_TRIM], &largeLNF);	// input trim
	gainKnob = setupKnobPrecise(gParams[SNE_GAIN], &largeLNF);	// output gain
	rcloKnob = setupKnob(gParams[SNE_RCLO], &normalLNF);		// rec lo
	rchiKnob = setupKnob(gParams[SNE_RCHI], &normalLNF);		// rec hi
	rploKnob = setupKnob(gParams[SNE_RPLO], &normalLNF);		// rep lo
	rphiKnob = setupKnob(gParams[SNE_RPHI], &normalLNF);		// rep hi
	headKnob = setupKnob(gParams[SNE_HEAD], &normalLNF);		// head Hz
	bumpKnob = setupKnob(gParams[SNE_BUMP], &normalLNF);		// head dB
	hissKnob = setupKnob(gParams[SNE_HISS], &normalLNF);		// hiss dB

	auto& params = processor.getParameters();

	// Room switch
	roomLNF.setImage(roomImage, 4);
	roomSwitch.setSliderStyle(juce::Slider::LinearBarVertical);
	roomSwitch.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	roomSwitch.setRange(0.0, 3.0, 1.0);
	roomSwitch.setLookAndFeel(&roomLNF);

	roomAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_ROOM].id, roomSwitch
	);
	addAndMakeVisible(roomSwitch);

	// EQSC Switch
	eqscLNF.setImage(eqscImage, 3);
	eqscSwitch.setSliderStyle(juce::Slider::LinearBar);
	eqscSwitch.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	eqscSwitch.setRange(0.0, 2.0, 1.0);
	eqscSwitch.setLookAndFeel(&eqscLNF);
	eqscAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_EQSC].id, eqscSwitch
	);
	addAndMakeVisible(eqscSwitch);

	// Attn Switch
	attnLNF.setImage(attnImage, 3);
	attnLNF.reverse = true;

	attnSwitch.setSliderStyle(juce::Slider::LinearBarVertical);
	attnSwitch.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	attnSwitch.setRange(0.0, 2.0, 1.0);
	attnSwitch.setLookAndFeel(&attnLNF);
	attnAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
		params, gParams[SNE_ATTN].id, attnSwitch
	);
	addAndMakeVisible(attnSwitch);

	// Switches
	holdSwitch = std::make_unique<SignalNoiseSwitchButton>("holdSwitch", switchImage); // VU hold peak
	holdSwitch->attachToParameter(params, gParams[SNE_HOLD].id);
	addAndMakeVisible(*holdSwitch);

	pathSwitch = std::make_unique<SignalNoiseSwitchButton>("pathSwitch", switchImage); // VU I/O switch
	pathSwitch->attachToParameter(params, gParams[SNE_PATH].id);
	addAndMakeVisible(*pathSwitch);

	hbonSwitch = std::make_unique<SignalNoiseSwitchButton>("hbonSwitch", switchImage); // bump on/off
	hbonSwitch->attachToParameter(params, gParams[SNE_HBON].id);
	addAndMakeVisible(*hbonSwitch);

	noisSwitch = std::make_unique<SignalNoiseSwitchButton>("noisSwitch", switchImage); // noise on/off
	noisSwitch->attachToParameter(params, gParams[SNE_NOIS].id);
	addAndMakeVisible(*noisSwitch);

	loonSwitch = std::make_unique<SignalNoiseSwitchButton>("loonSwitch", loonImage); // force LO on
	loonSwitch->attachToParameter(params, gParams[SNE_LOON].id);
	addAndMakeVisible(*loonSwitch);

	// VU Meter
	vuMeter = std::make_unique<SignalNoiseVU>(vuNeedleImage, peakNeedleImage, 130);
	addAndMakeVisible(*vuMeter);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseTapedeckGUI::setupKnobPrecise(
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

std::unique_ptr<SignalNoiseKnob> SignalNoiseTapedeckGUI::setupKnob(
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

SignalNoiseTapedeckGUI::~SignalNoiseTapedeckGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::resized()
{
	// Knob 1 (large knobs: 80x80)
	trimKnob->setBounds(30, 67, 80, 80);
	gainKnob->setBounds(410, 67, 80, 80);

	// Knob 2 (small knobs: 60x60)
	rcloKnob->setBounds(30, 220, 60, 60);
	rchiKnob->setBounds(140, 220, 60, 60);
	rploKnob->setBounds(320, 220, 60, 60);
	rphiKnob->setBounds(430, 220, 60, 60);
	headKnob->setBounds(30, 330, 60, 60);
	bumpKnob->setBounds(140, 330, 60, 60);
	hissKnob->setBounds(430, 330, 60, 60);

/*
	// text edits ------------------------------------------

	x = 110;
	y = 149;
	_txti = snCreateTextEdit(x, y, this, IDC_TX_TRIM);
	_txti->setValue(effect->getParameter(SNE_TRIM));
	frm->addView(_txti);

	x = 380;
	_txto = snCreateTextEdit(x, y, this, IDC_TX_GAIN);
	_txto->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_txto);
*/

	// switches --------------------------------------------
	hbonSwitch->setBounds(94, 401, 40, 30);
	noisSwitch->setBounds(440, 401, 40, 30);
	loonSwitch->setBounds(386, 188, 40, 30);
	eqscSwitch.setBounds(240, 230, 40, 40);
	attnSwitch.setBounds(212, 317, 28, 84);

	// VU switches -----------------------------------------
	roomSwitch.setBounds(160, 4, 40, 30);
	holdSwitch->setBounds(240, 0, 40, 30);
	pathSwitch->setBounds(320, 0, 40, 30);

	// VU meter --------------------------------------------
	vuMeter->setBounds(165, 62, 190, 90);

/*
	dword vut[] = {4, 21, 32, 42, 56, 65, 74, 84, 97, 109, 123, 129};	//valid for specific bitmap !!!

	x = 165;
	y = 62; 
	rc(x, y, x + SN03_METER_W, y + SN03_METER_H);
	_vumt = new SignalNoiseVU(rc, vublk, vut, effect->getSampleRate());
	frm->addView(_vumt);
	
	x = 356 - peakl->getWidth();
	y = 71; 
	rc(x, y, x + peakl->getWidth(), y + peakl->getWidth());
	_peak = new SignalNoisePeakLed(rc, peakl);
	frm->addView(_peak);

	if(effect->getParameter(SNE_HOLD) > 0.5)
		_vumt->setPeakBitmap(vured);

	float rm = effect->getParameter(SNE_ROOM);
	if(rm < .25f)		_vumt->setLevel(-12);
	else if(rm < .50f)	_vumt->setLevel(-14);
	else if(rm < .75f)	_vumt->setLevel(-18);
	else				_vumt->setLevel(-20);
*/

	_open = 1;
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::timerCallback()
{
	vuMeter->setLevel(processor.getVuLevel());
}

//------------------------------------------------------------------------------------
/*
void SignalNoiseTapedeckGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_TRIM:	if(_trim) _trim->setValue(effect->getParameter(at));
					if(_txti) _txti->setValue(effect->getParameter(at)); break;
	case SNE_GAIN:	if(_gain) _gain->setValue(effect->getParameter(at));
					if(_txto) _txto->setValue(effect->getParameter(at)); break;
	case SNE_RCLO:	if(_rclo) _rclo->setValue(effect->getParameter(at)); break;
	case SNE_RCHI:	if(_rchi) _rchi->setValue(effect->getParameter(at)); break;
	case SNE_RPLO:	if(_rplo) _rplo->setValue(effect->getParameter(at)); break;
	case SNE_RPHI:	if(_rphi) _rphi->setValue(effect->getParameter(at)); break;
	case SNE_HEAD:	if(_head) _head->setValue(effect->getParameter(at)); break;
	case SNE_BUMP:	if(_bump) _bump->setValue(effect->getParameter(at)); break;
	case SNE_HISS:	if(_hiss) _hiss->setValue(effect->getParameter(at)); break;
	case SNE_EQSC:	if(_mode) _mode->setValue(effect->getParameter(at)); break;
	case SNE_ROOM:	if(_room) _room->setValue(effect->getParameter(at)); break;
	case SNE_HOLD:	if(_hold) _hold->setValue(effect->getParameter(at)); break;
	case SNE_PATH:	if(_path) _path->setValue(effect->getParameter(at)); break;
	case SNE_ATTN:	if(_attn) _attn->setValue(effect->getParameter(at)); break;
	case SNE_NOIS:	if(_nois) _nois->setValue(effect->getParameter(at)); break;
	case SNE_HBON:	if(_hbon) _hbon->setValue(effect->getParameter(at)); break;
	case SNE_LOON:	if(_loon) _loon->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
{
	if(_open == 0)
		return;

	float v;
	char* c = 0;
	char t[256] = {0};
	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_TRIM:
	case SNE_GAIN:
	case SNE_RCLO:
	case SNE_RCHI:
	case SNE_RPLO:
	case SNE_RPHI:
	case SNE_HEAD:
	case SNE_BUMP:
	case SNE_HISS:
	case SNE_EQSC:
	case SNE_ROOM:
	case SNE_HOLD:
	case SNE_PATH:
	case SNE_ATTN:
	case SNE_NOIS:
	case SNE_HBON:
	case SNE_LOON:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	case IDC_TX_TRIM: 
		_txti->getText(t);
		v = (strtof(t, &c) + 24.f) / 48.f;
		effect->setParameter(SNE_TRIM, clampf(v));
		break;
	case IDC_TX_GAIN:
		_txto->getText(t);
		v = (strtof(t, &c) + 24.f) / 48.f;
		effect->setParameter(SNE_GAIN, clampf(v));
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::trackMeter(double A)
{
	if(_open == 0)
		return;
	_vumt->setAmp(A);
	_peak->setAmp(A);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::setupMeterLevel(double nl)
{
	if(_open == 0)
		return;
	_vumt->setLevel(nl);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::setupMeterFilter(double fs)
{
	if(_open == 0)
		return;
	_vumt->setFilter(fs);
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::setupMeterUseHold(bool on)
{
	if(_open == 0)
		return;
	CBitmap* map = new CBitmap(106);
	_vumt->setPeakBitmap(on ? map : 0);
	map->forget();
}

//------------------------------------------------------------------------------------
*/
