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

static const ParamDesc gParams[] =
{
	{ "trim",   "Input",     "dB",  0.50f }, // SNE_TRIM   input trim +/-24 dB
	{ "gain",   "Output",    "dB",  0.50f }, // SNE_GAIN   output gain +/-24 dB

	{ "eqsc",   "EQ Curve",  "Typ", 0.50f }, // SNE_EQSC   EQ curve [NAB, IEC 15, AES]

	{ "rclo",   "EQ RecLo",  "dB",  0.50f }, // SNE_RCLO   rec EQ bass +/-10 dB
	{ "rchi",   "EQ RecHi",  "dB",  0.50f }, // SNE_RCHI   rec EQ high +/-14 dB
	{ "rplo",   "EQ RepLo",  "dB",  0.50f }, // SNE_RPLO   rep EQ bass +/-10 dB
	{ "rphi",   "EQ RepHi",  "dB",  0.50f }, // SNE_RPHI   rep EQ high +/-14 dB

	{ "head",   "HB Freq",   "Hz",  0.00f }, // SNE_HEAD   head bump frequency
	{ "bump",   "HB Strn",   "dB",  0.00f }, // SNE_BUMP   head bump magnitude

	{ "hiss",   "Hiss dB",   "dB",  0.50f }, // SNE_HISS   hiss gain

	{ "room",   "VU Ref",    "dB",  0.625f }, // SNE_ROOM  VU meter headroom
	{ "hold",   "VU Hold",   "n/y", 0.00f },  // SNE_HOLD  VU meter peak hold
	{ "path",   "VU Path",   "I/O", 1.00f },  // SNE_PATH  VU meter path

	{ "attn",   "HB Attn",   "dB",  1.00f }, // SNE_ATTN   bump attenuator
	{ "nois",   "Hiss On",   "n/y", 1.00f }, // SNE_NOIS   noise on/off
	{ "hbon",   "HB On",     "n/y", 1.00f }, // SNE_HBON   head bump on/off
	{ "loon",   "Anarchy",   "n/y", 0.00f }, // SNE_LOON   force LO on
};

//------------------------------------------------------------------------------------

#define SN03_VER		1310
#ifdef SN03G
#define SN03_NAM		"SN03-G Tape Recorder"
#else
#define SN03_NAM		"SN03 Tape Recorder"
#endif

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

	inline float getParamNorm (int idx) const noexcept
	{
		auto ptr = getParameters().getRawParameterValue(gParams[idx].id);
		return ptr->load();
	}

public:
//create & destroy
	SignalNoiseTapedeck();
	~SignalNoiseTapedeck() override = default;

	// JUCE overrides
	void prepareToPlay(double newSampleRate, int samplesPerBlock) override;
	void releaseResources() override {}

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
