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
#include "ParameterInfo.h"

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

static const ParamDesc gParams[] =
{
    { "thrs", "Thresh",   "dB",  0.50f },  // SNE_TRSH  threshold
    { "ratio","Ratio",    "num", 0.50f },  // SNE_FUNC  ratio
    { "gain", "Gain",     "dB",  0.00f },  // SNE_GAIN  make-up gain

    { "attk", "Attack",   "ms",  0.375f }, // SNE_ATTK  attack
    { "rels", "Release",  "ms",  0.50f },  // SNE_RELS  release

    { "kwdt", "Knee dB",  "dB",  0.375f }, // SNE_KWDT  knee width
    { "knee", "Knee %",   "%",   0.00f },  // SNE_KNEE  knee strength

    { "mode", "SC Mode",  "typ", 0.00f },  // SNE_MODE  sidechain mode
    { "push", "Push",     "typ", 0.00f },  // SNE_PUSH  thrust

    { "comp", "Wet/Dry",  "%",   0.00f },  // SNE_COMP  dry/wet
    { "fbck", "F.Back",   "n/y", 0.00f },  // SNE_FBCK  feed-forward / feed-back
};

//------------------------------------------------------------------------------------

#define SN01_VER		1210

#ifdef SN01G
#define SN01_NAM		"SN01-G Compressor"
#else
#define SN01_NAM		"SN01 Compressor"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseCompressor : public juce::AudioProcessor,
                              public juce::AudioProcessorValueTreeState::Listener
{
private:
	double sampleRate = 44100.0;
    juce::AudioProcessorValueTreeState parameters;

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
	void processImpl(Sample** in, Sample** out, int numSamples);

	static juce::AudioProcessorValueTreeState::ParameterLayout
	createParameterLayout();

	int paramIdToIndex (const juce::String& id);

	inline float getParamNorm (int idx) const noexcept
	{
		return *parameters.getRawParameterValue (gParams[idx].id);
	}

public:
	SignalNoiseCompressor();
	~SignalNoiseCompressor() override = default;

	// JUCE overrides
	void prepareToPlay(double newSampleRate, int samplesPerBlock) override;
	void releaseResources() override {}

	void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&) override;
	void processBlock(juce::AudioBuffer<double>& buffer, juce::MidiBuffer&) override;

	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override { return false; }
	juce::AudioProcessorValueTreeState& getParameters() { return parameters; }

	const juce::String getName() const override { return "SignalNoiseEqualizer"; }
	bool acceptsMidi() const override { return false; }
	bool producesMidi() const override { return false; }
	double getTailLengthSeconds() const override { return 0.0; }

	int getNumPrograms() override { return 1; }
	int getCurrentProgram() override { return 0; }
	void setCurrentProgram(int) override {}
	const juce::String getProgramName(int) override { return {}; }
	void changeProgramName(int, const juce::String&) override {}
	bool isBusesLayoutSupported(const juce::AudioProcessor::BusesLayout& layouts) const override;
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	void getStateInformation(juce::MemoryBlock&) override;
	void setStateInformation(const void*, int) override;

//	float getInputLevel()  const noexcept { return inputLevel.load(); }
//	float getOutputLevel() const noexcept { return outputLevel.load(); }
};
