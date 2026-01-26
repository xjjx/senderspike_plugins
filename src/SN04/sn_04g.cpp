//------------------------------------------------------------------------------------
//
//	file:		sn_04g.cpp
//
//	purpose:	SN04 Channel EQ GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <sn_04e.h>
#include <sn_04g.h>
#include <stdio.h>
#include <math.h>


//------------------------------------------------------------------------------------

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

void SignalNoiseEqualizerArcSwitch::draw(CDrawContext* ctx)
{
	// empty

	/* dbg draw
	CColor clr = {255};
	setDirty(false);
	ctx->setFillColor(clr);
	ctx->drawRect(size);
	//*/
}

//------------------------------------------------------------------------------------

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

/* dbg msg
	char str[32] = {0};
	sprintf(str, "%2.2f | %2.2f | %i", an, flt, res);
	MessageBox(NULL, str, "", 0);
*/
	if(res > 5)
		_eff->setParameter(SNE_LPAS, (res - 6) * 0.2f);
	else
		_eff->setParameter(SNE_HPAS, res * 0.2f);
}

//------------------------------------------------------------------------------------
// class SignalNoiseEqualizerGUI
//------------------------------------------------------------------------------------

SignalNoiseEqualizerGUI::SignalNoiseEqualizerGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_hf_f = 0;
	_hf_g = 0;
	_hf_w = 0;
	_hf_t = 0;
	_hf_m = 0;
	_hf_b = 0;
	_hfon = 0;

	_mf_f = 0;
	_mf_g = 0;
	_mf_w = 0;
	_mf_t = 0;
	_mf_b = 0;
	_mfon = 0;

	_lf_f = 0;
	_lf_g = 0;
	_lf_w = 0;
	_lf_t = 0;
	_lf_m = 0;
	_lf_b = 0;
	_lfon = 0;

	_lpas = 0;
	_hpas = 0;
	_loct = 0;
	_hoct = 0;
	_lpon = 0;
	_hpon = 0;
	_hp_b = 0;
	_lp_b = 0;

	_gain = 0;
	_iphs = 0;

	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_tomg = 0;

	_mojo = 0;

	_hsfc = 0;
	_msfc = 0;
	_lsfc = 0;
	_hplp = 0;

	_open = 0;

	//init the size of the plugin
	CBitmap* bk	= new CBitmap(101);
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = (short)bk->getWidth();
	rect.bottom = (short)bk->getHeight();
	bk->forget();
}

//------------------------------------------------------------------------------------

SignalNoiseEqualizerGUI::~SignalNoiseEqualizerGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

bool SignalNoiseEqualizerGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	CCoord x, y;
	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(101);	// background
	CBitmap* knob1 = new CBitmap(102);	// inner knob
	CBitmap* knob2 = new CBitmap(103);	// rim
	CBitmap* knob3 = new CBitmap(104);	// HPF
	CBitmap* knob4 = new CBitmap(105);	// LPF
	CBitmap* knob5 = new CBitmap(106);	// gain knob
	CBitmap* modes = new CBitmap(107);	// 2-way switch
	CBitmap* slope = new CBitmap(108);	// 4-way switch
	CBitmap* bells = new CBitmap(109);	// bell switch
	CBitmap* phase = new CBitmap(110);	// phase switch
	CBitmap* onled = new CBitmap(111);	// on/off led
	CBitmap* pkled = new CBitmap(112);	// peak led
	CBitmap* round = new CBitmap(113);	// round button

	setKnobMode(kLinearMode);

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	// number click switches -------------------------------

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

	// HSF knobs -------------------------------------------

	x = SN04_KNOB_COL_1;
	y = 40;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_hf_f = new SignalNoiseKnob(rc, this, SNE_HF_F, SN04_KNOB2_FRAMES, SN04_KNOB_W, knob2, pt);
	_hf_f->setValue(effect->getParameter(SNE_HF_F));
	frm->addView(_hf_f);

	x += 10;
	y += 10;
	rc(x, y, x + SN04_KNOB2_W, y + SN04_KNOB2_W);
	_hf_g = new SignalNoiseKnobP(rc, this, SNE_HF_G, SN04_KNOB1_FRAMES, SN04_KNOB2_W, knob1, pt);
	_hf_g->setValue(effect->getParameter(SNE_HF_G));
	frm->addView(_hf_g);

	snSetKnobInnerMouse(_hf_g, x, y);

	x += SN04_KNOB_COL_2;
	y -= 10;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_hf_w = new SignalNoiseKnobP(rc, this, SNE_HF_Q, SN04_KNOB1_FRAMES, SN04_KNOB_W, knob5, pt);
	_hf_w->setValue(effect->getParameter(SNE_HF_Q));
	frm->addView(_hf_w);

	// PKF knobs -------------------------------------------

	x = SN04_KNOB_COL_1;
	y = 180;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_mf_f = new SignalNoiseKnob(rc, this, SNE_MF_F, SN04_KNOB2_FRAMES, SN04_KNOB_W, knob2, pt);
	_mf_f->setValue(effect->getParameter(SNE_MF_F));
	frm->addView(_mf_f);

	x += 10;
	y += 10;
	rc(x, y, x + SN04_KNOB2_W, y + SN04_KNOB2_W);
	_mf_g = new SignalNoiseKnobP(rc, this, SNE_MF_G, SN04_KNOB1_FRAMES, SN04_KNOB2_W, knob1, pt);
	_mf_g->setValue(effect->getParameter(SNE_MF_G));
	frm->addView(_mf_g);

	snSetKnobInnerMouse(_mf_g, x, y);

	x += SN04_KNOB_COL_2;
	y -= 10;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_mf_w = new SignalNoiseKnobP(rc, this, SNE_MF_Q, SN04_KNOB1_FRAMES, SN04_KNOB_W, knob5, pt);
	_mf_w->setValue(effect->getParameter(SNE_MF_Q));
	frm->addView(_mf_w);

	// LSF knobs -------------------------------------------

	x = SN04_KNOB_COL_1;
	y = 320;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_lf_f = new SignalNoiseKnob(rc, this, SNE_LF_F, SN04_KNOB2_FRAMES, SN04_KNOB_W, knob2, pt);
	_lf_f->setValue(effect->getParameter(SNE_LF_F));
	frm->addView(_lf_f);

	x += 10;
	y += 10;
	rc(x, y, x + SN04_KNOB2_W, y + SN04_KNOB2_W);
	_lf_g = new SignalNoiseKnobP(rc, this, SNE_LF_G, SN04_KNOB1_FRAMES, SN04_KNOB2_W, knob1, pt);
	_lf_g->setValue(effect->getParameter(SNE_LF_G));
	frm->addView(_lf_g);

	snSetKnobInnerMouse(_lf_g, x, y);

	x += SN04_KNOB_COL_2;
	y -= 10;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_lf_w = new SignalNoiseKnobP(rc, this, SNE_LF_Q, SN04_KNOB1_FRAMES, SN04_KNOB_W, knob5, pt);
	_lf_w->setValue(effect->getParameter(SNE_LF_Q));
	frm->addView(_lf_w);

	// LPF/HPF knobs ---------------------------------------

	x = SN04_KNOB_COL_1;
	y = 500;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_hpas = new SignalNoiseKnob(rc, this, SNE_HPAS, SN04_KNOB3_FRAMES, SN04_KNOB_W, knob3, pt);
	_hpas->setValue(effect->getParameter(SNE_HPAS));
	frm->addView(_hpas);

	x += 10;
	y += 10;
	rc(x, y, x + SN04_KNOB2_W, y + SN04_KNOB2_W);
	_lpas = new SignalNoiseKnob(rc, this, SNE_LPAS, SN04_KNOB3_FRAMES, SN04_KNOB2_W, knob4, pt);
	_lpas->setValue(effect->getParameter(SNE_LPAS));
	frm->addView(_lpas);

	snSetKnobInnerMouse(_lpas, x, y);

	// volume knob -----------------------------------------

	x += SN04_KNOB_COL_2;
	y -= 10;
	rc(x, y, x + SN04_KNOB_W, y + SN04_KNOB_W);
	_gain = new SignalNoiseKnobP(rc, this, SNE_GAIN, SN04_KNOB1_FRAMES, SN04_KNOB_W, knob5, pt);
	_gain->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_gain);

	// switches --------------------------------------------

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

	x = 190;
	y = 581;
	rc(x, y, x + 40, y + 30);
	_iphs = new COnOffButton(rc, this, SNE_IPHS, phase);
	_iphs->setValue(effect->getParameter(SNE_IPHS));
	frm->addView(_iphs);

	x = 133;
	y = 603;
	rc(x, y, x + 40, y + 30);
	_mojo = new COnOffButton(rc, this, SNE_MOJO, round);
	_mojo->setValue(effect->getParameter(SNE_MOJO));
	frm->addView(_mojo);

	// leds & mutes ----------------------------------------

	x = 131;
	y = 80;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_hfon = new SignalNoiseOnOffLed(rc, onled);
	_hfon->setOn(effect->getParameter(SNE_HF_F) >= 0.142);
	frm->addView(_hfon);

	_hf_b = new COnOffButton(rc, this, SNE_HF_B, 0);
	_hf_b->setValue(effect->getParameter(SNE_HF_B));
	_hfon->setBlink(effect->getParameter(SNE_HF_B) >= 0.5);
	frm->addView(_hf_b);

	y = 220;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_mfon = new SignalNoiseOnOffLed(rc, onled);
	_mfon->setOn(effect->getParameter(SNE_MF_F) >= 0.142);
	frm->addView(_mfon);

	_mf_b = new COnOffButton(rc, this, SNE_MF_B, 0);
	_mf_b->setValue(effect->getParameter(SNE_MF_B));
	_mfon->setBlink(effect->getParameter(SNE_MF_B) >= 0.5);
	frm->addView(_mf_b);

	y = 360;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_lfon = new SignalNoiseOnOffLed(rc, onled);
	_lfon->setOn(effect->getParameter(SNE_LF_F) >= 0.142);
	frm->addView(_lfon);

	_lf_b = new COnOffButton(rc, this, SNE_LF_B, 0);
	_lf_b->setValue(effect->getParameter(SNE_LF_B));
	_lfon->setBlink(effect->getParameter(SNE_LF_B) >= 0.5);
	frm->addView(_lf_b);

	x = 12;
	y = 592;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_hpon = new SignalNoiseOnOffLed(rc, onled);
	_hpon->setOn(effect->getParameter(SNE_HPAS) >= 0.2);
	frm->addView(_hpon);

	_hp_b = new COnOffButton(rc, this, SNE_HP_B, 0);
	_hp_b->setValue(effect->getParameter(SNE_HP_B));
	_hpon->setBlink(effect->getParameter(SNE_HP_B) >= 0.5);
	frm->addView(_hp_b);

	x = 121;
	y = 460;
	rc(x, y, x + SN04_LED_W, y + SN04_LED_W);
	_lpon = new SignalNoiseOnOffLed(rc, onled);
	_lpon->setOn(effect->getParameter(SNE_LPAS) >= 0.2);
	frm->addView(_lpon);

	_lp_b = new COnOffButton(rc, this, SNE_LP_B, 0);
	_lp_b->setValue(effect->getParameter(SNE_LP_B));
	_lpon->setBlink(effect->getParameter(SNE_LP_B) >= 0.5);
	frm->addView(_lp_b);

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

	_lpas->setRange(300);
	_hpas->setRange(300);

	_hf_f->setRange(400);
	_mf_f->setRange(400);
	_lf_f->setRange(400);

	_hf_g->setRangeAbsolute(36);
	_mf_g->setRangeAbsolute(36);
	_lf_g->setRangeAbsolute(36);

	_hf_w->setRangeAbsolute(40);
	_mf_w->setRangeAbsolute(40);
	_lf_w->setRangeAbsolute(40);

	_gain->setRangeAbsolute(50);

	_hf_f->setDefaultValue(gParam[SNE_HF_F].val);
	_hf_g->setDefaultValue(gParam[SNE_HF_G].val);
	_hf_w->setDefaultValue(gParam[SNE_HF_Q].val);
	_hf_t->setDefaultValue(gParam[SNE_HF_T].val);
	_hf_m->setDefaultValue(gParam[SNE_HF_M].val);
	_hf_b->setDefaultValue(gParam[SNE_HF_B].val);

	_mf_f->setDefaultValue(gParam[SNE_MF_F].val);
	_mf_g->setDefaultValue(gParam[SNE_MF_G].val);
	_mf_w->setDefaultValue(gParam[SNE_MF_Q].val);
	_mf_t->setDefaultValue(gParam[SNE_MF_T].val);
	_mf_b->setDefaultValue(gParam[SNE_MF_B].val);

	_lf_f->setDefaultValue(gParam[SNE_LF_F].val);
	_lf_g->setDefaultValue(gParam[SNE_LF_G].val);
	_lf_w->setDefaultValue(gParam[SNE_LF_Q].val);
	_lf_t->setDefaultValue(gParam[SNE_LF_T].val);
	_lf_m->setDefaultValue(gParam[SNE_LF_M].val);
	_lf_b->setDefaultValue(gParam[SNE_LF_B].val);

	_lpas->setDefaultValue(gParam[SNE_LPAS].val);
	_hpas->setDefaultValue(gParam[SNE_HPAS].val);
	_loct->setDefaultValue(gParam[SNE_LOCT].val);
	_hoct->setDefaultValue(gParam[SNE_HOCT].val);
	_lp_b->setDefaultValue(gParam[SNE_LP_B].val);
	_hp_b->setDefaultValue(gParam[SNE_HP_B].val);

	_gain->setDefaultValue(gParam[SNE_GAIN].val);
	_iphs->setDefaultValue(gParam[SNE_IPHS].val);
	_mojo->setDefaultValue(gParam[SNE_MOJO].val);

	backg->forget();
	knob1->forget();
	knob2->forget();
	knob3->forget();
	knob4->forget();
	knob5->forget();
	modes->forget();
	slope->forget();
	bells->forget();
	phase->forget();
	onled->forget();
	pkled->forget();
	round->forget();

	_time = GetTickCount();
	_blnk = false;

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

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

void SignalNoiseEqualizerGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;

	_hf_f = 0;
	_hf_g = 0;
	_hf_w = 0;
	_hf_t = 0;
	_hf_m = 0;
	_hf_b = 0;
	_hfon = 0;

	_mf_f = 0;
	_mf_g = 0;
	_mf_w = 0;
	_mf_t = 0;
	_mf_b = 0;
	_mfon = 0;

	_lf_f = 0;
	_lf_g = 0;
	_lf_w = 0;
	_lf_t = 0;
	_lf_m = 0;
	_lf_b = 0;
	_lfon = 0;

	_lpas = 0;
	_hpas = 0;
	_loct = 0;
	_hoct = 0;
	_lpon = 0;
	_hpon = 0;
	_hp_b = 0;
	_lp_b = 0;

	_gain = 0;
	_iphs = 0;

	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_thfg = 0;
	_tomg = 0;

	_mojo = 0;

	_hsfc = 0;
	_msfc = 0;
	_lsfc = 0;
	_hplp = 0;
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

//------------------------------------------------------------------------------------
