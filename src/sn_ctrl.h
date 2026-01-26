//------------------------------------------------------------------------------------
//
//	file:		sn_ctrl.h
//
//	purpose:	VST custom GUI controls
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_CTRL_H
#define _SN_CTRL_H


#include <audioeffectx.h>
#include <audioeffect.h>


//------------------------------------------------------------------------------------

#include <sn_core.h>
#include <vstgui.h>
#include <aeffguieditor.h>

//------------------------------------------------------------------------------------
// GUI helpers
//------------------------------------------------------------------------------------

void snResizeEditor(AEffGUIEditor* edit, CCoord w, CCoord h);
void snResizeControl(CControl* ctrl, CCoord x, CCoord y, CCoord w, CCoord h);

//------------------------------------------------------------------------------------
// GR meter [0..20]dB (simple exponential approximation)
//------------------------------------------------------------------------------------

class SignalNoiseGR : public CControl
{
private:
	double		_env;	// envelope coefficient
	double		_val;	// current value
	CBitmap*	_map;	// needle bitmap (vertical stack)
	dword		_num;	// num of map frames
public:
//create & destroy
	SignalNoiseGR(const CRect& rc, CBitmap* map, dword num, float fs);
	virtual ~SignalNoiseGR();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setVal(double dB);			// standard GR
	void setFilter(double fs);
//vtable
	CLASS_METHODS(SignalNoiseGR, CControl)
};

//------------------------------------------------------------------------------------
// VU meter [-20..+3]dB - simple adaptation (without oversampling) of the 
// original algorithm by (c) 2005-2006 Bryce E. Lobdell and Jont B. Allen
// 'A model of the VU (volume-unit) meter, with speech applications' (VUSOFT)
// original paper @ https://www.researchgate.net/publication/6507148
//------------------------------------------------------------------------------------

#define VU_MAX_TICKS					 12
#define VU_DEFAULT_NOMINAL_LEVEL		-18

//------------------------------------------------------------------------------------

struct vutick_t
{
	double	amp;	// linear amplitude of this mark
	dword	frm;	// bitmap frame (zero-based index)
};

//------------------------------------------------------------------------------------

class SignalNoiseVU : public CControl
{
private:
	//filter - constants
	static const double _wn;				// wn
	static const double _et;				// eta
	static const double _sc;				// y0 scaling (PI / (2 * sqrt(600*0.001*2))
	//filter - input/output
	double				_x0;				// <- no need to store this !!!
	double				_x1;
	double				_x2;
	double				_y0;
	double				_y1;
	double				_y2;
	//filter - coefficients
	double				_a0;				// <- no need to store this !!!
	double				_a1;
	double				_a2;
	double				_b0;
	double				_b1;
	double				_b2;
	//display
	vutick_t			_frm[VU_MAX_TICKS];	// frames [-20,-10,-7,-5,-3,-2,-1,0,+1,+2,+3,max]
	CBitmap*			_map;				// std. needle bitmap (vertical stack)
	CBitmap*			_pkm;				// peak needle bitmap (vertical stack)
	dword				_hms;				// peak hold time in ms
	dword				_hst;				// peak hold start time
	dword				_hfn;				// peak hold frame number
public:
//create & destroy
	SignalNoiseVU(const CRect& rc, CBitmap* map, dword* vut, float hz);
	virtual ~SignalNoiseVU();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setAmp(double A);
	void setLevel(double nl);
	void setFrames(dword* frm);
	void setFilter(double hz);
	void setHoldTime(dword ms);
	void setBitmap(CBitmap* map);
	void setPeakBitmap(CBitmap* map);
//vtable
	CLASS_METHODS(SignalNoiseVU, CControl)
};

//------------------------------------------------------------------------------------
// sample peak meter - bitmap must have two frames !!!
//------------------------------------------------------------------------------------

class SignalNoisePeakMeter : public CControl
{
private:
	double		_val;	// value
	double		_rng;	// range
	CBitmap*	_map;	// bitmap
public:
//create & destroy
	SignalNoisePeakMeter(const CRect& rc, CBitmap* map, float fs);
	virtual ~SignalNoisePeakMeter();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setVal(double dB);
	void setRange(double dB);
//vtable
	CLASS_METHODS(SignalNoisePeakMeter, CControl)
};

//------------------------------------------------------------------------------------
// sample peak led - bitmap must have two frames !!!
//------------------------------------------------------------------------------------

class SignalNoisePeakLed : public CControl
{
private:
	double	_amp;	// actual amplitude
	dword	_hms;	// peak hold time in ms
	dword	_hst;	// peak hold start time
	bool	_pks;	// is peaking
public:
//create & destroy
	SignalNoisePeakLed(const CRect& rc, CBitmap* map);
	virtual ~SignalNoisePeakLed();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setAmp(double A);
	void setHoldTime(dword ms);
//vtable
	CLASS_METHODS(SignalNoisePeakLed, CControl)
};

//------------------------------------------------------------------------------------
// on/off led with optional blinking state - bitmap must have two frames !!!
//------------------------------------------------------------------------------------

class SignalNoiseOnOffLed : public CControl
{
private:
	bool	_on;	// is on
	bool	_bk;	// is blinking
	bool	_bs;	// blinking state
public:
//create & destroy
	SignalNoiseOnOffLed(const CRect& rc, CBitmap* map);
	virtual ~SignalNoiseOnOffLed();	
//from SDK
	virtual void draw(CDrawContext* ctx);
	virtual void setDirty(const bool val = true);
//custom
	void setOn(bool on);
	void setBlink(bool on);
	void setBlinkState(bool on);
//vtable
	CLASS_METHODS(SignalNoiseOnOffLed, CControl)
};

//------------------------------------------------------------------------------------
// user editable label with custom font & background
//------------------------------------------------------------------------------------

class SignalNoiseUserLabel : public CTextEdit
{
#if WINDOWS
	HANDLE	_font;	// font resource
#endif // WINDOWS
	bool	_back;	// if true => draw bitmap
private:
	void clrFontCustom();
	bool setFontCustom(CDrawContext* ctx);
	void drawStringCustom(CDrawContext* ctx, const char *str, 
		const CRect &rc, const short opq, const CHoriTxtAlign ha);
public:
//create & destroy
	SignalNoiseUserLabel(const CRect& rc, CControlListener* lst, long tag);
	virtual ~SignalNoiseUserLabel();	
//from SDK
	virtual void draw(CDrawContext* ctx);
//custom
	void setTextShadow(bool on);
	void setDrawBackground(bool on);
	void loadFontCustom(const char* fnam, long fh, long fs); //font must be installed !!!
//vtable
	CLASS_METHODS(SignalNoiseUserLabel, CTextEdit)
};

//------------------------------------------------------------------------------------
// arc switch - invisible control that sets the value of "tag" by clicking into one 
// of "num" sectors that start at "beg"; 0 degrees is at [0, -rad], rotation CW
//------------------------------------------------------------------------------------

class SignalNoiseArcSwitch : public CControl
{
private:
	AudioEffect*	_eff;	// associated effect
	VstInt32		_tag;	// knob to notify
	float			_beg;	// start angle
	float			_dif;	// step in degrees
	float			_rad;	// inscribed circle
	int				_num;	// num of blocks
public:
//create & destroy
	SignalNoiseArcSwitch(const CRect& rc, float beg, 
		float dif, int num, VstInt32 tag, AudioEffect* eff);
	virtual ~SignalNoiseArcSwitch();	
//from SDK
	virtual void draw(CDrawContext* ctx);
//	virtual void mouse(CDrawContext* ctx, CPoint& pos, long btn = -1);
//vtable
	CLASS_METHODS(SignalNoiseArcSwitch, CControl)
};

//------------------------------------------------------------------------------------
// custom knob with adjustable sensitivity, always linear, no drag kbd. modifiers
//------------------------------------------------------------------------------------

class SignalNoiseKnob : public CAnimKnob
{
private:
	float _rng;	// range (pixels)
public:
//create & destroy
	SignalNoiseKnob(const CRect& rc, CControlListener* lst, 
		long tag, long nf, CCoord h, CBitmap* map, CPoint& pt);
	virtual ~SignalNoiseKnob();	
//from SDK
	virtual void mouse(CDrawContext* ctx, CPoint& pos, long btn = -1);
//custom
	void setRange(float r);
//vtable
	CLASS_METHODS(SignalNoiseKnob, CAnimKnob)
};

//------------------------------------------------------------------------------------
// custom precision knob - always linear, must set absolute range to work properly!!!
// - optional "lnk" knob that turns in opposite direction by the same amount of units
// - ticks on GUI panel should fall on whole numbers to look good :)
// - navigation scheme:
//   left mouse				-> +/- 1.0 unit
//   left mouse + shift		-> +/- 0.1 unit
//   left mouse + alt		-> turn linked
//------------------------------------------------------------------------------------

class SignalNoiseKnobP : public CAnimKnob
{
private:
	SignalNoiseKnobP*	_lnk;	// linked inversed knob
	float				_rng;	// range (pixels)
	float				_abs;	// range (absolute)
public:
//create & destroy
	SignalNoiseKnobP(const CRect& rc, CControlListener* lst, 
		long tag, long nf, CCoord h, CBitmap* map, CPoint& pt);
	virtual ~SignalNoiseKnobP();	
//from SDK
	virtual void mouse(CDrawContext* ctx, CPoint& pos, long btn = -1);
//custom
	void setRangePixels(float r);
	void setRangeAbsolute(float r);
	void setLinkInversed(SignalNoiseKnobP* lnk);
	float getRangeAbsolute();
//vtable
	CLASS_METHODS(SignalNoiseKnobP, CAnimKnob)
};

//------------------------------------------------------------------------------------


#endif // _SN_CTRL_H
