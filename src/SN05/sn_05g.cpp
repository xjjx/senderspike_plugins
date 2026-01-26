//------------------------------------------------------------------------------------
//
//	file:		sn_05e.cpp
//
//	purpose:	SN05 limiter GUI
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <sn_05e.h>
#include <sn_05g.h>
#include <stdio.h>
#include <math.h>


//------------------------------------------------------------------------------------

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

void SignalNoiseLimiterGR::draw(CDrawContext* ctx)
{
	if(_map == 0)
		return;

	CPoint pt;
	CCoord w = size.right - size.left;
	CCoord h = size.bottom - size.top;

	//draw unlit
	pt.v = h;
	if(bTransparencyEnabled)
		_map->drawTransparent(ctx, size, pt);
	else
		_map->draw(ctx, size, pt);

	//draw value
	double yb = _val < 0.0 ? 0.0 : (_val > 20.0 ? 20.0 : _val);
	CCoord cc = w - (long)(((long)(100 * (yb / 20.f) + 0.5f) / 100.f) * w);
	pt(cc, 0);
	cc += size.left;
	CRect rc(size.left, size.top, size.right, size.bottom);
	rc.left = cc;

	if(bTransparencyEnabled)
		_map->drawTransparent(ctx, rc, pt);
	else
		_map->draw(ctx, rc, pt);

	setDirty(false);

	_val = _val - 0.25;
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGR::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGR::setVal(double dB)
{
	if(dB > _val)
		_val = dB;
	setDirty(true);
}

//------------------------------------------------------------------------------------
// GUI
//------------------------------------------------------------------------------------

SignalNoiseLimiterGUI::SignalNoiseLimiterGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_grHL = 0;
	_grHC = 0;
	_ceil = 0;
	_gain = 0;
	_hpfc = 0;
	_atkh = 0;
	_relh = 0;
	_rels = 0;
	_clip = 0;
	_txtg = 0;
	_txtc = 0;
	_mode = 0;
	_hpon = 0;
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

SignalNoiseLimiterGUI::~SignalNoiseLimiterGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

bool SignalNoiseLimiterGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	CCoord x, y;
	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(101);	// background
	CBitmap* knob1 = new CBitmap(102);	// large knob
	CBitmap* knob2 = new CBitmap(103);	// small knob
	CBitmap* meter = new CBitmap(104);	// GR meter
	CBitmap* onoff = new CBitmap(105);	// rocker switch
	CBitmap* slide = new CBitmap(106);	// led "button"

	setKnobMode(kLinearMode);

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	// knobs -----------------------------------------------

	x = 30;
	y = 35;
	rc(x, y, x + 80, y + 80);
	_gain = new SignalNoiseKnobP(rc, this, SNE_GAIN, 97, 80, knob1, pt);
	_gain->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_gain);

	x = 400;
	rc(x, y, x + 80, y + 80);
	_ceil = new SignalNoiseKnobP(rc, this, SNE_CEIL, 97, 80, knob1, pt);
	_ceil->setValue(effect->getParameter(SNE_CEIL));
	frm->addView(_ceil);

	x = 155;
	y = 20;
	rc(x, y, x + 40, y + 40);
	_hpfc = new SignalNoiseKnob(rc, this, SNE_HPFC, 97, 40, knob2, pt);
	_hpfc->setValue(effect->getParameter(SNE_HPFC));
	frm->addView(_hpfc);

	x = 235;
	rc(x, y, x + 40, y + 40);
	_atkh = new SignalNoiseKnob(rc, this, SNE_ATKH, 97, 40, knob2, pt);
	_atkh->setValue(effect->getParameter(SNE_ATKH));
	frm->addView(_atkh);

	x = 315;
	rc(x, y, x + 40, y + 40);
	_relh = new SignalNoiseKnob(rc, this, SNE_RELH, 97, 40, knob2, pt);
	_relh->setValue(effect->getParameter(SNE_RELH));
	frm->addView(_relh);

	x = 155;
	y = 93;
	rc(x, y, x + 40, y + 40);
	_clip = new SignalNoiseKnob(rc, this, SNE_CLIP, 97, 40, knob2, pt);
	_clip->setValue(effect->getParameter(SNE_CLIP));
	frm->addView(_clip);

	x = 315;
	rc(x, y, x + 40, y + 40);
	_rels = new SignalNoiseKnob(rc, this, SNE_RELS, 97, 40, knob2, pt);
	_rels->setValue(effect->getParameter(SNE_RELS));
	frm->addView(_rels);

	// buttons ---------------------------------------------

	rc(235, 94, 275, 134);
	_mode = new COnOffButton(rc, this, SNE_MODE, onoff);
	_mode->setValue(effect->getParameter(SNE_MODE));
	frm->addView(_mode);

	rc(122, 6, 150, 34);
	_hpon = new COnOffButton(rc, this, SNE_HPON, slide);
	_hpon->setValue(effect->getParameter(SNE_HPON));
	frm->addView(_hpon);

	// text edits ------------------------------------------

	x = 55;
	y = 135;
	_txtg = snCreateTextEdit(x, y, this, IDC_TX_GAIN);
	_txtg->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_txtg);

	x = 425;
	_txtc = snCreateTextEdit(x, y, this, IDC_TX_CEIL);
	_txtc->setValue(effect->getParameter(SNE_CEIL));
	frm->addView(_txtc);

	// GR meters -------------------------------------------

	rc(55, 179, 455, 189);
	_grHL = new SignalNoiseLimiterGR(rc, meter, effect->getSampleRate());
	frm->addView(_grHL);

	rc(55, 191, 455, 201);
	_grHC = new SignalNoiseLimiterGR(rc, meter, effect->getSampleRate());
	frm->addView(_grHC);

	// finalize --------------------------------------------

	_gain->setRangePixels(480);
	_ceil->setRangePixels(480);
	_gain->setRangeAbsolute(24);
	_ceil->setRangeAbsolute(24);

	_atkh->setRange(480);
	_relh->setRange(480);
	_rels->setRange(480);
	_hpfc->setRange(632);
	_clip->setRange(500);

	_gain->setWheelInc(0.125f);
	_ceil->setWheelInc(0.125f);
	_atkh->setWheelInc(0.125f);
	_relh->setWheelInc(0.125f);
	_rels->setWheelInc(0.125f);
	_hpfc->setWheelInc(0.125f);
	_clip->setWheelInc(0.125f);

	_gain->setDefaultValue(gParam[SNE_GAIN].val);
	_ceil->setDefaultValue(gParam[SNE_CEIL].val);
	_atkh->setDefaultValue(gParam[SNE_ATKH].val);
	_relh->setDefaultValue(gParam[SNE_RELH].val);
	_rels->setDefaultValue(gParam[SNE_RELS].val);
	_mode->setDefaultValue(gParam[SNE_MODE].val);
	_hpon->setDefaultValue(gParam[SNE_HPON].val);
	_hpfc->setDefaultValue(gParam[SNE_HPFC].val);
	_clip->setDefaultValue(gParam[SNE_CLIP].val);

	backg->forget();
	knob1->forget();
	knob2->forget();
	onoff->forget();
	slide->forget();

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseLimiterGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;
	_grHL = 0;
	_grHC = 0;
	_ceil = 0;
	_gain = 0;
	_hpfc = 0;
	_atkh = 0;
	_relh = 0;
	_rels = 0;
	_clip = 0;
	_txtg = 0;
	_txtc = 0;
	_mode = 0;
	_hpon = 0;
}

//------------------------------------------------------------------------------------

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

