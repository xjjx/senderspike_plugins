//------------------------------------------------------------------------------------
//
//	file:		sn_vsti.h
//
//	purpose:	Signal Noise VST 2.4 plugin interface
//				- define SNFX_NO_SIGNAL_DEFAULT_GUI to bypass VSTGUI SDK when using
//				  SignalNoiseFX interface
//
//  authors:	2019 - 2025 Oto Spál
//
//------------------------------------------------------------------------------------


#ifndef _SN_VSTI_H
#define _SN_VSTI_H

#define VST_FOURCC(a,b,c,d) \
    ((VstInt32)(a)        | ((VstInt32)(b) << 8) | \
     ((VstInt32)(c) << 16)| ((VstInt32)(d) << 24))

//------------------------------------------------------------------------------------

#include <audioeffectx.h>

//------------------------------------------------------------------------------------
// types & consts
//------------------------------------------------------------------------------------

typedef unsigned short word;

//------------------------------------------------------------------------------------
// generic VST effect
//------------------------------------------------------------------------------------

struct param_t
{
	const char* nam;
	const char* lbl;
	float		val;
};

//------------------------------------------------------------------------------------

class SignalNoiseFX : public AudioEffectX
{
protected:
	const char**	_query;		// plugin properties
	param_t*		_param;		// plugin parameters
	word			_pnum;		// number of params
	word			_qnum;		// number of queries
	bool			_mono;		// speaker arrangement (mono => true)
	bool			_idle;		// playback is stopped
protected:
	void InitQueries(const char** q, word n);
	void InitParams(const param_t* p, word n);
//callbacks
	virtual void onSetSampleRate(float fs)				{ /* empty */ }
	virtual void onSetParameter(VstInt32 at, float v)	{ /* empty */ }
public:
//create & destroy
	SignalNoiseFX(audioMasterCallback cb, VstInt32 npgm, VstInt32 nprm);
	virtual ~SignalNoiseFX();
//plugin
	virtual bool getVendorString(char* txt);
	virtual void getProgramName(char* name);
	virtual void setProgramName(char* name);
//params
	virtual float getParameter(VstInt32 at);
	virtual void setParameter(VstInt32 at, float v);
	virtual void getParameterLabel(VstInt32 at, char* txt);
	virtual void getParameterName(VstInt32 at, char* txt);
	virtual void getParameterDisplay(VstInt32 at, char* txt);
	virtual VstInt32 getChunk(void** data, bool preset);
	virtual VstInt32 setChunk(void* data, VstInt32 sz, bool preset);
//query
	virtual VstInt32 canDo(char* txt);
//query - custom
	bool isPlaying();			// called only from isPlayStarted() below (could be moved there ...)
	bool isPlayStarted();		// used only by FX2 
//states
	virtual void setSampleRate(float fs);
	virtual bool setSpeakerArrangement(VstSpeakerArrangement* in, VstSpeakerArrangement* out);
    AEffect* getAeffect() { return &cEffect; }
};

//------------------------------------------------------------------------------------
// helper to use in effect class
//------------------------------------------------------------------------------------

#define VST_DEFINE_PLUGINFO(nam, ver, typ)							\
virtual bool getProductString(char* txt) {							\
	vst_strncpy(txt, nam, kVstMaxProductStrLen); return true; }		\
virtual bool getEffectName(char* txt) {								\
	vst_strncpy(txt, nam, kVstMaxEffectNameLen); return true; }		\
virtual VstPlugCategory getPlugCategory()	{ return typ; }			\
virtual VstInt32 getVendorVersion()			{ return ver; }

//------------------------------------------------------------------------------------
// generic custom UI
//------------------------------------------------------------------------------------

#ifdef SNFX_NO_SIGNAL_DEFAULT_GUI

//------------------------------------------------------------------------------------

#include <aeffeditor.h>

//------------------------------------------------------------------------------------

class SignalNoiseUI : public AEffEditor
{
public:
//create & destroy
	SignalNoiseUI(AudioEffect* effect) : AEffEditor(effect) { /* empty */ }
	virtual ~SignalNoiseUI()								{ /* empty */ }
//callbacks
	virtual void setParameter(VstInt32 index, float value)	{ /* empty */ }
};

//------------------------------------------------------------------------------------

#endif //SNFX_NO_SIGNAL_DEFAULT_GUI

//------------------------------------------------------------------------------------


#endif // _SN_CGUI_H
