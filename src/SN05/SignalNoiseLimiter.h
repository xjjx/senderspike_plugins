//------------------------------------------------------------------------------------
//
//	file:		sn_05e.h
//
//	purpose:	SN05 limiter effect
//
//  authors:	2019 - 2021 Oto Spál
//				uses c++ adaptation of original code by (c) 2011 M. Holters
//
//------------------------------------------------------------------------------------


#pragma once


//------------------------------------------------------------------------------------

#include <atomic>
#include <juce_audio_processors/juce_audio_processors.h>
#include <sn_core.h>
#include "SignalNoiseFX.h"

//------------------------------------------------------------------------------------
// effect
//------------------------------------------------------------------------------------

enum
{
	SNE_GAIN = 0,	// input gain
	SNE_CEIL,		// ceiling
	SNE_ATKH,		// attack - Holters' lim
	SNE_RELH,		// release - Holters' lim
	SNE_RELS,		// release - brickwall
	SNE_MODE,		// mode (clip/limit)
	SNE_HPON,		// high pass on/off
	SNE_HPFC,		// high pass Fc
	SNE_CLIP,		// clipper % (0% = true bypass)
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------
static const std::vector<const char*> LC = { "Limiter", "Clipper" };
static const std::vector<const char*> On = { "Off", "On" };
static const ParamDesc gParams[] =
{
    { ParamType::Decibel,    "gain", "Gain",    "dB",  0.0f,  24.0f, 0.00f }, // SNE_GAIN input gain
    { ParamType::Decibel,    "ceil", "Ceiling", "dB", -24.0f,  0.0f, 0.00f }, // SNE_CEIL ceiling
    { ParamType::Normalized, "atkh", "AT",      "ms",  0.0f,   1.0f, 0.00f }, // SNE_ATKH attack - Holters' limiter
    { ParamType::Normalized, "relh", "R1",      "ms",  0.0f,   1.0f, 1.00f }, // SNE_RELH release - Holters' limiter
    { ParamType::Normalized, "rels", "R2",      "ms",  0.0f,   1.0f, 1.00f }, // SNE_RELS release - brickwall
    { ParamType::Choice,     "mode", "Mode",    "", 0.0f,   1.0f, 0.00f, LC }, // SNE_MODE mode (limit/clip)
    { ParamType::Choice,     "hpon", "HP On",   "", 0.0f,   1.0f, 0.00f, On }, // SNE_HPON high pass on/off
    { ParamType::Normalized, "hpfc", "HP Freq", "Hz",  0.0f,   1.0f, 0.50f }, // SNE_HPFC high pass cutoff
    { ParamType::Percent,    "clip", "SC",      "%",   0.0f, 100.0f, 0.00f }, // SNE_CLIP clipper % (0% = true bypass)
};

//------------------------------------------------------------------------------------

#define SN05_VER		2210
#ifdef SN05G
#define SN05_NAM		"SN05-G Limiter"
#else
#define SN05_NAM		"SN05 Limiter"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseLimiter : public SignalNoiseFX
{
private:
	//Holters' limiter
	double	_dlL[5];	// delay line L
	double	_dlR[5];	// delay line R
	double	_max;		// peak
	double	_grH;		// gain reduction
	double	_atH;		// attack coefficient
	double	_rlH;		// release coefficient
	//brickwall
	double	_env;		// envelope feedback
	double	_atk;		// attack coefficient
	double	_rls;		// release coefficient
	//SC filter
	biquad	_hL1;		// SC HPF 2nd order L
	biquad	_hR1;		// SC HPF 2nd order R
	foHPF	_hL2;		// SC HPF 1st order L
	foHPF	_hR2;		// SC HPF 1st order R
private:
	void setupLimiter();
	void setupClipper();
	void setupSidechain();

	template <typename Sample>
	void processImpl(juce::AudioBuffer<Sample>& buffer);

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

	// ================= METERS =================
	std::atomic<float> gainReduction  { 0.0f };

public:
	SignalNoiseLimiter();
	~SignalNoiseLimiter() override = default;

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
