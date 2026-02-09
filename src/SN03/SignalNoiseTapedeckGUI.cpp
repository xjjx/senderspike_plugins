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

	jassert(!knobLargeImage.isNull());
	jassert(!knobNormalImage.isNull());

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
/*
	x = 240;
	y = 230;
	rc(x, y, x + SN03_MODES_SZ, y + SN03_MODES_SZ);
	_mode = new CHorizontalSwitch(rc, this, SNE_EQSC, 3, SN03_MODES_SZ, 3, modes, pt);
	_mode->setValue(effect->getParameter(SNE_EQSC));
	frm->addView(_mode);

	x = 212;
	y = 317;
	rc(x, y, x + 28, y + 84);
	_attn = new CVerticalSwitch(rc, this, SNE_ATTN, 3, 84, 3, three, pt);
	_attn->setValue(effect->getParameter(SNE_ATTN));
	frm->addView(_attn);

	x = 94;
	y = 401;
	rc(x, y, x + 40, y + 30);
	_hbon = new CHorizontalSwitch(rc, this, SNE_HBON, 2, 30, 2, slide, pt);
	_hbon->setValue(effect->getParameter(SNE_HBON));
	frm->addView(_hbon);

	x = 440;
	rc(x, y, x + 40, y + 30);
	_nois = new CHorizontalSwitch(rc, this, SNE_NOIS, 2, 30, 2, slide, pt);
	_nois->setValue(effect->getParameter(SNE_NOIS));
	frm->addView(_nois);

	x = 386;
	y = 188;
	rc(x, y, x + 40, y + 30);
	_loon = new COnOffButton(rc, this, SNE_LOON, buttn);
	_loon->setValue(effect->getParameter(SNE_LOON));
	frm->addView(_loon);

	// VU switches -----------------------------------------

	x = 160;
	rc(x, 5, x + 40, 35);
	_room = new CHorizontalSwitch(rc, this, SNE_ROOM, 4, 30, 4, displ, pt);
	_room->setValue(effect->getParameter(SNE_ROOM));
	frm->addView(_room);

	x = 240;
	rc(x, 0, x + 40, 30);
	_hold = new CHorizontalSwitch(rc, this, SNE_HOLD, 2, 30, 2, slide, pt);
	_hold->setValue(effect->getParameter(SNE_HOLD));
	frm->addView(_hold);

	x = 320;
	rc(x, 0, x + 40, 30);
	_path = new CHorizontalSwitch(rc, this, SNE_PATH, 2, 30, 2, slide, pt);
	_path->setValue(effect->getParameter(SNE_PATH));
	frm->addView(_path);

	// VU meter --------------------------------------------

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
//	inputMeter .setLevel(processor.getInputLevel());
///	outputMeter.setLevel(processor.getOutputLevel());
//	peakLed.setLevel(processor.getOutputLevel());

	repaint();
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
