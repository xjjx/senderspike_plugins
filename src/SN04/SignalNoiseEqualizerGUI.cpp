//------------------------------------------------------------------------------------
//
//	file:		sn_04g.cpp
//
//	purpose:	SN04 Channel EQ GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <stdio.h>
#include <math.h>
#include <stdio.h>
#include <math.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "SignalNoiseEqualizerGUI.h"
#include "BinaryData.h"

//------------------------------------------------------------------------------------
// arc switch
//------------------------------------------------------------------------------------
/*
SignalNoiseEqualizerArcSwitch::SignalNoiseEqualizerArcSwitch(const CRect& rc, AudioEffect* eff) : CControl(rc)
{
	_eff = eff;
	_rad = (rc.right - rc.left) / 2.f;
}

//------------------------------------------------------------------------------------

SignalNoiseEqualizerArcSwitch::~SignalNoiseEqualizerArcSwitch()
{
	//empty
}

//------------------------------------------------------------------------------------
*/

//void SignalNoiseEqualizerArcSwitch::draw(CDrawContext* ctx)
//{
	// empty

	/* dbg draw
	CColor clr = {255};
	setDirty(false);
	ctx->setFillColor(clr);
	ctx->drawRect(size);
	//*/
//}

//------------------------------------------------------------------------------------
/*
void SignalNoiseEqualizerArcSwitch::mouse(CDrawContext* ctx, CPoint& pos, long btn)
{
	if(!bMouseEnabled)
		return;
 	if(btn == -1)
		btn = ctx->getMouseButtons();
	if(!(btn & kLButton))
		return;

	float x1 = _rad;
	float y1 = 0;
	float x2 = pos.x - (_rad + size.left);
	float y2 = pos.y - (_rad + size.top);
	
	float dp = x1 * x2 + y1 * y2;
	float dt = x1 * y2 - y1 * x2;
	float an = (atan2f(dt, dp) / float(M_PI) * 180.f) - 90.f;
	if(an < 0) an += 360.f;
	if(an > 345) an = -15;

	int res = 0;
	float flt = -15;
	for(int i = 0; i < 12; i++)
	{
		if(an < flt)
			break;
		res = i;
		flt += 30;
	}

// dbg msg
//	char str[32] = {0};
//	sprintf(str, "%2.2f | %2.2f | %i", an, flt, res);
	MessageBox(NULL, str, "", 0);
//
	if(res > 5)
		_eff->setParameter(SNE_LPAS, (res - 6) * 0.2f);
	else
		_eff->setParameter(SNE_HPAS, res * 0.2f);
}
*/
//------------------------------------------------------------------------------------
// class SignalNoiseEqualizerGUI
//------------------------------------------------------------------------------------

SignalNoiseEqualizerGUI::SignalNoiseEqualizerGUI(SignalNoiseEqualizer& p)
: AudioProcessorEditor(&p), processor(p)
{
	// Load images
	background = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_bk_png,
		BinaryData::sn04g_bk_pngSize
	);

	juce::Image innerKnobImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b0_png,
		BinaryData::sn04g_b0_pngSize
	);

	juce::Image rimImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b1_png,
		BinaryData::sn04g_b1_pngSize
	);

	juce::Image hpfImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b2_png,
		BinaryData::sn04g_b2_pngSize
	);

	juce::Image lpfImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b3_png,
		BinaryData::sn04g_b3_pngSize
	);

	juce::Image gainImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b4_png,
		BinaryData::sn04g_b4_pngSize
	);

	juce::Image switch4WayImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_b5_png,
		BinaryData::sn04g_b5_pngSize
	);

	juce::Image onOffSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_ld_png,
		BinaryData::sn04g_ld_pngSize
	);

	juce::Image bellSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s1_png,
		BinaryData::sn04g_s1_pngSize
	);

	juce::Image phaseSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s2_png,
		BinaryData::sn04g_s2_pngSize
	);

	juce::Image mojoSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s3_png,
		BinaryData::sn04g_s3_pngSize
	);

	juce::Image switch2WayImage = juce::ImageCache::getFromMemory(
		BinaryData::sn03g_b2_png,
		BinaryData::sn03g_b2_pngSize
	);

	jassert(innerKnobImage.isValid());
	jassert(rimImage.isValid());
	jassert(hpfImage.isValid());
	jassert(lpfImage.isValid());
	jassert(gainImage.isValid());
	jassert(onOffSwitchImage.isValid());
	jassert(phaseSwitchImage.isValid());
	jassert(mojoSwitchImage.isValid());
	jassert(switch2WayImage.isValid());

	// Knobs
	innerKnobLNF.setImage(innerKnobImage);
	rimLNF.setImage(rimImage);
	hpfLNF.setImage(hpfImage);
	lpfLNF.setImage(lpfImage);
	gainLNF.setImage(gainImage);
	octLNF.setImage(switch4WayImage, 4);

	_hf_f = setupKnob(gParams[SNE_HF_F], &rimLNF);      // HSF freq
	_hf_g = setupKnobPrecise(gParams[SNE_HF_G], &innerKnobLNF, _thfg); // HSF gain
	_hf_w = setupKnobPrecise(gParams[SNE_HF_Q], &gainLNF, _thfw); // HSF bw

	_mf_f = setupKnob(gParams[SNE_MF_F], &rimLNF);              // PKF freq
	_mf_g = setupKnobPrecise(gParams[SNE_MF_G], &innerKnobLNF, _tmfg); // PKF gain
	_mf_w = setupKnobPrecise(gParams[SNE_MF_Q], &gainLNF, _tmfw);     // PKF bw

	_lf_f = setupKnob(gParams[SNE_LF_F], &rimLNF);              // LSF freq
	_lf_g = setupKnobPrecise(gParams[SNE_LF_G], &innerKnobLNF, _tlfg); // LSF gain
	_lf_w = setupKnobPrecise(gParams[SNE_LF_Q], &gainLNF, _tlfw);     // LSF bw

	_hpas = setupKnob(gParams[SNE_HPAS], &hpfLNF);              // HPF freq
	_lpas = setupKnob(gParams[SNE_LPAS], &lpfLNF);       // LPF freq
	_gain = setupKnobPrecise(gParams[SNE_GAIN], &gainLNF, _tomg);     // output

	// Switches
	auto& params = processor.getParameters();

	_hf_b = std::make_unique<SignalNoiseSwitchButton>("hf_b", onOffSwitchImage);
	_hf_b->attachToParameter(params, gParams[SNE_HF_B].id);
	addAndMakeVisible(*_hf_b);

	_hf_m = std::make_unique<SignalNoiseSwitchButton>("hf_m", bellSwitchImage);
	_hf_m->attachToParameter(params, gParams[SNE_HF_M].id);
	addAndMakeVisible(*_hf_m);

	_hf_t = std::make_unique<SignalNoiseSwitchButton>("_hf_t", switch2WayImage);
	_hf_t->attachToParameter(params, gParams[SNE_HF_T].id);
	addAndMakeVisible(*_hf_t);

	_mf_b = std::make_unique<SignalNoiseSwitchButton>("mf_b", onOffSwitchImage);
	_mf_b->attachToParameter(params, gParams[SNE_MF_B].id);
	addAndMakeVisible(*_mf_b);

	_mf_t = std::make_unique<SignalNoiseSwitchButton>("_mf_t", switch2WayImage);
	_mf_t->attachToParameter(params, gParams[SNE_MF_T].id);
	addAndMakeVisible(*_mf_t);

	_lf_b = std::make_unique<SignalNoiseSwitchButton>("lf_b", onOffSwitchImage);
	_lf_b->attachToParameter(params, gParams[SNE_LF_B].id);
	addAndMakeVisible(*_lf_b);

	_lf_m = std::make_unique<SignalNoiseSwitchButton>("lf_m", bellSwitchImage);
	_lf_m->attachToParameter(params, gParams[SNE_LF_M].id);
	addAndMakeVisible(*_lf_m);

	_lf_t = std::make_unique<SignalNoiseSwitchButton>("_lf_t", switch2WayImage);
	_lf_t->attachToParameter(params, gParams[SNE_LF_T].id);
	addAndMakeVisible(*_lf_t);

	_hp_b = std::make_unique<SignalNoiseSwitchButton>("hp_b", onOffSwitchImage);
	_hp_b->attachToParameter(params, gParams[SNE_HP_B].id);
	addAndMakeVisible(*_hp_b);

	_lp_b = std::make_unique<SignalNoiseSwitchButton>("lp_b", onOffSwitchImage);
	_lp_b->attachToParameter(params, gParams[SNE_LP_B].id);
	addAndMakeVisible(*_lp_b);

	_iphs = std::make_unique<SignalNoiseSwitchButton>("iphs", phaseSwitchImage);
	_iphs->attachToParameter(params, gParams[SNE_IPHS].id);
	addAndMakeVisible(*_iphs);

	_mojo = std::make_unique<SignalNoiseSwitchButton>("mojo", mojoSwitchImage);
	_mojo->attachToParameter(params, gParams[SNE_MOJO].id);
	addAndMakeVisible(*_mojo);

	// Switches filters oct
	_loct = setupKnob(gParams[SNE_LOCT], &octLNF);
	_loct->setSliderStyle(juce::Slider::LinearBar);
	_hoct = setupKnob(gParams[SNE_HOCT], &octLNF);
	_hoct->setSliderStyle(juce::Slider::LinearBar);

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseEqualizerGUI::setupKnobPrecise(
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

std::unique_ptr<SignalNoiseKnob> SignalNoiseEqualizerGUI::setupKnob(
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

SignalNoiseEqualizerGUI::~SignalNoiseEqualizerGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizerGUI::resized()
{

	// number click switches -------------------------------
/*

	x = 20;
	y = 20;
	rc(x, y, x + 120, y + 120);
	_hsfc = new SignalNoiseArcSwitch(rc, 15, 30, 8, SNE_HF_F, effect);
	frm->addView(_hsfc);

	y = 160;
	rc(x, y, x + 120, y + 120);
	_msfc = new SignalNoiseArcSwitch(rc, 15, 30, 8, SNE_MF_F, effect);
	frm->addView(_msfc);

	y = 300;
	rc(x, y, x + 120, y + 120);
	_lsfc = new SignalNoiseArcSwitch(rc, 15, 30, 8, SNE_LF_F, effect);
	frm->addView(_lsfc);

	y = 480;
	rc(x, y, x + 120, y + 120);
	_hplp = new SignalNoiseEqualizerArcSwitch(rc, effect);
	frm->addView(_hplp);
*/

	// HSF knobs -------------------------------------------
	_hf_f->setBounds(40, 40, 80, 80);
	_hf_g->setBounds(50, 50, 60, 60);
	_hf_w->setBounds(170, 40, 80, 80);

	// PKF knobs -------------------------------------------
	_mf_f->setBounds(40, 180, 80, 80);
	_mf_g->setBounds(50, 190, 60, 60);
	_mf_w->setBounds(170, 180, 80, 80);

	// LSF knobs -------------------------------------------
	_lf_f->setBounds(40, 320, 80, 80);
	_lf_g->setBounds(50, 330, 60, 60);
	_lf_w->setBounds(170, 320, 80, 80);

	// LPF/HPF knobs ---------------------------------------
	_hpas->setBounds(40, 500, 80, 80);
	_lpas->setBounds(50, 510, 60, 60);

	// volume knob -----------------------------------------
	_gain->setBounds(170, 500, 80, 80);

	// switches --------------------------------------------
	_hf_b->setBounds(131, 80, 28, 28);
	_mf_b->setBounds(131, 220, 28, 28);
	_lf_b->setBounds(131, 360, 28, 28);
	_hp_b->setBounds(12, 592, 28, 28);
	_lp_b->setBounds(121, 460, 28, 28);
	_loct->setBounds(32, 460, 60, 30);
	_hoct->setBounds(68, 590, 60, 30);

	// push buttons ----------------------------------------
	_mojo->setBounds(133, 603, 40, 30);
	_iphs->setBounds(190, 581, 40, 30);
	_hf_m->setBounds(60, 120, 40, 30);
	_lf_m->setBounds(60, 400, 40, 30);
	_hf_t->setBounds(190, 120, 40, 30);
	_mf_t->setBounds(190, 260, 40, 30);
	_lf_t->setBounds(190, 400, 40, 30);

/*
	// leds & mutes ----------------------------------------

	x = 230;
	y = 605;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_pkld = new SignalNoisePeakLed(rc, pkled);
	frm->addView(_pkld);
*/

	// text edits ------------------------------------------
	_thfg.setBounds(105, 117, 30, 14);
	_tmfg.setBounds(105, 257, 30, 14);
	_tlfg.setBounds(105, 397, 30, 14);
	_thfw.setBounds(245, 40, 30, 14);
	_tmfw.setBounds(245, 180, 30, 14);
	_tlfw.setBounds(245, 320, 30, 14);
	_tomg.setBounds(245, 500, 30, 14);
}

//------------------------------------------------------------------------------------
void SignalNoiseEqualizerGUI::paint(juce::Graphics& g)
{
	// Fill background with black if image fails
	g.fillAll(juce::Colours::black);

	if (background.isValid())
		g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//------------------------------------------------------------------------------------
/*
void SignalNoiseEqualizerGUI::idle()
{
	// always call this first!!!
	AEffGUIEditor::idle();

	// custom code -----------------------------------------

	dword dt = GetTickCount();
	bool ch = dt - _time >= 500;

	if(ch)
	{
		_blnk = !_blnk;
		_time = dt;
	}

	if(_open == 0)
		return;

	if(_hfon && effect->getParameter(SNE_HF_B) > 0.5) _hfon->setBlinkState(_blnk);
	if(_mfon && effect->getParameter(SNE_MF_B) > 0.5) _mfon->setBlinkState(_blnk);
	if(_lfon && effect->getParameter(SNE_LF_B) > 0.5) _lfon->setBlinkState(_blnk);
	if(_lpon && effect->getParameter(SNE_LP_B) > 0.5) _lpon->setBlinkState(_blnk);
	if(_hpon && effect->getParameter(SNE_HP_B) > 0.5) _hpon->setBlinkState(_blnk);
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizerGUI::trackPeaks(double A)
{
	if(_open == 0)
		return;
	_pkld->setAmp(A);
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizerGUI::setLed(band_e b, bool on)
{
	if(_open == 0)
		return;

	switch(b)
	{
	case HF: if(_hfon) _hfon->setOn(on); break;
	case MF: if(_mfon) _mfon->setOn(on); break;
	case LF: if(_lfon) _lfon->setOn(on); break;
	case LP: if(_lpon) _lpon->setOn(on); break;
	case HP: if(_hpon) _hpon->setOn(on); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseEqualizerGUI::setBlink(band_e b, bool on)
{
	if(_open == 0)
		return;

	switch(b)
	{
	case HF: if(_hfon) _hfon->setBlink(on); break;
	case MF: if(_mfon) _mfon->setBlink(on); break;
	case LF: if(_lfon) _lfon->setBlink(on); break;
	case LP: if(_lpon) _lpon->setBlink(on); break;
	case HP: if(_hpon) _hpon->setBlink(on); break;
	}
}
*/
//------------------------------------------------------------------------------------
void SignalNoiseEqualizerGUI::timerCallback()
{
	// empty
}
