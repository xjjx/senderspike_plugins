//------------------------------------------------------------------------------------
//
//	file:		sn_02g.cpp
//
//	purpose:	SN02 VU meter
//
//  authors:	2019 - 2026 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <sn_02g.h>


//------------------------------------------------------------------------------------
// factory
//------------------------------------------------------------------------------------

AudioEffect* createEffectInstance(audioMasterCallback cb)
{
	return new SignalNoiseVUMeter(cb);
}

//------------------------------------------------------------------------------------
// class SignalNoiseCompressor
//------------------------------------------------------------------------------------

SignalNoiseVUMeter::SignalNoiseVUMeter(audioMasterCallback cb) : SignalNoiseFX(cb, 0, SNE_SIZE)
{
	memset(_name, 0, sizeof(char) * 256);

	InitParams(gParam, SNE_SIZE);
	
	setUniqueID(SN02_UID);
	setNumInputs(2);
	setNumOutputs(2);
	canProcessReplacing(true);
	canDoubleReplacing(true);
	programsAreChunks(true);

	editor = new SignalNoiseVUMeterGUI(this);
}

//------------------------------------------------------------------------------------

SignalNoiseVUMeter::~SignalNoiseVUMeter()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeter::onSetSampleRate(float fs)
{
	((SignalNoiseVUMeterGUI*)editor)->setupMeterFilter(fs);
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeter::onSetParameter(VstInt32 at, float v)
{
	switch(at)
	{
	case SNE_ROOM:
		if(v < .25f)		((SignalNoiseVUMeterGUI*)editor)->setupMeterLevel(-12);
		else if(v < .5f)	((SignalNoiseVUMeterGUI*)editor)->setupMeterLevel(-14);
		else if(v < .75f)	((SignalNoiseVUMeterGUI*)editor)->setupMeterLevel(-18);
		else				((SignalNoiseVUMeterGUI*)editor)->setupMeterLevel(-20);
		break;
	case SNE_HOLD: ((SignalNoiseVUMeterGUI*)editor)->setupMeterUseHold(v > 0.5);		break;
	case SNE_XTND: ((SignalNoiseVUMeterGUI*)editor)->toggleUserInterface();			break;
	}
}

//------------------------------------------------------------------------------------
// save & load
//------------------------------------------------------------------------------------

struct chunk_t
{
	float sz;
	float p1;
	float p2;
	float p3;
	float p4;
};

//------------------------------------------------------------------------------------

VstInt32 SignalNoiseVUMeter::getChunk(void** data, bool preset)
{
	size_t ln = strlen(_name) + 1;
	if(ln > 1)
	{
		size_t sz = sizeof(chunk_t);
		chunk_t* ch = (chunk_t*)malloc(sz + ln);
		ch->sz = float(ln);
		ch->p1 = _param[SNE_ROOM].val;
		ch->p2 = _param[SNE_MODE].val;
		ch->p3 = _param[SNE_HOLD].val;
		ch->p4 = _param[SNE_XTND].val;
		memcpy(((char*)(ch+1)), _name, sizeof(char) * ln);
		*data = ch;
		return VstInt32(sz + ln);
	}
	else
	{
		return SignalNoiseFX::getChunk(data, preset);
	}
	return 0;
}

//------------------------------------------------------------------------------------

VstInt32 SignalNoiseVUMeter::setChunk(void* data, VstInt32 sz, bool preset)
{
	float f = ((float*)data)[0];
	if(f > 1.f)
	{
		memset(_name, 0, 256);
		chunk_t* ch = (chunk_t*)data;
		_param[SNE_ROOM].val = clampf(ch->p1);
		_param[SNE_MODE].val = clampf(ch->p2);
		_param[SNE_HOLD].val = clampf(ch->p3);
		_param[SNE_XTND].val = clampf(ch->p4);
		memcpy(_name, ((char*)(ch+1)), size_t(ch->sz));
		onSetParameter(SNE_ROOM, _param[SNE_ROOM].val);
		onSetParameter(SNE_MODE, _param[SNE_MODE].val);
		onSetParameter(SNE_HOLD, _param[SNE_HOLD].val);
		onSetParameter(SNE_XTND, _param[SNE_XTND].val);
		if(editor)
		{
			((AEffGUIEditor*)editor)->setParameter(SNE_ROOM, _param[SNE_ROOM].val);
			((AEffGUIEditor*)editor)->setParameter(SNE_MODE, _param[SNE_MODE].val);
			((AEffGUIEditor*)editor)->setParameter(SNE_HOLD, _param[SNE_HOLD].val);
			((AEffGUIEditor*)editor)->setParameter(SNE_XTND, _param[SNE_XTND].val);
			((SignalNoiseVUMeterGUI*)editor)->setInstName(_name);
		}
	}
	else
	{
		SignalNoiseFX::setChunk(data, sz, preset);
	}
	return 0;
}

//------------------------------------------------------------------------------------
// DSP
//------------------------------------------------------------------------------------

static int GetSwitch(const float& v)
{
	static const float sv = 1.f / 3.f;

	if(v < sv) return 0;
	if(v > 1 - sv) return 2;
	return 1;
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeter::processReplacing(float** in, float** out, VstInt32 sz)
{
	float* inL = in[0];
	float* inR = in[1];
	float* outL = out[0];
	float* outR = out[1];

	double L, R, M;
	int mode = GetSwitch(_param[SNE_MODE].val);

	while(--sz >= 0)
	{
		L = *inL++;
		R = *inR++;

		(*outL++) = float(L);
		(*outR++) = float(R);

		if(_mono)
		{
			L = fabs(L);
			((SignalNoiseVUMeterGUI*)editor)->trackMeter(L, L);
		}
		else
		{
			L = fabs(L);
			R = fabs(R);

			if(mode == 0)
			{
				L = (L + R) * 0.5;
				R = L;
			}
			else if(mode == 2)
			{
				M = (L + R) * 0.5;
				R = fabs(L - R) * 0.5;
				L = M;
			}
			((SignalNoiseVUMeterGUI*)editor)->trackMeter(L, R);
		}
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeter::processDoubleReplacing(double** in, double** out, VstInt32 sz)
{
	double* inL = in[0];
	double* inR = in[1];
	double* outL = out[0];
	double* outR = out[1];

	double L, R, M;
	int mode = GetSwitch(_param[SNE_MODE].val);

	while(--sz >= 0)
	{
		L = *inL++;
		R = *inR++;

		(*outL++) = L;
		(*outR++) = R;

		if(_mono)
		{
			L = fabs(L);
			((SignalNoiseVUMeterGUI*)editor)->trackMeter(L, L);
		}
		else
		{
			L = fabs(L);
			R = fabs(R);

			if(mode == 0)
			{
				L = (L + R) * 0.5;
				R = L;
			}
			else if(mode == 2)
			{
				M = (L + R) * 0.5;
				R = fabs(L - R) * 0.5;
				L = M;
			}
			((SignalNoiseVUMeterGUI*)editor)->trackMeter(L, R);
		}
	}
}

//------------------------------------------------------------------------------------
// class SignalNoiseVUMeterGUI
//------------------------------------------------------------------------------------

SignalNoiseVUMeterGUI::SignalNoiseVUMeterGUI(AudioEffect* fx) : AEffGUIEditor(fx) 
{
	_vucL = 0;
	_vucR = 0;
	_room = 0;
	_mode = 0;
	_hold = 0;
	_xtnd = 0;
	_name = 0;
	_open = 0;

	//init the size of the plugin
	rect.left   = 0;
	rect.top    = 0;
	rect.right  = SN02_Ws;
	rect.bottom = SN02_Hs;
}

//------------------------------------------------------------------------------------

SignalNoiseVUMeterGUI::~SignalNoiseVUMeterGUI()
{
	// empty
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::toggleUserInterface()
{
	if(_open == 0)
		return;

	// initialize ------------------------------------------

	bool xl = effect->getParameter(SNE_XTND) > 0.5;
	long id = xl ? 200 : 100;

	CBitmap* backg = new CBitmap(id+1);	// background
	CBitmap* vustd = new CBitmap(id+2);	// std needle
	CBitmap* vured = new CBitmap(id+3);	// red needle
	CBitmap* strip = new CBitmap(id+8);	// tape strip
	CBitmap* pkled = new CBitmap(id+9);	// peak led

	// frame -----------------------------------------------

	snResizeEditor(this, backg->getWidth(), backg->getHeight());
	frame->setBackground(backg);

	// VU meter --------------------------------------------

	CCoord q = xl ? 0 : 7;					//led offset y
	CCoord r = xl ? 2 : 1;					//led offset x
	CCoord s = pkled->getHeight() / 2;		//single frame height

	CCoord w, h, y, x = 30;
	if(xl)	{ y = SN02_METER_Yx; w = SN02_METER_Wx; h = SN02_METER_Hx;	}
	else	{ y = SN02_METER_Ys; w = SN02_METER_Ws; h = SN02_METER_Hs;	}
	snResizeControl(_vucL, x, y, w, h);
	_vucL->setBitmap(vustd);

	snResizeControl(_ledL, x + w - pkled->getWidth() + r, y + q, pkled->getWidth(), s);
	_ledL->setBackground(pkled);

	y += xl ? SN02_METER_OffX : SN02_METER_OffS;
	snResizeControl(_vucR, x, y, w, h);
	_vucR->setBitmap(vustd);

	snResizeControl(_ledR, x + w - pkled->getWidth() + r, y + q, pkled->getWidth(), s);
	_ledR->setBackground(pkled);

	if(effect->getParameter(SNE_HOLD) > 0.5)
	{
		_vucL->setPeakBitmap(vured);
		_vucR->setPeakBitmap(vured);
	}

	// switches --------------------------------------------

	x = backg->getWidth() - SN02_HOLD_X;
	snResizeControl(_hold, x, SN02_HOLD_Y, SN02_HOLD_W, SN02_HOLD_H);

	x = backg->getWidth() - SN02_XTND_X;
	y = backg->getHeight() - SN02_XTND_H;
	snResizeControl(_xtnd, x, y, SN02_XTND_W, SN02_XTND_H);

	// name ------------------------------------------------

	x = 20;
	if(xl)	{ y = 195; w = SN02_TAPE_Wx; h = SN02_TAPE_Hx; }
	else	{ y = 153; w = SN02_TAPE_Ws; h = SN02_TAPE_Hs; }
	snResizeControl(_name, x, y, w, h);
	_name->loadFontCustom(IDR_FONT_NAME, xl ? 23 : 16, 0);
	_name->setBackground(strip);

	// finalize --------------------------------------------

	backg->forget();
	vustd->forget();
	vured->forget();
	strip->forget();
	pkled->forget();
}

//------------------------------------------------------------------------------------

static void limitStringLength(char* in, char* out)
{
	strncpy(out, in, 30);
}

//------------------------------------------------------------------------------------

bool SignalNoiseVUMeterGUI::open(void *ptr)
{
	// always call this first !!!
	AEffGUIEditor::open(ptr);

	// initialize ------------------------------------------

	dword vut[] = {4, 21, 32, 42, 56, 65, 74, 84, 97, 109, 123, 129}; //change with X02 & X03 !!!

	bool xl = effect->getParameter(SNE_XTND) > 0.5;
	long id = xl ? 200 : 100;

	CPoint pt(0, 0);
	CBitmap* backg = new CBitmap(id+1);	// background
	CBitmap* vustd = new CBitmap(id+2);	// std needle
	CBitmap* vured = new CBitmap(id+3);	// red needle
	CBitmap* mode1 = new CBitmap(104);	// headroom
	CBitmap* mode2 = new CBitmap(105);	// mode
	CBitmap* mode3 = new CBitmap(106);	// hold
	CBitmap* dummy = new CBitmap(107);	// GUI switch
	CBitmap* strip = new CBitmap(id+8);	// tape strip
	CBitmap* pkled = new CBitmap(id+9);	// peak led

	// frame -----------------------------------------------

	CRect rc(0, 0, backg->getWidth(), backg->getHeight());
	CFrame* frm = new CFrame(rc, ptr, this);
	frm->setBackground(backg);

	rect.right  = (short)backg->getWidth();
	rect.bottom = (short)backg->getHeight();

	// VU meter --------------------------------------------

	CCoord q = xl ? 0 : 7;					//led offset y
	CCoord r = xl ? 2 : 1;					//led offset x
	CCoord s = pkled->getHeight() / 2;		//single frame height

	CCoord w, h, y, x = 30;
	if(xl)	{ y = SN02_METER_Yx; w = SN02_METER_Wx; h = SN02_METER_Hx;	}
	else	{ y = SN02_METER_Ys; w = SN02_METER_Ws; h = SN02_METER_Hs;	}

	rc(x, y, x + w, y + h);
	_vucL = new SignalNoiseVU(rc, vustd, vut, effect->getSampleRate());
	frm->addView(_vucL);
	
	rc(x + w - pkled->getWidth() + r, y + q, x + w, y + q + s);
	_ledL = new SignalNoisePeakLed(rc, pkled);
	frm->addView(_ledL);

	y += xl ? SN02_METER_OffX : SN02_METER_OffS;
	rc(x, y, x + w, y + h);
	_vucR = new SignalNoiseVU(rc, vustd, vut, effect->getSampleRate());
	frm->addView(_vucR);

	rc(x + w - pkled->getWidth() + r, y + q, x + w, y + q + s);
	_ledR = new SignalNoisePeakLed(rc, pkled);
	frm->addView(_ledR);

	// switches --------------------------------------------

	x = 26;
	rc(x, SN02_SWITCH_Y, x + 80, SN02_SWITCH_H + SN02_SWITCH_Y);
	_room = new CHorizontalSwitch(rc, this, SNE_ROOM, 4, SN02_SWITCH_H, 4, mode1, pt);
	frm->addView(_room);

	x = 143;
	rc(x, SN02_SWITCH_Y, x + 60, SN02_SWITCH_H + SN02_SWITCH_Y);
	_mode = new CHorizontalSwitch(rc, this, SNE_MODE, 3, SN02_SWITCH_H, 3, mode2, pt);
	frm->addView(_mode);

	x = backg->getWidth() - SN02_HOLD_X;
	rc(x, SN02_HOLD_Y, x + SN02_HOLD_W, SN02_HOLD_H + SN02_HOLD_Y);
	_hold = new COnOffButton(rc, this, SNE_HOLD, mode3);
	frm->addView(_hold);

	x = backg->getWidth() - SN02_XTND_X;
	y = backg->getHeight() - SN02_XTND_H;
	rc(x, y, x + SN02_XTND_W, y + SN02_XTND_H);
	_xtnd = new COnOffButton(rc, this, SNE_XTND, dummy);
	frm->addView(_xtnd);

	// name ------------------------------------------------

	CColor clr = {0};
	if(xl)	rc(20, 195, 20 + SN02_TAPE_Wx, 195 + SN02_TAPE_Hx);
	else	rc(20, 153, 20 + SN02_TAPE_Ws, 153 + SN02_TAPE_Hs);
	_name = new SignalNoiseUserLabel(rc, this, IDC_NAME);
	_name->loadFontCustom(IDR_FONT_NAME, xl ? 23 : 16, 0);
	_name->setTextEditConvert(limitStringLength);
	_name->setBackground(strip);
	_name->setFontColor(clr);
	frm->addView(_name);

	// finalize --------------------------------------------

	_room->setValue(effect->getParameter(SNE_ROOM));
	_mode->setValue(effect->getParameter(SNE_MODE));
	_hold->setValue(effect->getParameter(SNE_HOLD));
	_xtnd->setValue(effect->getParameter(SNE_XTND));
	_room->setDefaultValue(gParam[SNE_ROOM].val);
	_mode->setDefaultValue(gParam[SNE_MODE].val);
	_hold->setDefaultValue(gParam[SNE_HOLD].val);
	_xtnd->setDefaultValue(gParam[SNE_XTND].val);

	if(effect->getParameter(SNE_HOLD) > 0.5)
	{
		_vucL->setPeakBitmap(vured);
		_vucR->setPeakBitmap(vured);
	}

	float rm = effect->getParameter(SNE_ROOM);
	if(rm < .25f)
	{
		_vucL->setLevel(-12);
		_vucR->setLevel(-12);
	}
	else if(rm < .50f)
	{
		_vucL->setLevel(-14);
		_vucR->setLevel(-14);
	}
	else if(rm < .75f)
	{
		_vucL->setLevel(-18);
		_vucR->setLevel(-18);
	}
	else
	{
		_vucL->setLevel(-20);
		_vucR->setLevel(-20);
	}

	char* txt = ((SignalNoiseVUMeter*)effect)->getInstName();
	_name->setText(txt);
	_name->setDrawBackground(txt && strlen(txt));

	backg->forget();
	vustd->forget();
	vured->forget();
	mode1->forget();
	mode2->forget();
	mode3->forget();
	dummy->forget();
	strip->forget();
	pkled->forget();

	frame = frm;

	_open = 1;

	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::close()
{
	_open = 0;

	delete frame; // deletes all attached views
	frame = 0;
	_vucL = 0;
	_vucR = 0;
	_room = 0;
	_mode = 0;
	_hold = 0;
	_xtnd = 0;
	_name = 0;
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::setParameter(VstInt32 at, float v)
{
	if(_open == 0)
		return;

	//called from fx edit
	switch(at)
	{
	case SNE_ROOM: if(_room) _room->setValue(effect->getParameter(at)); break;
	case SNE_MODE: if(_mode) _mode->setValue(effect->getParameter(at)); break;
	case SNE_HOLD: if(_hold) _hold->setValue(effect->getParameter(at)); break;
	case SNE_XTND: if(_xtnd) _xtnd->setValue(effect->getParameter(at)); break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::valueChanged(CControl* ctrl)
{
	if(_open == 0)
		return;

	char* txt = 0;
	long tag = ctrl->getTag();
	switch(tag)
	{
	case SNE_ROOM:
	case SNE_MODE:
	case SNE_HOLD:
	case SNE_XTND:
		effect->setParameterAutomated(tag, ctrl->getValue());
		ctrl->setDirty();
		break;
	case IDC_NAME:
		txt = ((SignalNoiseVUMeter*)effect)->getInstName();
		_name->getText(txt);
		_name->setDrawBackground(txt && strlen(txt));
		break;
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::trackMeter(double L, double R)
{
	if(_open == 0)
		return;
	_vucL->setAmp(L);
	_vucR->setAmp(R);
	_ledL->setAmp(L);
	_ledR->setAmp(R);
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::setupMeterLevel(double nl)
{
	if(_open == 0)
		return;
	_vucL->setLevel(nl);
	_vucR->setLevel(nl);
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::setupMeterFilter(double fs)
{
	if(_open == 0)
		return;
	_vucL->setFilter(fs);
	_vucR->setFilter(fs);
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::setupMeterUseHold(bool on)
{
	if(_open == 0)
		return;
	
	long id = effect->getParameter(SNE_XTND) > 0.5 ? 200 : 100;
	CBitmap* map = new CBitmap(id+3);

	_vucL->setPeakBitmap(on ? map : 0);
	_vucR->setPeakBitmap(on ? map : 0);

	map->forget();
}

//------------------------------------------------------------------------------------

void SignalNoiseVUMeterGUI::setInstName(char* txt)
{
	if(_open == 0)
		return;
	_name->setText(txt);
	_name->setDrawBackground(txt && strlen(txt));
}

//------------------------------------------------------------------------------------
