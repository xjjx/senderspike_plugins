//------------------------------------------------------------------------------------
//
//	file:		sn_06e.cpp
//
//	purpose:	SN06 op-amp GUI
//
//  authors:	2020 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <sn_06e.h>
#include <sn_06g.h>
#include <stdio.h>
#include <math.h>


//------------------------------------------------------------------------------------

static void snFormatValueGain(float val, char* str)
{
	sprintf(str, "%2.2f", val * 32 - 8);
}

//------------------------------------------------------------------------------------

static void snFormatValueVolu(float val, char* str)
{
	sprintf(str, "%2.2f", val * 64 - 48);
}

//------------------------------------------------------------------------------------

static void snFormatValueTrim(float val, char* str)
{
	sprintf(str, "%2.2f", val * 40 - 20);
}

//------------------------------------------------------------------------------------
// helpers
//------------------------------------------------------------------------------------

static CTextEdit* snCreateTextEdit(CCoord x, CCoord y, CControlListener* cl, long id)
{
	CColor clr = {168, 168, 168, 255};
	CRect rc(x, y, x + SN06_TEXT_W, y + SN06_TEXT_H);
	if(id == IDC_TX_TRIM) rc(x, y, x + SN06_TEXT_W, y + 12);
	CTextEdit* tx = new CTextEdit(rc, cl, id);
	tx->setTransparency(true);
	tx->setFontColor(clr);
	switch(id)
	{
	case IDC_TX_GAIN:
		tx->setFont(kNormalFontSmall);
		tx->setStringConvert(snFormatValueGain);
		break;
	case IDC_TX_VOLU:
		tx->setFont(kNormalFontSmall);
		tx->setStringConvert(snFormatValueVolu);
		break;
	case IDC_TX_TRIM:
		tx->setFont(kNormalFontVerySmall);
		tx->setStringConvert(snFormatValueTrim);
		break;
	}
	
	return tx;
}

//------------------------------------------------------------------------------------
// GUI
//------------------------------------------------------------------------------------

SignalNoiseOpampGUI::SignalNoiseOpampGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_pkld = 0;
	_trim = 0;
	_gain = 0;
	_volu = 0;
	_txti = 0;
	_txtg = 0;
	_txtv = 0;
	_vupI = 0;
	_vupO = 0;
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

SignalNoiseOpampGUI::~SignalNoiseOpampGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

bool SignalNoiseOpampGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	CCoord x, y;
	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(101);	// background
	CBitmap* knob1 = new CBitmap(102);	// large knob
	CBitmap* knob2 = new CBitmap(103);	// screw knob
	CBitmap* pkled = new CBitmap(104);	// peak led
	CBitmap* vumap = new CBitmap(105);	// peak meter

	setKnobMode(kLinearMode);

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	// controls --------------------------------------------

	x = 44;
	y = 35;
	rc(x, y, x + 30, y + 30);
	_trim = new SignalNoiseKnobP(rc, this, SNE_TRIM, 97, 30, knob2, pt);
	_trim->setValue(effect->getParameter(SNE_TRIM));
	frm->addView(_trim);

	x = 80;
	y = 100;
	rc(x, y, x + 80, y + 80);
	_gain = new SignalNoiseKnobP(rc, this, SNE_GAIN, 97, 80, knob1, pt);
	_gain->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_gain);

	y = 230;
	rc(x, y, x + 80, y + 80);
	_volu = new SignalNoiseKnobP(rc, this, SNE_VOLU, 97, 80, knob1, pt);
	_volu->setValue(effect->getParameter(SNE_VOLU));
	frm->addView(_volu);

	// text edits ------------------------------------------

	x = 44;
	y = 74;
	_txti = snCreateTextEdit(x, y, this, IDC_TX_TRIM);
	_txti->setValue(effect->getParameter(SNE_TRIM));
	frm->addView(_txti);

	x = 30;
	y = 184;
	_txtg = snCreateTextEdit(x, y, this, IDC_TX_GAIN);
	_txtg->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_txtg);

	y = 314;
	_txtv = snCreateTextEdit(x, y, this, IDC_TX_VOLU);
	_txtv->setValue(effect->getParameter(SNE_VOLU));
	frm->addView(_txtv);

	// leds ------------------------------------------------

	x = 180;
	y = 307;
	rc(x, y, x + SN06_LED_W, y + SN06_LED_W);
	_pkld = new SignalNoisePeakLed(rc, pkled);
	frm->addView(_pkld);

	// peak meters -----------------------------------------

	x = 112;
	y = 42;
	rc(x, y, x + 100, y + 5);
	_vupI = new SignalNoisePeakMeter(rc, vumap, effect->getSampleRate());
	frm->addView(_vupI);

	y = 52;
	rc(x, y, x + 100, y + 5);
	_vupO = new SignalNoisePeakMeter(rc, vumap, effect->getSampleRate());
	frm->addView(_vupO);

	// finalize --------------------------------------------

	_trim->setWheelInc(0.125f);
	_trim->setRangeAbsolute(40);
	_trim->setLinkInversed(_volu);
	_trim->setDefaultValue(gParam[SNE_TRIM].val);

	_gain->setWheelInc(0.125f);
	_gain->setRangeAbsolute(32);
	_gain->setLinkInversed(_volu);
	_gain->setDefaultValue(gParam[SNE_GAIN].val);

	_volu->setWheelInc(0.125f);
	_volu->setRangeAbsolute(64);
	_volu->setDefaultValue(gParam[SNE_VOLU].val);

	backg->forget();
	knob1->forget();
	knob2->forget();
	pkled->forget();
	vumap->forget();

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseOpampGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;
	_pkld = 0;
	_trim = 0;
	_gain = 0;
	_volu = 0;
	_txti = 0;
	_txtg = 0;
	_txtv = 0;
	_vupI = 0;
	_vupO = 0;
}

//------------------------------------------------------------------------------------

void SignalNoiseOpampGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_GAIN:	if(_gain) _gain->setValue(effect->getParameter(at));
					if(_txtg) _txtg->setValue(effect->getParameter(at)); break;
	case SNE_TRIM:	if(_trim) _trim->setValue(effect->getParameter(at));
					if(_txti) _txti->setValue(effect->getParameter(at)); break;
	case SNE_VOLU:	if(_volu) _volu->setValue(effect->getParameter(at));
					if(_txtv) _txtv->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseOpampGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
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
	case SNE_TRIM:
	case SNE_VOLU:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	case IDC_TX_GAIN:
		_txtg->getText(t);
		v = (strtof(t, &c) + 8) / 32.f;
		effect->setParameter(SNE_GAIN, clampf(v));
		break;
	case IDC_TX_VOLU:
		_txtv->getText(t);
		v = (strtof(t, &c) + 48) / 64.f;
		effect->setParameter(SNE_VOLU, clampf(v));
		break;
	case IDC_TX_TRIM:
		_txti->getText(t);
		v = (strtof(t, &c) + 20) / 40.f;
		effect->setParameter(SNE_TRIM, clampf(v));
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseOpampGUI::trackMeters(double I, double O)
{
	if(_open == 0)
		return;
	_vupI->setVal(I);
	_vupO->setVal(O);
	_pkld->setAmp(O);
}

//------------------------------------------------------------------------------------
