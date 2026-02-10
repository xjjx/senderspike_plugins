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
/*
static void snFormatValue18(float val, char* str)
{
	sprintf(str, "%2.2f", val * 36 - 18);
}

//------------------------------------------------------------------------------------

static void snFormatValue25(float val, char* str)
{
	sprintf(str, "%2.2f", val * 50 - 25);
}

//------------------------------------------------------------------------------------

static void snFormatValue20(float val, char* str)
{
	sprintf(str, "%2.2f", val * 40 - 20);
}

//------------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------------

static void snSetKnobInnerMouse(CControl* ctrl, CCoord x, CCoord y)
{
	x += 8;
	y += 8;
	CCoord s = SN04_KNOB2_W - 16;
	CRect rc(x, y, x + s, y + s);
	ctrl->setMouseableArea(rc);
}

//------------------------------------------------------------------------------------

static void snSetSwitchInnerMouse(CControl* ctrl, CCoord x, CCoord y, int off)
{
	CCoord s;

	if(off < 0)
	{
		y -= off;
		s = 30 + off;
	}
	else
	{
		s = 30 - off;
	}
	CRect rc(x, y, x + 60, y + s);
	ctrl->setMouseableArea(rc);
}

//------------------------------------------------------------------------------------

static CTextEdit* snCreateTextEdit(CCoord x, CCoord y, CControlListener* cl, long id)
{
	CColor clr = {168, 168, 168, 255};
	CRect rc(x, y, x + SN04_TEXT_W, y + SN04_TEXT_H);
	CTextEdit* tx = new CTextEdit(rc, cl, id);
	tx->setFont(kNormalFontVerySmall);
	tx->setTransparency(true);
	tx->setFontColor(clr);
	switch(id)
	{
	case IDC_TX_GAIN: tx->setStringConvert(snFormatValue25); break;
	case IDC_TX_HF_G:
	case IDC_TX_MF_G:
	case IDC_TX_LF_G: tx->setStringConvert(snFormatValue18); break;
	case IDC_TX_HF_W:
	case IDC_TX_MF_W:
	case IDC_TX_LF_W: tx->setStringConvert(snFormatValue20); break;
	}
	
	return tx;
}

//------------------------------------------------------------------------------------

static void snSetValueFromText(CTextEdit* tx, SignalNoiseEqualizerGUI* gui, VstInt32 at, float rng)
{
	char* c = 0;
	char t[256] = {0};

	tx->getText(t);
	float v = (strtof(t, &c) + rng) / (rng * 2);

	gui->getEffect()->setParameter(at, clampf(v));
}

//------------------------------------------------------------------------------------
// arc switch
//------------------------------------------------------------------------------------

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

	juce::Image onOffSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_ld_png,
		BinaryData::sn04g_ld_pngSize
	);

	juce::Image phaseSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s2_png,
		BinaryData::sn04g_s2_pngSize
	);

	juce::Image mojoSwitchImage = juce::ImageCache::getFromMemory(
		BinaryData::sn04g_s3_png,
		BinaryData::sn04g_s3_pngSize
	);

	jassert(innerKnobImage.isValid());
	jassert(rimImage.isValid());
	jassert(hpfImage.isValid());
	jassert(lpfImage.isValid());
	jassert(gainImage.isValid());
	jassert(onOffSwitchImage.isValid());
	jassert(phaseSwitchImage.isValid());
	jassert(mojoSwitchImage.isValid());

	// Knobs
	innerKnobLNF.setImage(innerKnobImage);
	rimLNF.setImage(rimImage);
	hpfLNF.setImage(hpfImage);
	lpfLNF.setImage(lpfImage);
	gainLNF.setImage(gainImage);

	_hf_f = setupKnob(gParams[SNE_HF_F], &rimLNF);      // HSF freq
	_hf_g = setupKnobPrecise(gParams[SNE_HF_G], &innerKnobLNF); // HSF gain
	_hf_w = setupKnobPrecise(gParams[SNE_HF_Q], &gainLNF); // HSF bw

	_mf_f = setupKnob(gParams[SNE_MF_F], &rimLNF);              // PKF freq
	_mf_g = setupKnobPrecise(gParams[SNE_MF_G], &innerKnobLNF); // PKF gain
	_mf_w = setupKnobPrecise(gParams[SNE_MF_Q], &gainLNF);     // PKF bw

	_lf_f = setupKnob(gParams[SNE_LF_F], &rimLNF);              // LSF freq
	_lf_g = setupKnobPrecise(gParams[SNE_LF_G], &innerKnobLNF); // LSF gain
	_lf_w = setupKnobPrecise(gParams[SNE_LF_Q], &gainLNF);     // LSF bw

	_hpas = setupKnob(gParams[SNE_HPAS], &rimLNF);              // HPF freq
	_lpas = setupKnob(gParams[SNE_LPAS], &innerKnobLNF);       // LPF freq
	_gain = setupKnobPrecise(gParams[SNE_GAIN], &gainLNF);     // output

	// Switches
	auto& params = processor.getParameters();

	_hf_b = std::make_unique<SignalNoiseSwitchButton>("hf_b", onOffSwitchImage);
	_hf_b->attachToParameter(params, gParams[SNE_HF_B].id);
	addAndMakeVisible(*_hf_b);

	_mf_b = std::make_unique<SignalNoiseSwitchButton>("mf_b", onOffSwitchImage);
	_mf_b->attachToParameter(params, gParams[SNE_MF_B].id);
	addAndMakeVisible(*_mf_b);

	_lf_b = std::make_unique<SignalNoiseSwitchButton>("lf_b", onOffSwitchImage);
	_lf_b->attachToParameter(params, gParams[SNE_LF_B].id);
	addAndMakeVisible(*_lf_b);

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

	// Set initial size based on background
	setSize(background.getWidth(), background.getHeight());

	startTimerHz(30); // GUI refresh rate
}

std::unique_ptr<SignalNoiseKnobPrecise> SignalNoiseEqualizerGUI::setupKnobPrecise(
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

	// push buttons ----------------------------------------
	_mojo->setBounds(133, 603, 40, 30);
	_iphs->setBounds(190, 581, 40, 30);

/*
	x = 190;
	y = 120;
	rc(x, y, x + 40, y + 30);
	_hf_t = new CHorizontalSwitch(rc, this, SNE_HF_T, 2, 30, 2, modes, pt);
	_hf_t->setValue(effect->getParameter(SNE_HF_T));
	frm->addView(_hf_t);

	y = 260;
	rc(x, y, x + 40, y + 30);
	_mf_t = new CHorizontalSwitch(rc, this, SNE_MF_T, 2, 30, 2, modes, pt);
	_mf_t->setValue(effect->getParameter(SNE_MF_T));
	frm->addView(_mf_t);

	y = 400;
	rc(x, y, x + 40, y + 30);
	_lf_t = new CHorizontalSwitch(rc, this, SNE_LF_T, 2, 30, 2, modes, pt);
	_lf_t->setValue(effect->getParameter(SNE_LF_T));
	frm->addView(_lf_t);

	x = 32;
	y = 460;
	rc(x, y, x + 60, y + 30);
	_loct = new CHorizontalSwitch(rc, this, SNE_LOCT, 4, 30, 4, slope, pt);
	_loct->setValue(effect->getParameter(SNE_LOCT));
	frm->addView(_loct);

	snSetSwitchInnerMouse(_loct, x, y, 5);

	x = 68;
	y = 590;
	rc(x, y, x + 60, y + 30);
	_hoct = new CHorizontalSwitch(rc, this, SNE_HOCT, 4, 30, 4, slope, pt);
	_hoct->setValue(effect->getParameter(SNE_HOCT));
	frm->addView(_hoct);

	snSetSwitchInnerMouse(_hoct, x, y, -5);

	// push buttons ----------------------------------------

	x = 60;
	y = 120;
	rc(x, y, x + 40, y + 30);
	_hf_m = new COnOffButton(rc, this, SNE_HF_M, bells);
	_hf_m->setValue(effect->getParameter(SNE_HF_M));
	frm->addView(_hf_m);

	y = 400;
	rc(x, y, x + 40, y + 30);
	_lf_m = new COnOffButton(rc, this, SNE_LF_M, bells);
	_lf_m->setValue(effect->getParameter(SNE_LF_M));
	frm->addView(_lf_m);

	// leds & mutes ----------------------------------------

	x = 230;
	y = 605;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_pkld = new SignalNoisePeakLed(rc, pkled);
	frm->addView(_pkld);

	// text edits ------------------------------------------

	x = 105;
	y = 117;
	_thfg = snCreateTextEdit(x, y, this, IDC_TX_HF_G);
	_thfg->setValue(effect->getParameter(SNE_HF_G));
	frm->addView(_thfg);

	y = 257;
	_tmfg = snCreateTextEdit(x, y, this, IDC_TX_MF_G);
	_tmfg->setValue(effect->getParameter(SNE_MF_G));
	frm->addView(_tmfg);

	y = 397;
	_tlfg = snCreateTextEdit(x, y, this, IDC_TX_LF_G);
	_tlfg->setValue(effect->getParameter(SNE_LF_G));
	frm->addView(_tlfg);

	x = 245;
	y = 40;
	_thfw = snCreateTextEdit(x, y, this, IDC_TX_HF_W);
	_thfw->setValue(effect->getParameter(SNE_HF_Q));
	frm->addView(_thfw);

	y = 180;
	_tmfw = snCreateTextEdit(x, y, this, IDC_TX_MF_W);
	_tmfw->setValue(effect->getParameter(SNE_MF_Q));
	frm->addView(_tmfw);

	y = 320;
	_tlfw = snCreateTextEdit(x, y, this, IDC_TX_LF_W);
	_tlfw->setValue(effect->getParameter(SNE_LF_Q));
	frm->addView(_tlfw);

	y = 500;
	_tomg = snCreateTextEdit(x, y, this, IDC_TX_GAIN);
	_tomg->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_tomg);

	// finalize --------------------------------------------
*/
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

void SignalNoiseEqualizerGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
//output
	case SNE_GAIN:	if(_gain) _gain->setValue(effect->getParameter(at));
					if(_tomg) _tomg->setValue(effect->getParameter(at)); break;
	case SNE_IPHS:	if(_iphs) _iphs->setValue(effect->getParameter(at)); break;
//HF
	case SNE_HF_F:	if(_hf_f) _hf_f->setValue(effect->getParameter(at)); break;
	case SNE_HF_G:	if(_hf_g) _hf_g->setValue(effect->getParameter(at)); 
					if(_thfg) _thfg->setValue(effect->getParameter(at)); break;
	case SNE_HF_Q:	if(_hf_w) _hf_w->setValue(effect->getParameter(at));
					if(_thfw) _thfw->setValue(effect->getParameter(at)); break;
	case SNE_HF_T:	if(_hf_t) _hf_t->setValue(effect->getParameter(at)); break;
	case SNE_HF_M:	if(_hf_m) _hf_m->setValue(effect->getParameter(at)); break;
//MF
	case SNE_MF_F:	if(_mf_f) _mf_f->setValue(effect->getParameter(at)); break;
	case SNE_MF_G:	if(_mf_g) _mf_g->setValue(effect->getParameter(at));
					if(_tmfg) _tmfg->setValue(effect->getParameter(at)); break;
	case SNE_MF_Q:	if(_mf_w) _mf_w->setValue(effect->getParameter(at));
					if(_tmfw) _tmfw->setValue(effect->getParameter(at)); break;
	case SNE_MF_T:	if(_mf_t) _mf_t->setValue(effect->getParameter(at)); break;
//LF
	case SNE_LF_F:	if(_lf_f) _lf_f->setValue(effect->getParameter(at)); break;
	case SNE_LF_G:	if(_lf_g) _lf_g->setValue(effect->getParameter(at));
					if(_tlfg) _tlfg->setValue(effect->getParameter(at)); break;
	case SNE_LF_Q:	if(_lf_w) _lf_w->setValue(effect->getParameter(at));
					if(_tlfw) _tlfw->setValue(effect->getParameter(at)); break;
	case SNE_LF_T:	if(_lf_t) _lf_t->setValue(effect->getParameter(at)); break;
	case SNE_LF_M:	if(_lf_m) _lf_m->setValue(effect->getParameter(at)); break;
//HPF/LPF
	case SNE_LPAS:	if(_lpas) _lpas->setValue(effect->getParameter(at)); break;
	case SNE_HPAS:	if(_hpas) _hpas->setValue(effect->getParameter(at)); break;
	case SNE_LOCT:	if(_loct) _loct->setValue(effect->getParameter(at)); break;
	case SNE_HOCT:	if(_hoct) _hoct->setValue(effect->getParameter(at)); break;
//"analog"
	case SNE_MOJO:	if(_mojo) _mojo->setValue(effect->getParameter(at)); break;
//mutes
	case SNE_HF_B:	if(_hf_b) _hf_b->setValue(effect->getParameter(at)); break;
	case SNE_MF_B:	if(_mf_b) _mf_b->setValue(effect->getParameter(at)); break;
	case SNE_LF_B:	if(_lf_b) _lf_b->setValue(effect->getParameter(at)); break;
	case SNE_LP_B:	if(_lp_b) _lp_b->setValue(effect->getParameter(at)); break;
	case SNE_HP_B:	if(_hp_b) _hp_b->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

static void snValueChanged(AudioEffect* e, CControl* c, long tag)
{
	e->setParameterAutomated(tag, c->getValue());
	c->setDirty();
}

static void snValueChanged(AudioEffect* e, CControl* c, long tag, float step)
{
	float v = c->getValue();
	c->setValue(floorf(v * step) / step);
	e->setParameterAutomated(tag, c->getValue());
	c->setDirty();
}

void SignalNoiseEqualizerGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
{
	if(_open == 0)
		return;

	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_GAIN:
	case SNE_IPHS:
	case SNE_HF_G:
	case SNE_HF_Q:
	case SNE_HF_T:
	case SNE_HF_M:
	case SNE_MF_G:
	case SNE_MF_Q:
	case SNE_MF_T:
	case SNE_LF_G:
	case SNE_LF_Q:
	case SNE_LF_T:
	case SNE_LF_M:
	case SNE_LOCT:
	case SNE_HOCT:
	case SNE_MOJO:
	case SNE_HF_B:
	case SNE_MF_B:
	case SNE_LF_B:
	case SNE_LP_B:
	case SNE_HP_B:
		snValueChanged(effect, ctrl, tag);
		break;
	case SNE_HF_F:
	case SNE_MF_F:
	case SNE_LF_F:
		snValueChanged(effect, ctrl, tag, 7);
		break;
	case SNE_LPAS:
	case SNE_HPAS:
		snValueChanged(effect, ctrl, tag, 5);
		break;
	case IDC_TX_HF_G: snSetValueFromText(_thfg, this, SNE_HF_G, 18); break;
	case IDC_TX_HF_W: snSetValueFromText(_thfw, this, SNE_HF_Q, 20); break;
	case IDC_TX_MF_G: snSetValueFromText(_tmfg, this, SNE_MF_G, 18); break;
	case IDC_TX_MF_W: snSetValueFromText(_tmfw, this, SNE_MF_Q, 20); break;
	case IDC_TX_LF_G: snSetValueFromText(_tlfg, this, SNE_LF_G, 18); break;
	case IDC_TX_LF_W: snSetValueFromText(_tlfw, this, SNE_LF_Q, 20); break;
	case IDC_TX_GAIN: snSetValueFromText(_tomg, this, SNE_GAIN, 25); break;
	}
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
//	inputMeter .setLevel(processor.getInputLevel());
///	outputMeter.setLevel(processor.getOutputLevel());
//	peakLed.setLevel(processor.getOutputLevel());

	repaint();
}
