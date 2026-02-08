//------------------------------------------------------------------------------------
//
//	file:		sn_03e.h
//
//	purpose:	SN03 tape recorder emulator effect
//
//  authors:	2019 - 2021 Oto Spál
//
//------------------------------------------------------------------------------------


#pragma once


//------------------------------------------------------------------------------------

#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <sn_core.h>
#include "SignalNoiseFX.h"

//------------------------------------------------------------------------------------

enum
{
	SNE_TRIM,		// input trim +/-24dB
	SNE_GAIN,		// output gain +/-24dB
	SNE_EQSC,		// EQ curve [NAB, IEC 15, AES]
	SNE_RCLO,		// rec EQ - bass +/-10dB
	SNE_RCHI,		// rec EQ - high +/-14dB
	SNE_RPLO,		// rep EQ - bass +/-10dB
	SNE_RPHI,		// rep EQ - high +/-14dB
	SNE_HEAD,		// head bump frequency
	SNE_BUMP,		// head bump magitude
	SNE_HISS,		// hiss gain
	SNE_ROOM,		// VU meter headroom
	SNE_HOLD,		// VU meter peak hold
	SNE_PATH,		// VU meter path
	SNE_ATTN,		// bump attenuator
	SNE_NOIS,		// noise on/off
	SNE_HBON,		// head bump on/off
	SNE_LOON,		// force LO on
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const std::vector<const char*> On = { "Off", "On" };
static const std::vector<const char*> EQSC = { "NAB", "IEC 15 IPS", "AES 30 IPS" };
static const ParamDesc gParams[] =
{
	{ ParamType::Decibel,    "trim", "Input",    "dB", -24.0f, 24.0f,    0.0f }, // SNE_TRIM input trim +/-24 dB
	{ ParamType::Decibel,    "gain", "Output",   "dB", -24.0f, 24.0f,    0.0f }, // SNE_GAIN output gain +/-24 dB

	{ ParamType::Choice,     "eqsc", "EQ Curve",   "",  0.0f,  3.0f,    1.0f, EQSC }, // SNE_EQSC EQ curve [NAB, IEC 15, AES]

	{ ParamType::Decibel,    "rclo", "EQ RecLo", "dB", -14.0f, 14.0f,    0.0f }, // SNE_RCLO rec EQ bass +/-10 dB
	{ ParamType::Decibel,    "rchi", "EQ RecHi", "dB", -14.0f, 14.0f,    0.0f }, // SNE_RCHI rec EQ high +/-14 dB
	{ ParamType::Decibel,    "rplo", "EQ RepLo", "dB", -10.0f, 10.0f,    0.0f }, // SNE_RPLO rep EQ bass +/-10 dB
	{ ParamType::Decibel,    "rphi", "EQ RepHi", "dB", -14.0f, 14.0f,    0.0f }, // SNE_RPHI rep EQ high +/-14 dB

	{ ParamType::Normalized, "head", "HB Freq",  "Hz",   0.0f, 1.0f,    0.00f }, // SNE_HEAD head bump frequency
	{ ParamType::Normalized, "bump", "HB Strn",  "dB",   0.0f, 1.0f,    0.00f }, // SNE_BUMP head bump magnitude

	{ ParamType::Decibel,    "hiss", "Hiss dB",  "dB", -96.0f, -36.0f, -66.0f }, // SNE_HISS hiss gain

	{ ParamType::Normalized, "room", "VU Ref",   "dB",   0.0f, 1.0f,   0.625f }, // SNE_ROOM VU meter headroom
	{ ParamType::Normalized, "hold", "VU Hold",  "n/y",  0.0f, 1.0f,    0.00f }, // SNE_HOLD VU meter peak hold
	{ ParamType::Normalized, "path", "VU Path",  "I/O",  0.0f, 1.0f,    1.00f }, // SNE_PATH VU meter path

	{ ParamType::Normalized, "attn", "HB Attn",  "dB",   0.0f, 1.0f,    1.00f }, // SNE_ATTN bump attenuator
	{ ParamType::Choice,     "nois", "Hiss On",    "",   0.0f, 1.0f,    1.00f, On }, // SNE_NOIS noise on/off
	{ ParamType::Choice,     "hbon", "HB On",      "",   0.0f, 1.0f,    1.00f, On }, // SNE_HBON head bump on/off
	{ ParamType::Normalized, "loon", "Anarchy",  "n/y",  0.0f, 1.0f,    0.00f }, // SNE_LOON force LO on
};

//------------------------------------------------------------------------------------

class SignalNoiseTapedeck : public SignalNoiseFX
{
private:
//vu meter
	std::atomic<float> vuLevel  { 0.0f };

//repro head
	foHPF	_rep0L;		// repro - LF roll-off
	foHPF	_rep0R;		// repro - LF roll-off
	biquad	_rep1L;		// repro - bump
	biquad	_rep1R;		// repro - bump
//record EQ
	foLSF	_pre1L;		// pre-emphasis LF
	foLSF	_pre1R;		// pre-emphasis LF
	foHSF	_pre2L;		// pre-emphasis HF
	foHSF	_pre2R;		// pre-emphasis HF
//repro EQ
	foLSF	_dee1L;		// de-emphasis LF
	foLSF	_dee1R;		// de-emphasis LF
	foHSF	_dee2L;		// de-emphasis HF
	foHSF	_dee2R;		// de-emphasis HF
//other
	noise	_rand;		// noise generator
private:
	void setupTapeheads();
	void setupEqualizer();

	template <typename Sample>
	void processImpl(juce::AudioBuffer<Sample>& buffer);

	int paramIdToIndex (const juce::String& id);

	inline float getParamValue (int idx) const noexcept
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return ptr->load();
	}

	inline int getParamChoice(int idx)
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return static_cast<int>(ptr->load());
	}

public:
//create & destroy
	SignalNoiseTapedeck();
	~SignalNoiseTapedeck();

	// JUCE overrides
	void prepareToPlay(double newSampleRate, int samplesPerBlock) override;
	void releaseResources() override {}

	void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

	const juce::String getName() const override { return JucePlugin_Name; }

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return false; }

	void parameterChanged(const juce::String& parameterID, float newValue) override;

//	float getInputLevel()  const noexcept { return inputLevel.load(); }
//	float getOutputLevel() const noexcept { return outputLevel.load(); }
};
