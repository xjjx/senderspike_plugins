//------------------------------------------------------------------------------------
//
//	file:		sn_01e.h
//
//	purpose:	SN01 compressor effect
//
//  authors:	2019 - 2026 Oto Spál
//				based on (c) 2012 D. Giannoulis, M. Massberg, and Joshua D. Reiss
//				'Digital Dynamic Range Compressor Design—A Tutorial and Analysis'
//				original paper @ https://www.eecs.qmul.ac.uk/~josh/documents/2012/
//				GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
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
	SNE_TRSH = 0,	// threshold
	SNE_FUNC,		// ratio
	SNE_GAIN,		// make-up
	SNE_ATTK,		// attack
	SNE_RELS,		// release
	SNE_KWDT,		// knee width
	SNE_KNEE,		// knee strength
	SNE_MODE,		// SC mode
	SNE_PUSH,		// 'thrust'
	SNE_COMP,		// dry/wet
	SNE_FBCK,		// FF/FB switch
	SNE_SIZE,		// num of params
};

//------------------------------------------------------------------------------------

static const std::vector<const char*> MODE = { "Flat", "Type A", "Type B" };
static const std::vector<const char*> FBCK = { "FeedForward", "FeedBack" };
static const std::vector<const char*> PUSH = { "0", "1", "2" };
static const ParamDesc gParams[] =
{
    { ParamType::Decibel, "thrs",  "Thresh", "dB", -40.0f,    0.0f, -20.0f }, // SNE_TRSH threshold
    { ParamType::Ratio,  "ratio",   "Ratio",   "",   0.0f,    1.0f,  0.50f }, // SNE_FUNC ratio
    { ParamType::Decibel, "gain",    "Gain", "dB",   0.0f,   24.0f,  0.00f }, // SNE_GAIN make-up gain

    { ParamType::Cubic,   "attk",  "Attack", "ms",  0.03f,   30.0f,  1.61f }, // SNE_ATTK attack
    { ParamType::Cubic,   "rels", "Release", "ms",  50.0f, 2000.0f, 293.75f }, // SNE_RELS release

    { ParamType::Decibel, "kwdt", "Knee dB", "dB",   0.0f,   24.0f,  9.00f }, // SNE_KWDT knee width
    { ParamType::Percent, "knee",  "Knee %",  "%",   0.0f,  100.0f,  0.00f }, // SNE_KNEE knee strength

    { ParamType::Choice,  "mode", "SC Mode",   "",   0.0f,    1.0f,  0.00f, MODE }, // SNE_MODE sidechain mode
    { ParamType::Choice,  "push",    "Push",   "",   0.0f,    1.0f,  0.00f, PUSH }, // SNE_PUSH thrust

    { ParamType::Percent, "comp", "Wet/Dry",  "%",   0.0f,  100.0f,  0.00f }, // SNE_COMP dry/wet
    { ParamType::Choice,  "fbck",    "Mode",   "",   0.0f,    1.0f,  0.00f, FBCK }, // SNE_FBCK feed-forward / feed-back
};

//------------------------------------------------------------------------------------

class SignalNoiseCompressor : public SignalNoiseFX
{
private:
	double	_TdB;	// envelope filter
	double	_atk;	// attack coefficient
	double	_rls;	// release coefficient
	double	_fbL;	// feedback line L
	double	_fbR;	// feedback line R
	noise	_rnd;	// noise generator
	biquad	_lsL;	// SC low shelf L
	biquad	_hsL;	// SC high shelf L
	biquad	_lsR;	// SC low shelf R
	biquad	_hsR;	// SC high shelf R
private:
	void setupEnvelope();
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

public:
	SignalNoiseCompressor();
	~SignalNoiseCompressor();

	// JUCE overrides
	void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	void processBlock (juce::AudioBuffer<double>&, juce::MidiBuffer&) override;

	const juce::String getName() const override { return JucePlugin_Name; }

	void prepareToPlay(double newSampleRate, int samplesPerBlock) override;
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return true; }

//	float getInputLevel()  const noexcept { return inputLevel.load(); }
//	float getOutputLevel() const noexcept { return outputLevel.load(); }
};
