//------------------------------------------------------------------------------------
//
//	file:		sn_03g.cpp
//
//	purpose:	SN03 tape recorder emulator GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <sn_03e.h>
#include <sn_03g.h>
#include <stdio.h>


//------------------------------------------------------------------------------------

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

//------------------------------------------------------------------------------------
// tape deck editor
//------------------------------------------------------------------------------------

SignalNoiseTapedeckGUI::SignalNoiseTapedeckGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_trim = 0;
	_gain = 0;
	_rclo = 0;
	_rchi = 0;
	_rplo = 0;
	_rphi = 0;
	_head = 0;
	_bump = 0;
	_hiss = 0;
	_mode = 0;
	_room = 0;
	_hold = 0;
	_path = 0;
	_nois = 0;
	_hbon = 0;
	_loon = 0;
	_attn = 0;
	_vumt = 0;
	_txti = 0;
	_txto = 0;
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

SignalNoiseTapedeckGUI::~SignalNoiseTapedeckGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

bool SignalNoiseTapedeckGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	CCoord x, y;
	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(101);	// background
	CBitmap* knob1 = new CBitmap(102);	// large knob
	CBitmap* knob2 = new CBitmap(103);	// small knob
	CBitmap* modes = new CBitmap(104);	// switch
	CBitmap* vublk = new CBitmap(105);	// VU needle
	CBitmap* vured = new CBitmap(106);	// peak needle
	CBitmap* peakl = new CBitmap(107);	// peak led
	CBitmap* displ = new CBitmap(108);	// headroom display
	CBitmap* slide = new CBitmap(109);	// on/off slider
	CBitmap* three = new CBitmap(110);	// 3-way switch
	CBitmap* buttn = new CBitmap(111);	// "A" button

	setKnobMode(kLinearMode);

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	// big knobs -------------------------------------------

	x = 30;
	y = 67;
	rc(x, y, x + SN03_KNOB1_W, y + SN03_KNOB1_W);
	_trim = new SignalNoiseKnobP(rc, this, SNE_TRIM, SN03_KNOB1_FRAMES, SN03_KNOB1_W, knob1, pt);
	_trim->setValue(effect->getParameter(SNE_TRIM));
	frm->addView(_trim);

	x = 410;
	rc(x, y, x + SN03_KNOB1_W, y + SN03_KNOB1_W);
	_gain = new SignalNoiseKnobP(rc, this, SNE_GAIN, SN03_KNOB1_FRAMES, SN03_KNOB1_W, knob1, pt);
	_gain->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_gain);

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

	// small knobs -----------------------------------------

	x = 30;
	y = 220;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_rclo = new SignalNoiseKnob(rc, this, SNE_RCLO, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_rclo->setValue(effect->getParameter(SNE_RCLO));
	frm->addView(_rclo);

	x = 140;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_rchi = new SignalNoiseKnob(rc, this, SNE_RCHI, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_rchi->setValue(effect->getParameter(SNE_RCHI));
	frm->addView(_rchi);

	x = 320;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_rplo = new SignalNoiseKnob(rc, this, SNE_RPLO, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_rplo->setValue(effect->getParameter(SNE_RPLO));
	frm->addView(_rplo);

	x = 430;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_rphi = new SignalNoiseKnob(rc, this, SNE_RPHI, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_rphi->setValue(effect->getParameter(SNE_RPHI));
	frm->addView(_rphi);

	x = 30;
	y = 330;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_head = new SignalNoiseKnob(rc, this, SNE_HEAD, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_head->setValue(effect->getParameter(SNE_HEAD));
	frm->addView(_head);

	x = 140;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_bump = new SignalNoiseKnob(rc, this, SNE_BUMP, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_bump->setValue(effect->getParameter(SNE_BUMP));
	frm->addView(_bump);

	x = 430;
	rc(x, y, x + SN03_KNOB2_W, y + SN03_KNOB2_W);
	_hiss = new SignalNoiseKnob(rc, this, SNE_HISS, SN03_KNOB2_FRAMES, SN03_KNOB2_W, knob2, pt);
	_hiss->setValue(effect->getParameter(SNE_HISS));
	frm->addView(_hiss);

	// switches --------------------------------------------

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

	// finalize --------------------------------------------

	_trim->setRangePixels(480);
	_gain->setRangePixels(480);
	_trim->setRangeAbsolute(48);
	_gain->setRangeAbsolute(48);
	_trim->setLinkInversed(_gain);

	_rclo->setRange(400);
	_rchi->setRange(560);
	_rplo->setRange(400);
	_rphi->setRange(560);
	_head->setRange(400);
	_bump->setRange(400);
	_hiss->setRange(450);

	_trim->setWheelInc(0.125f);
	_gain->setWheelInc(0.125f);
	_rclo->setWheelInc(0.125f);
	_rchi->setWheelInc(0.125f);
	_rplo->setWheelInc(0.125f);
	_rphi->setWheelInc(0.125f);
	_head->setWheelInc(0.125f);
	_bump->setWheelInc(0.125f);
	_hiss->setWheelInc(0.125f);

	_trim->setDefaultValue(gParam[SNE_TRIM].val);
	_gain->setDefaultValue(gParam[SNE_GAIN].val);
	_rclo->setDefaultValue(gParam[SNE_RCLO].val);
	_rchi->setDefaultValue(gParam[SNE_RCHI].val);
	_rplo->setDefaultValue(gParam[SNE_RPLO].val);
	_rphi->setDefaultValue(gParam[SNE_RPHI].val);
	_head->setDefaultValue(gParam[SNE_HEAD].val);
	_bump->setDefaultValue(gParam[SNE_BUMP].val);
	_hiss->setDefaultValue(gParam[SNE_HISS].val);
	_mode->setDefaultValue(gParam[SNE_EQSC].val);
	_room->setDefaultValue(gParam[SNE_ROOM].val);
	_hold->setDefaultValue(gParam[SNE_HOLD].val);
	_path->setDefaultValue(gParam[SNE_PATH].val);
	_attn->setDefaultValue(gParam[SNE_ATTN].val);
	_nois->setDefaultValue(gParam[SNE_NOIS].val);
	_hbon->setDefaultValue(gParam[SNE_HBON].val);

	if(effect->getParameter(SNE_HOLD) > 0.5)
		_vumt->setPeakBitmap(vured);

	float rm = effect->getParameter(SNE_ROOM);
	if(rm < .25f)		_vumt->setLevel(-12);
	else if(rm < .50f)	_vumt->setLevel(-14);
	else if(rm < .75f)	_vumt->setLevel(-18);
	else				_vumt->setLevel(-20);

	backg->forget();
	knob1->forget();
	knob2->forget();
	modes->forget();
	vublk->forget();
	vured->forget();
	peakl->forget();
	displ->forget();
	slide->forget();
	three->forget();
	buttn->forget();

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseTapedeckGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;
	_trim = 0;
	_gain = 0;
	_rclo = 0;
	_rchi = 0;
	_rplo = 0;
	_rphi = 0;
	_head = 0;
	_bump = 0;
	_hiss = 0;
	_mode = 0;
	_room = 0;
	_hold = 0;
	_path = 0;
	_nois = 0;
	_hbon = 0;
	_loon = 0;
	_attn = 0;
	_vumt = 0;
	_txti = 0;
	_txto = 0;
}

//------------------------------------------------------------------------------------

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
