//------------------------------------------------------------------------------------
//
//	file:		sn_vsti.cpp
//
//	purpose:	Signal Noise VST 2.4 plugin interface
//
//  authors:	2019 - 2025 Oto Spál
//
//------------------------------------------------------------------------------------

#include <cstdlib>
#include <sn_core.h>
#include <sn_vsti.h>
#ifndef SNFX_NO_SIGNAL_DEFAULT_GUI
#include <aeffguieditor.h>
#endif //SNFX_NO_SIGNAL_DEFAULT_GUI


//------------------------------------------------------------------------------------
// class SignalNoiseFX
//------------------------------------------------------------------------------------

SignalNoiseFX::SignalNoiseFX(audioMasterCallback cb, VstInt32 npgm, VstInt32 nprm)
	: AudioEffectX(cb, npgm, nprm)
{
	_param = NULL;
	_query = NULL;
	_pnum = nprm;
	_qnum = 0;
	_mono = false;
	_idle = true;
	if(nprm)
	{
		_param = new param_t[nprm];
		memset(_param, 0, sizeof(param_t) * nprm);
	}
}

//------------------------------------------------------------------------------------

SignalNoiseFX::~SignalNoiseFX()
{
	if(_param)
		delete[] _param;
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::InitQueries(const char** q, word n)
{
	_query = q;
	_qnum = n;
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::InitParams(const param_t* p, word n)
{
	for(word i = 0; i < n; i++)
	{
		_param[i].nam = p[i].nam;
		_param[i].lbl = p[i].lbl;
		_param[i].val = p[i].val;
	}
}

//------------------------------------------------------------------------------------

bool SignalNoiseFX::getVendorString(char* txt)
{
	vst_strncpy(txt, "Sender Spike", kVstMaxVendorStrLen);
	return true;
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::getProgramName(char* name)
{
	vst_strncpy(name, "Default", kVstMaxProgNameLen);
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::setProgramName(char* name)
{
	/* empty */
}

//------------------------------------------------------------------------------------

VstInt32 SignalNoiseFX::getChunk(void** data, bool preset)
{
	if(_pnum)
	{
		size_t sz = _pnum * sizeof(float);
		float* param = (float*)malloc(sz);
		for(word i = 0; i < _pnum; i++)
			param[i] = _param[i].val;
		*data = param;
		return (VstInt32)sz;
	}
	return 0;
}

//------------------------------------------------------------------------------------

VstInt32 SignalNoiseFX::setChunk(void* data, VstInt32 sz, bool preset)
{
	float* param = (float*)data;
	for(word i = 0; i < _pnum; i++)
	{
		_param[i].val = clampf(param[i]);
		onSetParameter(i, _param[i].val);
		if(editor)
		{
#ifndef SNFX_NO_SIGNAL_DEFAULT_GUI
			((AEffGUIEditor*)editor)->setParameter(i, _param[i].val);
#else
			((SignalNoiseUI*)editor)->setParameter(i, _param[i].val);
#endif //SNFX_NO_SIGNAL_DEFAULT_GUI
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------

float SignalNoiseFX::getParameter(VstInt32 at)
{
	if(at >= 0 && at < _pnum)
		return _param[at].val;
	return 0;
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::setParameter(VstInt32 at, float v)
{
	if(at >= 0 && at < _pnum)
	{
		_param[at].val = clampf(v);
		onSetParameter(at, _param[at].val);
		if(editor)
		{
#ifndef SNFX_NO_SIGNAL_DEFAULT_GUI
			((AEffGUIEditor*)editor)->setParameter(at, _param[at].val);
#else
			((SignalNoiseUI*)editor)->setParameter(at, _param[at].val);
#endif //SNFX_NO_SIGNAL_DEFAULT_GUI
		}
	}
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::getParameterLabel(VstInt32 at, char* txt)
{
	if(at >= 0 && at < _pnum)
		vst_strncpy(txt, _param[at].lbl, kVstMaxParamStrLen);
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::getParameterName(VstInt32 at, char* txt)
{
	if(at >= 0 && at < _pnum)
		vst_strncpy(txt, _param[at].nam, kVstMaxParamStrLen);
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::getParameterDisplay(VstInt32 at, char* txt)
{
	if(at >= 0 && at < _pnum)
		float2string(_param[at].val, txt, kVstMaxParamStrLen);
}

//------------------------------------------------------------------------------------

VstInt32 SignalNoiseFX::canDo(char* txt)
{
	for(int i = 0; i < _qnum; i++)
	{
		if(strcmp(txt, _query[i]) == 0)
			return 1;
	}
	return -1;
}

//------------------------------------------------------------------------------------

bool SignalNoiseFX::isPlaying()
{
	VstTimeInfo* vst = getTimeInfo(0);
	if(vst && vst->flags & kVstTransportPlaying)
		return true;
	_idle = true;
	return false;
}

//------------------------------------------------------------------------------------

bool SignalNoiseFX::isPlayStarted()
{
	if(isPlaying() && _idle)
	{
		_idle = false;
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------

void SignalNoiseFX::setSampleRate(float fs)
{
	AudioEffectX::setSampleRate(fs);
	onSetSampleRate(fs);
}

//------------------------------------------------------------------------------------

bool SignalNoiseFX::setSpeakerArrangement(VstSpeakerArrangement* in, VstSpeakerArrangement* out)
{
	bool r = false;
	if(in && out)
	{
		if(in->numChannels == 1 && out->numChannels == 1)
			_mono = true;
		else
			_mono = false;
		r = in->numChannels == out->numChannels && (in->numChannels == 1 || in->numChannels == 2);
	}
	return r;
}

//------------------------------------------------------------------------------------
