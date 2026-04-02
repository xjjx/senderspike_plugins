//------------------------------------------------------------------------------------
//
//	file:		sn_ctrl.cpp
//
//	purpose:	VST custom GUI controls
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#include <math.h>
#include <stdio.h>
#include <sn_ctrl.h>


//------------------------------------------------------------------------------------
// GUI helpers
//------------------------------------------------------------------------------------

void snResizeEditor(AEffGUIEditor* edit, CCoord w, CCoord h)
{
	ERect* rc = 0;
	edit->getRect(&rc);
	edit->getFrame()->setSize(w, h);
	rc->right = (short)w;
	rc->bottom = (short)h;
}

//------------------------------------------------------------------------------------

void snResizeControl(CControl* ctrl, CCoord x, CCoord y, CCoord w, CCoord h)
{
	CRect rc(x, y, x+w, y+h);

	ctrl->setViewSize(rc);
	ctrl->setMouseableArea(rc);
}

//------------------------------------------------------------------------------------
// GR meter
//------------------------------------------------------------------------------------

SignalNoiseGR::SignalNoiseGR(const CRect& rc, CBitmap* map, dword num, float fs) : CControl(rc, 0, 0)
{
	setFilter(fs);

	_map = map;
	_num = num;

	if(_map)
		_map->remember();
}

//------------------------------------------------------------------------------------

SignalNoiseGR::~SignalNoiseGR()
{
	if(_map)
		_map->forget();
}

//------------------------------------------------------------------------------------

void SignalNoiseGR::draw(CDrawContext* ctx)
{
	if(_map == 0)
		return;

	CPoint pt;
	double yb = _val < 0.0 ? 0.0 : (_val > 20.0 ? 20.0 : _val);
	pt.v = (size.bottom - size.top) * dword((yb / 20.0) * (_num - 1));

	if(bTransparencyEnabled)
		_map->drawTransparent(ctx, size, pt);
	else
		_map->draw(ctx, size, pt);

	setDirty(false);
}

//------------------------------------------------------------------------------------

void SignalNoiseGR::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoiseGR::setVal(double dB)
{
	if(dB > _val)
		_val = _val * _env + dB * (1.0 - _env);
	else
		_val = dB;
	setDirty(true);
}

//------------------------------------------------------------------------------------

void SignalNoiseGR::setFilter(double fs)
{
	_env = exp(-2000.0 / (300.0 * fs));
	_val = DC_OFFSET;
}

//------------------------------------------------------------------------------------
// VU meter
//------------------------------------------------------------------------------------

const double SignalNoiseVU::_wn = 13.5119;
const double SignalNoiseVU::_et = 0.8127;
const double SignalNoiseVU::_sc = M_PI / 2.1908902300206644538278791312032;

static const double gTicks[] = {-20, -10, -7, -5, -3, -2, -1, 0, 1, 2, 3, 3.5};

//------------------------------------------------------------------------------------

SignalNoiseVU::SignalNoiseVU(const CRect& rc, CBitmap* map, dword* vut, float hz) : CControl(rc, 0, 0)
{
	setFilter(hz);
	setFrames(vut);
	setLevel(VU_DEFAULT_NOMINAL_LEVEL);

	_pkm = 0;
	_hms = 1000;
	_hst = GetTickCount();
	_hfn = 0;
	_map = map;

	if(_map)
		_map->remember();
}

//------------------------------------------------------------------------------------

SignalNoiseVU::~SignalNoiseVU()
{
	if(_map) _map->forget();
	if(_pkm) _pkm->forget();
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::draw(CDrawContext* ctx)
{
	if(_map == 0)
		return;

	//finalize VU ------------------------------------

	double v = _y0 * _sc;
	if(v < 0) v = 0;

	//calculate frame --------------------------------

	dword f = 0;
	if(v < _frm[0].amp)
	{
		v = v / _frm[0].amp;
		f = dword(v * _frm[0].frm);
	}
	else
	{
		bool b = false;
		vutick_t *ct, *pv;

		for(int i = 1; i < VU_MAX_TICKS; i++)
		{
			ct = &_frm[i];
			pv = &_frm[i-1];

			if(v < ct->amp)
			{
				b = true;
				v = (v - pv->amp) / (ct->amp - pv->amp);
				f = dword(v * (ct->frm - pv->frm) + pv->frm);
				break;
			}
		}
		if(!b) f = _frm[VU_MAX_TICKS-1].frm;
	}
	
	//draw -------------------------------------------

	CPoint pt;
	dword ct = GetTickCount();

	if(f > _hfn || ct - _hst >= _hms)
	{
		_hst = ct;
		_hfn = f;
	}
	else if(_pkm && _hfn - f > 1) // draw only if needles don't overlap
	{
		pt.v = (size.bottom - size.top) * _hfn;

		if(bTransparencyEnabled)
			_pkm->drawTransparent(ctx, size, pt);
		else
			_pkm->draw(ctx, size, pt);
	}

	pt.v = (size.bottom - size.top) * f;

	if(bTransparencyEnabled)
		_map->drawTransparent(ctx, size, pt);
	else
		_map->draw(ctx, size, pt);

	setDirty(false);
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setAmp(double A)
{
	//no need to store x0 & y0 !!!
	_x2 = _x1;
	_x1 = _x0;
	_x0 = A;
	_y2 = _y1;
	_y1 = _y0;
	_y0 = _b0 * _x0 + _b1 * _x1 + _b2 * _x2 - _a1 * _y1 - _a2 * _y2;

	setDirty(true);
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setLevel(double nl)
{
	for(int i = 0; i < VU_MAX_TICKS; i++)
		_frm[i].amp = dB2lin(gTicks[i] + nl - (1 / 1.2));
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setFrames(dword* frm)
{
	for(int i = 0; i < VU_MAX_TICKS; i++)
		_frm[i].frm = frm[i];
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setFilter(double hz)
{
	_x0 = _x1 = _x2 = _y0 = _y1 = _y2 = 0;

	double Td = 1 / hz;
	double Td2 = Td * Td;
	double wn2 = _wn * _wn;

	_a0 = 4 + 4 * _et * _wn * Td + wn2 * Td2;
	_a1 = -8 + 2 * wn2 * Td2;
	_a2 = 4 - 4 * _et * _wn * Td + wn2 * Td2;
	_b0 = Td2 * wn2;
	_b1 = 2 * _b0;
	_b2 = _b0;

	_a1 /= _a0;
	_a2 /= _a0;
	_b0 /= _a0;
	_b1 /= _a0;
	_b2 /= _a0;
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setHoldTime(dword ms)
{
	_hms = ms;
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setBitmap(CBitmap* map)
{
	if(_map) _map->forget();
	_map = map;
	if(_map) _map->remember();
}

//------------------------------------------------------------------------------------

void SignalNoiseVU::setPeakBitmap(CBitmap* map)
{
	if(_pkm) _pkm->forget();
	_pkm = map;
	if(_pkm) _pkm->remember();
}

//------------------------------------------------------------------------------------
// sample peak meter
//------------------------------------------------------------------------------------

SignalNoisePeakMeter::SignalNoisePeakMeter(const CRect& rc, CBitmap* map, float fs) : CControl(rc, 0, 0)
{
	_val = 0;
	_rng = 72;
	_map = map;

	if(_map)
		_map->remember();
}

//------------------------------------------------------------------------------------

SignalNoisePeakMeter::~SignalNoisePeakMeter()
{
	if(_map)
		_map->forget();
}

//------------------------------------------------------------------------------------

void SignalNoisePeakMeter::draw(CDrawContext* ctx)
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
	pt.v = 0;
	double dB = juce::Decibels::gainToDecibels(_val);
	dB = ((dB < -_rng ? -_rng : dB) + _rng) / _rng;
	CRect rc(size.left, size.top, size.right, size.bottom);
	CCoord cc = CCoord(w * dB);
	rc.right = rc.left + cc;

	if(bTransparencyEnabled)
		_map->drawTransparent(ctx, rc, pt);
	else
		_map->draw(ctx, rc, pt);

	setDirty(false);

	_val = _val - 0.01;
}

//------------------------------------------------------------------------------------

void SignalNoisePeakMeter::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoisePeakMeter::setVal(double dB)
{
	dB = clampd(dB);
	if(dB > _val)
		_val = dB;
	setDirty(true);
}

//------------------------------------------------------------------------------------

void SignalNoisePeakMeter::setRange(double dB)
{
	_rng = dB;
}

//------------------------------------------------------------------------------------
// sample peak led
//------------------------------------------------------------------------------------

SignalNoisePeakLed::SignalNoisePeakLed(const CRect& rc, CBitmap* map) : CControl(rc, 0, 0)
{
	_amp = 0;
	_hms = 300;
	_hst = GetTickCount();
	_pks = false;

	setBackground(map);
}

//------------------------------------------------------------------------------------

SignalNoisePeakLed::~SignalNoisePeakLed()
{
	//empty
}

//------------------------------------------------------------------------------------

void SignalNoisePeakLed::draw(CDrawContext* ctx)
{
	if(!pBackground)
		return;

	CPoint pt;
	dword ct = GetTickCount();
	
	if(_amp > 1.0)
	{
		_hst = ct;
		_pks = true;
		pt.v = size.bottom - size.top;
	}
	else
	{
		if(_pks && ct - _hst < _hms)
		{
			pt.v = size.bottom - size.top;
		}
		else
		{
			_hst = ct;
			_pks = false;
		}
	}
	_amp = 0;

	if(bTransparencyEnabled)
		pBackground->drawTransparent(ctx, size, pt);
	else
		pBackground->draw(ctx, size, pt);

	setDirty(false);
}

//------------------------------------------------------------------------------------

void SignalNoisePeakLed::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoisePeakLed::setAmp(double A)
{
	if(A > _amp) _amp = A;
}

//------------------------------------------------------------------------------------

void SignalNoisePeakLed::setHoldTime(dword ms)
{
	_hms = ms;
}

//------------------------------------------------------------------------------------
// on/off led
//------------------------------------------------------------------------------------

SignalNoiseOnOffLed::SignalNoiseOnOffLed(const CRect& rc, CBitmap* map) : CControl(rc, 0, 0)
{
	_on = false;
	_bk = false;
	_bs = false;

	setBackground(map);
}

//------------------------------------------------------------------------------------

SignalNoiseOnOffLed::~SignalNoiseOnOffLed()
{
	//empty
}

//------------------------------------------------------------------------------------

void SignalNoiseOnOffLed::draw(CDrawContext* ctx)
{
	if(!pBackground)
		return;

	CPoint pt;

	if(_bk)
	{
		pt.v = _bs ? size.bottom - size.top : 0;
	}
	else
	{
		pt.v = _on ? size.bottom - size.top : 0;
	}

	if(bTransparencyEnabled)
		pBackground->drawTransparent(ctx, size, pt);
	else
		pBackground->draw(ctx, size, pt);

	setDirty(false);
}

//------------------------------------------------------------------------------------

void SignalNoiseOnOffLed::setDirty(const bool val)
{
	CView::setDirty(val);
}

//------------------------------------------------------------------------------------

void SignalNoiseOnOffLed::setOn(bool on)
{
	_on = on;
}

//------------------------------------------------------------------------------------

void SignalNoiseOnOffLed::setBlink(bool on)
{
	_bk = on;
	_bs = false;
}

//------------------------------------------------------------------------------------

void SignalNoiseOnOffLed::setBlinkState(bool on)
{
	_bs = on;
}

//------------------------------------------------------------------------------------
// user editable label with custom font & background
//------------------------------------------------------------------------------------

SignalNoiseUserLabel::SignalNoiseUserLabel(const CRect& rc, CControlListener* lst, long tag) 
	: CTextEdit(rc, lst, tag)
{
#if WINDOWS
	_font = 0;
#endif // WINDOWS
	_back = false;
}

//------------------------------------------------------------------------------------

SignalNoiseUserLabel::~SignalNoiseUserLabel()
{
	clrFontCustom();
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::clrFontCustom()
{
#if WINDOWS
	if(_font)
	{
		DeleteObject(_font);
		_font = 0;
	}
#endif // WINDOWS
}

//------------------------------------------------------------------------------------

bool SignalNoiseUserLabel::setFontCustom(CDrawContext* ctx)
{
#if WINDOWS
	if(_font)
	{
		SelectObject((HDC)ctx->getSystemContext(), _font);
		return true;
	}
#endif // WINDOWS
	return false;
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::drawStringCustom(CDrawContext* ctx, 
	const char *str, const CRect &rc, const short opq, const CHoriTxtAlign ha)
{
	if(!str) return;
	
	CRect rect(rc);
	rect.offset(ctx->offset.h, ctx->offset.v);

#if WINDOWS
	SetBkMode((HDC)ctx->getSystemContext(), opq ? OPAQUE : TRANSPARENT);
	RECT r = {rect.left, rect.top, rect.right, rect.bottom};
	UINT f = DT_VCENTER + DT_SINGLELINE + DT_NOPREFIX;
	switch(ha)
	{
	case kCenterText:	f += DT_CENTER;	break;
	case kRightText:	f += DT_RIGHT;	break;
	default:			r.left++;		break;	//DT_LEFT == 0
	}
	DrawText((HDC)ctx->getSystemContext(), str, (int)strlen(str), &r, f);
	SetBkMode((HDC)ctx->getSystemContext(), TRANSPARENT);
#endif // WINDOWS
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::draw(CDrawContext* ctx)
{
	//don't draw while in edit mode
	if(platformControl)
	{
		#if MACX
		#if QUARTZ
			if(textControl)
				HIViewSetNeedsDisplay(textControl, true);
			else
		#endif
				TXNDraw((TXNObject)platformControl, NULL);
		#endif
		setDirty(false);
		return;
	}

	setDirty(false);

	//draw back
	if(_back)
	{
		if(pBackground)
		{
			if(bTransparencyEnabled)
				pBackground->drawTransparent(ctx, size, backOffset);
			else
				pBackground->draw(ctx, size, backOffset);
		}
		else if(!bTransparencyEnabled)
		{
			ctx->setFillColor(backColor);
			ctx->fillRect(size);
		}
	}

	//format text (maintain support for CParamDisplay & CTextEdit)
	char out[256] = {0};

	if(editConvert2)
		editConvert2(text, out, userData);
	else if(editConvert)
		editConvert(text, out);
	else if(stringConvert2)
	{
		stringConvert2(value, out, userData);
		strcpy(text, out);
	}
	else if(stringConvert)
	{
		stringConvert(value, out);
		strcpy(text, out);
	}
	else
		sprintf(out, "%s", text);

	//draw text
	if(text)
	{
		CRect oldClip;
		ctx->getClipRect(oldClip);
		CRect newClip(size);
		newClip.bound(oldClip);
		ctx->setClipRect(newClip);

		if(!setFontCustom(ctx))
			ctx->setFont(fontID, 0, txtFace);
	
		//shadow
		if(style == kShadowText) 
		{
			CRect newSize(size);
			newSize.offset(1, 1);
			ctx->setFontColor(shadowColor);
			drawStringCustom(ctx, out, newSize, !bTextTransparencyEnabled, horiTxtAlign);
		}
		ctx->setFontColor(fontColor);
		drawStringCustom(ctx, out, size, !bTextTransparencyEnabled, horiTxtAlign);
		ctx->setClipRect(oldClip);
	}
	setDirty(false);
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::setTextShadow(bool on)
{
	style = on ? kShadowText : 0;
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::setDrawBackground(bool on)
{
	_back = on;
}

//------------------------------------------------------------------------------------

void SignalNoiseUserLabel::loadFontCustom(const char* fnam, long fh, long fs)
{
	clrFontCustom();

#if WINDOWS

	if(fnam)
	{
		LOGFONT logfont = {0};
	
		strcpy(logfont.lfFaceName, fnam);
		logfont.lfWeight			= (fs & kBoldFace) ? FW_BOLD : FW_NORMAL;
		logfont.lfItalic			= fs & kItalicFace;
		logfont.lfUnderline			= fs & kUnderlineFace;
		logfont.lfHeight			= -fh;
		logfont.lfPitchAndFamily	= FIXED_PITCH | FF_DONTCARE;
		logfont.lfClipPrecision		= CLIP_DEFAULT_PRECIS;
		logfont.lfOutPrecision		= OUT_TT_ONLY_PRECIS;
		logfont.lfQuality 			= CLEARTYPE_QUALITY;
		logfont.lfCharSet			= ANSI_CHARSET;

		_font = CreateFontIndirect(&logfont);
	}

#endif // WINDOWS
}

//------------------------------------------------------------------------------------
// arc switch
//------------------------------------------------------------------------------------

SignalNoiseArcSwitch::SignalNoiseArcSwitch(const CRect& rc, 
	float beg, float dif, int num, VstInt32 tag, AudioEffect* eff) : CControl(rc)
{
	_eff = eff;
	_tag = tag;
	_beg = beg;
	_dif = dif;
	_num = num;
	_rad = (rc.right - rc.left) / 2.f;
}

//------------------------------------------------------------------------------------

SignalNoiseArcSwitch::~SignalNoiseArcSwitch()
{
	//empty
}

//------------------------------------------------------------------------------------

void SignalNoiseArcSwitch::draw(CDrawContext* ctx)
{
	// empty

	/* dbg draw
	CColor clr = {255};
	setDirty(false);
	ctx->setFillColor(clr);
	ctx->drawRect(size);
	*/
}

//------------------------------------------------------------------------------------

void SignalNoiseArcSwitch::mouse(CDrawContext* ctx, CPoint& pos, long btn)
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

	int res = 0;
	float flt = _beg;
	for(int i = 0; i < _num; i++)
	{
		if(an < flt)
			break;
		res = i;
		flt += _dif;
	}

	_eff->setParameter(_tag, res * (1.f / float(_num-1)));
}

//------------------------------------------------------------------------------------
// custom knob
//------------------------------------------------------------------------------------

SignalNoiseKnob::SignalNoiseKnob(const CRect& rc, CControlListener* lst, long tag, 
	long nf, CCoord h, CBitmap* map, CPoint& pt) : CAnimKnob(rc, lst, tag, nf, h, map, pt)
{
	_rng = 400.f;
}

//------------------------------------------------------------------------------------

SignalNoiseKnob::~SignalNoiseKnob()
{
	//empty
}

//------------------------------------------------------------------------------------

void SignalNoiseKnob::mouse(CDrawContext* ctx, CPoint& hit, long btn)
{
	if(!bMouseEnabled)
		return;
	if(btn == -1)
		btn = ctx->getMouseButtons();
	if(!(btn & kLButton))
		return;
	if(listener && btn & (kAlt | kShift | kControl | kApple))
	{
		if(listener->controlModifierClicked(ctx, this, btn) != 0)
			return;
	}
	if(checkDefaultValue(ctx, btn))
		return;

	//setup
	CCoord dt;
	CPoint fp = hit;
	CPoint pt(-1, -1);
	float cv = value;
	float cf = (vmax - vmin) / _rng;

	//edit
	beginEdit();
	do
	{
		btn = ctx->getMouseButtons();
		if(hit != pt)
		{
			pt = hit;
			dt = (fp.v - hit.v) + (hit.h - fp.h);
			value = cv + dt * cf;
			bounceValue();
			
			if(isDirty() && listener)
				listener->valueChanged(ctx, this);
		}
		getMouseLocation(ctx, hit);
		doIdleStuff();
	}
	while(btn & kLButton);

	endEdit();
}

//------------------------------------------------------------------------------------

void SignalNoiseKnob::setRange(float r)
{
	_rng = r;
}

//------------------------------------------------------------------------------------
// custom precision knob
//------------------------------------------------------------------------------------

SignalNoiseKnobP::SignalNoiseKnobP(const CRect& rc, CControlListener* lst, long tag, 
	long nf, CCoord h, CBitmap* map, CPoint& pt) : CAnimKnob(rc, lst, tag, nf, h, map, pt)
{
	_rng = 400.f;
	_abs = 1.f;
	_lnk = 0;
}

//------------------------------------------------------------------------------------

SignalNoiseKnobP::~SignalNoiseKnobP()
{
	//empty
}

//------------------------------------------------------------------------------------

void SignalNoiseKnobP::mouse(CDrawContext* ctx, CPoint& hit, long btn)
{
	if(!bMouseEnabled)
		return;
	if(btn == -1)
		btn = ctx->getMouseButtons();
	if(!(btn & kLButton))
		return;
	if(listener && btn & (kAlt | kShift | kControl | kApple))
	{
		if(listener->controlModifierClicked(ctx, this, btn) != 0)
			return;
	}
	if(checkDefaultValue(ctx, btn))
		return;

	//setup
	long old;
	CCoord dt;
	CPoint fp, pt(-1, -1);
	float valA, rngA, relA, absA;
	float valB, rngB, relB;

	if(btn & kShift)
	{
		rngA = _abs * 10;
		if(_lnk) rngB = _lnk->getRangeAbsolute() * 10;
	}
	else
	{
		rngA = _abs;
		if(_lnk) rngB = _lnk->getRangeAbsolute();
	}
	relA = 1.0f / rngA;
	absA = _rng / _abs;
	valA = value;
	old = btn;
	fp = hit;
	if(_lnk)
	{
		relB = 1.0f / rngB;
		valB = _lnk->getValue();
	}

	//edit
	beginEdit();
	do
	{
		btn = ctx->getMouseButtons();
		if(hit != pt)
		{
			pt = hit;
			
			if(btn != old)
			{
				if(btn & kShift)
				{
					rngA = _abs * 10;
					if(_lnk) rngB = _lnk->getRangeAbsolute() * 10;
				}
				else
				{
					rngA = _abs;
					if(_lnk) rngB = _lnk->getRangeAbsolute();
				}
				relA = 1.0f / rngA;
				absA = _rng / _abs;
				valA = value;
				old = btn;
				fp = hit;
				if(_lnk)
				{
					relB = 1.0f / rngB;
					valB = _lnk->getValue();
				}
			}
			else
			{
				dt = (fp.v - hit.v) + (hit.h - fp.h);
				value = valA + float(long(dt / absA)) * relA;
				bounceValue();
				if(isDirty() && listener)
					listener->valueChanged(ctx, this);

				if(_lnk && btn & kAlt)
				{
					_lnk->setValue(valB - float(long(dt / absA)) * relB);
					_lnk->bounceValue();
					if(_lnk->isDirty() && listener)
						listener->valueChanged(ctx, _lnk);
				}
			}
		}
		getMouseLocation(ctx, hit);
		doIdleStuff();
	}
	while(btn & kLButton);

	endEdit();
}

//------------------------------------------------------------------------------------

void SignalNoiseKnobP::setRangePixels(float r)
{
	_rng = r;
}

//------------------------------------------------------------------------------------

void SignalNoiseKnobP::setRangeAbsolute(float r)
{
	_abs = r;
}

//------------------------------------------------------------------------------------

void SignalNoiseKnobP::setLinkInversed(SignalNoiseKnobP* lnk)
{
	_lnk = lnk;
}

//------------------------------------------------------------------------------------

float SignalNoiseKnobP::getRangeAbsolute()
{
	return _abs;
}

//------------------------------------------------------------------------------------
