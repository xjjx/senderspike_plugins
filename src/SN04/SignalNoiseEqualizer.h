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

static const ParamDesc gParams[SNE_SIZE] =
{
	{ "gain",  "Output",   "dB",   0.50f }, // SNE_GAIN
	{ "iphs",  "Phase",    "n/y",  0.00f }, // SNE_IPHS

	{ "hf_f",  "HF Freq",  "Hz",   0.00f }, // SNE_HF_F
	{ "hf_g",  "HF Gain",  "dB",   0.50f }, // SNE_HF_G
	{ "hf_q",  "HF BW",    "oct",  0.50f }, // SNE_HF_Q
	{ "hf_t",  "HF Type",  "typ",  0.00f }, // SNE_HF_T
	{ "hf_m",  "HF Mode",  "n/y",  0.00f }, // SNE_HF_M

	{ "mf_f",  "MF Freq",  "Hz",   0.00f }, // SNE_MF_F
	{ "mf_g",  "MF Gain",  "dB",   0.50f }, // SNE_MF_G
	{ "mf_q",  "MF BW",    "oct",  0.50f }, // SNE_MF_Q
	{ "mf_t",  "MF Type",  "typ",  0.00f }, // SNE_MF_T

	{ "lf_f",  "LF Freq",  "Hz",   0.00f }, // SNE_LF_F
	{ "lf_g",  "LF Gain",  "dB",   0.50f }, // SNE_LF_G
	{ "lf_q",  "LF BW",    "oct",  0.50f }, // SNE_LF_Q
	{ "lf_t",  "LF Type",  "typ",  0.00f }, // SNE_LF_T
	{ "lf_m",  "LF Mode",  "n/y",  0.00f }, // SNE_LF_M

	{ "lpas",  "Lo-Pass",  "Hz",   0.00f }, // SNE_LPAS
	{ "hpas",  "Hi-Pass",  "Hz",   0.00f }, // SNE_HPAS
	{ "loct",  "Lo Slope", "dB/o", 0.00f }, // SNE_LOCT
	{ "hoct",  "Hi Slope", "dB/o", 0.00f }, // SNE_HOCT

	{ "mojo",  "Analog",   "n/y",  0.00f }, // SNE_MOJO

	{ "hf_b",  "Mute HF",  "n/y",  0.00f }, // SNE_HF_B
	{ "mf_b",  "Mute MF",  "n/y",  0.00f }, // SNE_MF_B
	{ "lf_b",  "Mute LF",  "n/y",  0.00f }, // SNE_LF_B
	{ "lp_b",  "Mute LPF", "n/y",  0.00f }, // SNE_LP_B
	{ "hp_b",  "Mute HPF", "n/y",  0.00f }, // SNE_HP_B
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

	inline float getParamNorm (int idx) const noexcept
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return ptr->load();
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

	void getStateInformation(juce::MemoryBlock&) override;
	void setStateInformation(const void*, int) override;

//	float getInputLevel()  const noexcept { return inputLevel.load(); }
//	float getOutputLevel() const noexcept { return outputLevel.load(); }
};
