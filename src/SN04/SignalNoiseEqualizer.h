//------------------------------------------------------------------------------------
//
//	file:		sn_04e.h
//
//	purpose:	SN04 Channel EQ effect
//
//	authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#pragma once


//------------------------------------------------------------------------------------

#include <juce_audio_processors/juce_audio_processors.h>
#include <sn_core.h>
#include "SignalNoiseFX.h"

//------------------------------------------------------------------------------------
// effect
//------------------------------------------------------------------------------------

enum
{
	SNE_GAIN = 0,	// output
	SNE_IPHS,		// phase invert
	SNE_HF_F,		// HF freq
	SNE_HF_G,		// HF gain
	SNE_HF_Q,		// HF width
	SNE_HF_T,		// HF curve type
	SNE_HF_M,		// HF mode [HSF, PKF]
	SNE_MF_F,		// MF freq
	SNE_MF_G,		// MF gain
	SNE_MF_Q,		// MF width
	SNE_MF_T,		// MF curve type
	SNE_LF_F,		// LF freq
	SNE_LF_G,		// LF gain
	SNE_LF_Q,		// LF width
	SNE_LF_T,		// LF curve type
	SNE_LF_M,		// LF mode [LSF, PKF]
	SNE_LPAS,		// low pass
	SNE_HPAS,		// high pass
	SNE_LOCT,		// low pass dB/oct [6,12,18,24]
	SNE_HOCT,		// high pass dB/oct [6,12,18,24]
	SNE_MOJO,		// "analog"
	SNE_HF_B,		// bypass HF
	SNE_MF_B,		// bypass MF
	SNE_LF_B,		// bypass LF
	SNE_LP_B,		// bypass LPF
	SNE_HP_B,		// bypass HPF
	SNE_SIZE,		// num of params
};

static const std::vector<const char*> On = { "Off", "On" };
static const std::vector<const char*> pHF = { "Disable", "1.1 kHz", "2.2 kHz", "3.9 kHz", "5.8 kHz", "8.2 kHz", "10 kHz", "12 kHz" };
static const std::vector<const char*> pMF = { "Disable", "220 Hz", "360 Hz", "700 Hz", "1.6 kHz", "3.2 kHz", "4.8 kHz",  "7.2 kHz" };
static const std::vector<const char*> pLF = { "Disable", "35 Hz", "45 Hz", "60 Hz", "110 Hz", "220 Hz", "300 Hz", "400 Hz" };
// FIXME: we probably need to use AudioParameterInt here 
//static const std::vector<const char*> pnLF = { "Disable", "120 Hz", "150 Hz", "200 Hz", "300 Hz", "440 Hz", "600 Hz", "800 Hz" };
static const std::vector<const char*> pLP = { "Disable", "18 kHz", "14 kHz", "10 kHz", "8 kHz", "6 kHz" };
static const std::vector<const char*> pHP = { "Disable", "15 Hz", "45 Hz", "70 Hz", "160 Hz", "360 Hz" };
static const std::vector<const char*> pSlope = { "6dB/oct", "12dB/oct", "18dB/oct", "24dB/oct" };
static const ParamDesc gParams[SNE_SIZE] =
{
	{ ParamType::Decibel,    "gain",  "Output",   "dB", -25.0f, 25.0f,  0.0f }, // SNE_GAIN
	{ ParamType::Choice,     "iphs",  "Phase",      "",  0.0f,  1.0f,  0.0f, On }, // SNE_IPHS

	{ ParamType::Choice,     "hf_f",  "HF Freq",    "",   0.0f,  1.0f, 0.00f, pHF }, // SNE_HF_F
	{ ParamType::Decibel,    "hf_g",  "HF Gain",  "dB", -18.0f, 18.0f,  0.0f }, // SNE_HF_G
	{ ParamType::Normalized, "hf_q",  "HF BW",    "oct",  0.0f,  1.0f, 0.50f }, // SNE_HF_Q
	{ ParamType::Normalized, "hf_t",  "HF Type",  "typ",  0.0f,  1.0f, 0.00f }, // SNE_HF_T
	{ ParamType::Normalized, "hf_m",  "HF Mode",  "n/y",  0.0f,  1.0f, 0.00f }, // SNE_HF_M

	{ ParamType::Choice,     "mf_f",  "MF Freq",    "",   0.0f,  1.0f, 0.00f, pMF }, // SNE_MF_F
	{ ParamType::Decibel,    "mf_g",  "MF Gain",  "dB", -18.0f, 18.0f,  0.0f }, // SNE_MF_G
	{ ParamType::Normalized, "mf_q",  "MF BW",    "oct",  0.0f,  1.0f, 0.50f }, // SNE_MF_Q
	{ ParamType::Normalized, "mf_t",  "MF Type",  "typ",  0.0f,  1.0f, 0.00f }, // SNE_MF_T

	{ ParamType::Normalized, "lf_f",  "LF Freq",    "",   0.0f,  1.0f, 0.00f, pLF }, // SNE_LF_F
	{ ParamType::Decibel,    "lf_g",  "LF Gain",  "dB", -18.0f, 18.0f,  0.0f }, // SNE_LF_G
	{ ParamType::Normalized, "lf_q",  "LF BW",    "oct",  0.0f,  1.0f, 0.50f }, // SNE_LF_Q
	{ ParamType::Normalized, "lf_t",  "LF Type",  "typ",  0.0f,  1.0f, 0.00f }, // SNE_LF_T
	{ ParamType::Normalized, "lf_m",  "LF Mode",  "n/y",  0.0f,  1.0f, 0.00f }, // SNE_LF_M

	{ ParamType::Choice,     "lpas",  "Lo-Pass",  "Hz",   0.0f,  1.0f, 0.00f, pLP }, // SNE_LPAS
	{ ParamType::Choice,     "hpas",  "Hi-Pass",  "Hz",   0.0f,  1.0f, 0.00f, pHP }, // SNE_HPAS
	{ ParamType::Choice,     "loct",  "Lo Slope",   "", 0.0f,  1.0f, 0.00f, pSlope }, // SNE_LOCT
	{ ParamType::Choice,     "hoct",  "Hi Slope",   "", 0.0f,  1.0f, 0.00f, pSlope }, // SNE_HOCT

	{ ParamType::Choice,     "mojo",  "Analog",      "",  0.0f,  1.0f, 0.00f, On }, // SNE_MOJO

	{ ParamType::Choice,     "hf_b",  "Mute HF",  "n/y",  0.0f,  1.0f, 0.00f, On }, // SNE_HF_B
	{ ParamType::Choice,     "mf_b",  "Mute MF",  "n/y",  0.0f,  1.0f, 0.00f, On }, // SNE_MF_B
	{ ParamType::Choice,     "lf_b",  "Mute LF",  "n/y",  0.0f,  1.0f, 0.00f, On }, // SNE_LF_B
	{ ParamType::Choice,     "lp_b",  "Mute LPF", "n/y",  0.0f,  1.0f, 0.00f, On }, // SNE_LP_B
	{ ParamType::Choice,     "hp_b",  "Mute HPF", "n/y",  0.0f,  1.0f, 0.00f, On }, // SNE_HP_B
};

//------------------------------------------------------------------------------------

#define SN04_VER		1210
#ifdef SN04G
#define SN04_NAM		"SN04-G Channel EQ"
#else
#define SN04_NAM		"SN04 Channel EQ"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseEqualizer : public SignalNoiseFX
{
private:
//bands		   L		R
	biquad	_hf_La,  _hf_Ra;
	foHSF	_hf_Lb,  _hf_Rb;
	biquad	_mf_Lp,  _mf_Rp;
	biquad	_lf_La,  _lf_Ra;
	biquad	_lf_Lb,  _lf_Rb;
	foLSF	_lf_Lc,  _lf_Rc;
//LPF
	foLPF	_lp06_L, _lp06_R;
	biquad	_lp12_L, _lp12_R;
	biquad	_lp24_L, _lp24_R;
//HPF
	foHPF	_hp06_L, _hp06_R;
	biquad	_hp12_L, _hp12_R;
	biquad	_hp24_L, _hp24_R;
//character
	foHSF	_hsfL,	 _hsfR;
//other
	noise	_mojo;
	double	_norm;
private:
	void setupHF();
	void setupMF();
	void setupLF();
	void setupLP();
	void setupHP();
	void setupAnalog();

	template <typename Sample>
	void processImpl(juce::AudioBuffer<Sample>& buffer);

	static juce::AudioProcessorValueTreeState::ParameterLayout
	createParameterLayout();

	int paramIdToIndex (const juce::String& id);

	inline float getParamValue (int idx) const noexcept
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return ptr->load();
	}

	inline float getParamNorm (int idx) const noexcept
	{
		auto* p = getParameters().getParameter(gParams[idx].id);
		return p->getValue();
	}

	inline int getParamChoice(int idx)
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return static_cast<int>(ptr->load());
	}

public:
	SignalNoiseEqualizer();
	~SignalNoiseEqualizer() override = default;

	// JUCE overrides
	void prepareToPlay(double newSampleRate, int samplesPerBlock) override;

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return false; }

	void parameterChanged(const juce::String& parameterID, float newValue) override;

//	float getInputLevel()  const noexcept { return inputLevel.load(); }
//	float getOutputLevel() const noexcept { return outputLevel.load(); }
};
