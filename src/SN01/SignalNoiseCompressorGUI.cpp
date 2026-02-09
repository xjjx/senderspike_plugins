//------------------------------------------------------------------------------------
//
//	file:		sn_01g.cpp
//
//	purpose:	SN01 compressor GUI
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <sn_01e.h>
#include <sn_01g.h>


//------------------------------------------------------------------------------------

SignalNoiseCompressorGUI::SignalNoiseCompressorGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_thrs = 0;
	_func = 0;
	_gain = 0;
	_attk = 0;
	_rels = 0;
	_kwdt = 0;
	_kprc = 0;
	_comp = 0;
	_grdb = 0;
	_mode = 0;
	_push = 0;
	_fbck = 0;
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

SignalNoiseCompressorGUI::~SignalNoiseCompressorGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

bool SignalNoiseCompressorGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(101);	// background
	CBitmap* knob1 = new CBitmap(102);	// large knob
	CBitmap* knob2 = new CBitmap(103);	// small knob
	CBitmap* knob3 = new CBitmap(104);	// screw knob
	CBitmap* needl = new CBitmap(105);	// GR needle
	CBitmap* modes = new CBitmap(106);	// mode switch
	CBitmap* onoff = new CBitmap(107);	// on/off button

	setKnobMode(kLinearMode);

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	// big knobs -------------------------------------------

	CCoord x = SN01_KNOB1_X;
	rc(x, SN01_KNOB1_Y, x + SN01_KNOB1_W, SN01_KNOB1_H);
	_thrs = new CAnimKnob(rc, this, SNE_TRSH, SN01_KNOB1_FRAMES, SN01_KNOB1_W, knob1, pt);
	_thrs->setValue(effect->getParameter(SNE_TRSH));
	frm->addView(_thrs);

	x += SN01_KNOB_OFFSET;
	rc(x, SN01_KNOB1_Y, x + SN01_KNOB1_W, SN01_KNOB1_H);
	_func = new CAnimKnob(rc, this, SNE_FUNC, SN01_KNOB1_FRAMES, SN01_KNOB1_W, knob1, pt);
	_func->setValue(effect->getParameter(SNE_FUNC));
	frm->addView(_func);

	x += SN01_KNOB_OFFSET;
	rc(x, SN01_KNOB1_Y, x + SN01_KNOB1_W, SN01_KNOB1_H);
	_gain = new CAnimKnob(rc, this, SNE_GAIN, SN01_KNOB1_FRAMES, SN01_KNOB1_W, knob1, pt);
	_gain->setValue(effect->getParameter(SNE_GAIN));
	frm->addView(_gain);

	// small knobs -----------------------------------------

	x = SN01_KNOB2_X;
	rc(x, SN01_KNOB2_Y, x + SN01_KNOB2_W, SN01_KNOB2_H);
	_attk = new CAnimKnob(rc, this, SNE_ATTK, SN01_KNOB2_FRAMES, SN01_KNOB2_W, knob2, pt);
	_attk->setValue(effect->getParameter(SNE_ATTK));
	frm->addView(_attk);

	x += SN01_KNOB_OFFSET;
	rc(x, SN01_KNOB2_Y, x + SN01_KNOB2_W, SN01_KNOB2_H);
	_rels = new CAnimKnob(rc, this, SNE_RELS, SN01_KNOB2_FRAMES, SN01_KNOB2_W, knob2, pt);
	_rels->setValue(effect->getParameter(SNE_RELS));
	frm->addView(_rels);

	x += SN01_KNOB_OFFSET;
	rc(x, SN01_KNOB2_Y, x + SN01_KNOB2_W, SN01_KNOB2_H);
	_kwdt = new CAnimKnob(rc, this, SNE_KWDT, SN01_KNOB2_FRAMES, SN01_KNOB2_W, knob2, pt);
	_kwdt->setValue(effect->getParameter(SNE_KWDT));
	frm->addView(_kwdt);

	x += SN01_KNOB_OFFSET;
	rc(x, SN01_KNOB2_Y, x + SN01_KNOB2_W, SN01_KNOB2_H);
	_kprc = new CAnimKnob(rc, this, SNE_KNEE, SN01_KNOB2_FRAMES, SN01_KNOB2_W, knob2, pt);
	_kprc->setValue(effect->getParameter(SNE_KNEE));
	frm->addView(_kprc);

	// screw knob ------------------------------------------

	rc(640, 140, 680, 180);
	_comp = new CAnimKnob(rc, this, SNE_COMP, 97, 40, knob3, pt);
	_comp->setValue(effect->getParameter(SNE_COMP));
	frm->addView(_comp);

	// GR meter --------------------------------------------

	rc(SN01_NEEDL_X, SN01_NEEDL_Y, SN01_NEEDL_W, SN01_NEEDL_H);
	_grdb = new SignalNoiseGR(rc, needl, SN01_NEEDL_FRAMES, effect->getSampleRate());
	frm->addView(_grdb);

	// switches --------------------------------------------

	x = SN01_MODES_X;
	rc(x, SN01_MODES_Y, x + SN01_MODES_SZ, SN01_MODES_Y + SN01_MODES_SZ);
	_mode = new CHorizontalSwitch(rc, this, SNE_MODE, 3, SN01_MODES_SZ, 3, modes, pt);
	_mode->setValue(effect->getParameter(SNE_MODE));
	frm->addView(_mode);

	x += SN01_MODES_OFFSET;
	rc(x, SN01_MODES_Y, x + SN01_MODES_SZ, SN01_MODES_Y + SN01_MODES_SZ);
	_push = new CHorizontalSwitch(rc, this, SNE_PUSH, 3, SN01_MODES_SZ, 3, modes, pt);
	_push->setValue(effect->getParameter(SNE_PUSH));
	frm->addView(_push);

	// on/off buttons --------------------------------------

	rc(25, 90, 65, 130);
	_fbck = new COnOffButton(rc, this, SNE_FBCK, onoff);
	_fbck->setValue(effect->getParameter(SNE_FBCK));
	frm->addView(_fbck);

	// finalize --------------------------------------------

	_thrs->setWheelInc(0.125f);
	_func->setWheelInc(0.125f);
	_gain->setWheelInc(0.125f);
	_attk->setWheelInc(0.125f);
	_rels->setWheelInc(0.125f);
	_kwdt->setWheelInc(0.125f);
	_kprc->setWheelInc(0.125f);
	_comp->setWheelInc(0.125f);

	_thrs->setDefaultValue(gParam[SNE_TRSH].val);
	_func->setDefaultValue(gParam[SNE_FUNC].val);
	_gain->setDefaultValue(gParam[SNE_GAIN].val);
	_attk->setDefaultValue(gParam[SNE_ATTK].val);
	_rels->setDefaultValue(gParam[SNE_RELS].val);
	_kwdt->setDefaultValue(gParam[SNE_KWDT].val);
	_kprc->setDefaultValue(gParam[SNE_KNEE].val);
	_mode->setDefaultValue(gParam[SNE_MODE].val);
	_push->setDefaultValue(gParam[SNE_PUSH].val);
	_comp->setDefaultValue(gParam[SNE_COMP].val);

	backg->forget();
	knob1->forget();
	knob2->forget();
	knob3->forget();
	needl->forget();
	modes->forget();
	onoff->forget();

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;
	_thrs = 0;
	_func = 0;
	_gain = 0;
	_attk = 0;
	_rels = 0;
	_kwdt = 0;
	_kprc = 0;
	_comp = 0;
	_grdb = 0;
	_mode = 0;
	_push = 0;
	_fbck = 0;
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_TRSH: if(_thrs) _thrs->setValue(effect->getParameter(at)); break;
	case SNE_FUNC: if(_func) _func->setValue(effect->getParameter(at)); break;
	case SNE_GAIN: if(_gain) _gain->setValue(effect->getParameter(at)); break;
	case SNE_ATTK: if(_attk) _attk->setValue(effect->getParameter(at)); break;
	case SNE_RELS: if(_rels) _rels->setValue(effect->getParameter(at)); break;
	case SNE_KNEE: if(_kprc) _kprc->setValue(effect->getParameter(at)); break;
	case SNE_KWDT: if(_kwdt) _kwdt->setValue(effect->getParameter(at)); break;
	case SNE_COMP: if(_comp) _comp->setValue(effect->getParameter(at)); break;
	case SNE_MODE: if(_mode) _mode->setValue(effect->getParameter(at)); break;
	case SNE_PUSH: if(_push) _push->setValue(effect->getParameter(at)); break;
	case SNE_FBCK: if(_fbck) _fbck->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::valueChanged(CDrawContext* ctx, CControl* ctrl)
{
	if(_open == 0)
		return;

	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_TRSH:
	case SNE_FUNC:
	case SNE_GAIN:
	case SNE_ATTK:
	case SNE_RELS:
	case SNE_KNEE:
	case SNE_KWDT:
	case SNE_COMP:
	case SNE_MODE:
	case SNE_PUSH:
	case SNE_FBCK:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::trackMeter(double dB)
{
	if(_open == 0)
		return;
	_grdb->setVal(dB);
}

//------------------------------------------------------------------------------------

void SignalNoiseCompressorGUI::setupMeter(double fs)
{
	if(_open == 0)
		return;
	_grdb->setFilter(fs);
}

//------------------------------------------------------------------------------------
