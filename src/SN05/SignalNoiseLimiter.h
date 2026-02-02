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

struct ParamDesc
{
	const char* id;			  // JUCE parameter ID
	const char* name;		  // display name
	const char* unit;		  // "dB", "Hz", "n/y", etc.
	float defaultNorm;		  // normalized [0..1]
};

static const ParamDesc gParams[] =
{
    { "gain",   "Gain",      "dB",  0.00f }, // SNE_GAIN      input gain
    { "ceil",   "Ceiling",   "dB",  1.00f }, // SNE_CEIL      ceiling
    { "atkh",   "AT",        "ms",  0.00f }, // SNE_ATKH      attack - Holters' limiter
    { "relh",   "R1",        "ms",  1.00f }, // SNE_RELH      release - Holters' limiter
    { "rels",   "R2",        "ms",  1.00f }, // SNE_RELS      release - brickwall
    { "mode",   "Mode",      "L/C", 0.00f }, // SNE_MODE      mode (clip/limit)
    { "hpon",   "HP On",     "y/n", 0.00f }, // SNE_HPON      high pass on/off
    { "hpfc",   "HP Freq",   "Hz",  0.50f }, // SNE_HPFC      high pass cutoff
    { "clip",   "SC",        "%",   0.00f }, // SNE_CLIP      clipper % (0% = true bypass)
};

//------------------------------------------------------------------------------------

#define SN05_VER		2210
#ifdef SN05G
#define SN05_NAM		"SN05-G Limiter"
#else
#define SN05_NAM		"SN05 Limiter"
#endif

//------------------------------------------------------------------------------------

class SignalNoiseLimiter : public juce::AudioProcessor,
                           public juce::AudioProcessorValueTreeState::Listener
{
private:
	double sampleRate = 44100.0;
    juce::AudioProcessorValueTreeState parameters;

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
	void processImpl(Sample** in, Sample** out, int numSamples);

	static juce::AudioProcessorValueTreeState::ParameterLayout
	createParameterLayout();

	int paramIdToIndex (const juce::String& id);

	inline float getParamNorm (int idx) const noexcept
	{
		return *parameters.getRawParameterValue (gParams[idx].id);
	}

	// ================= METERS =================
	std::atomic<float> gainReduction  { 0.0f };

public:
	SignalNoiseLimiter();
	~SignalNoiseLimiter() override = default;

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
