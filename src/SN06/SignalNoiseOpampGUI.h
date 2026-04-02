#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SignalNoisePeakMeter.h"
#include "SignalNoisePeakLed.h"
#include "SignalNoiseKnobLookAndFeel.h"
#include "SignalNoiseKnobPrecise.h"
#include "SignalNoiseKnobLabel.h"
#include "SignalNoiseOpamp.h"

class SignalNoiseOpampGUI : public juce::AudioProcessorEditor,
                               private juce::Timer
{
public:
	explicit SignalNoiseOpampGUI (SignalNoiseOpamp&);
	~SignalNoiseOpampGUI() override;

	void paint (juce::Graphics&) override;
	void resized() override;

private:
	SignalNoiseOpamp& processor;
	void timerCallback() override;

	juce::Image background;

	SignalNoiseKnobLookAndFeel largeLNF;
	SignalNoiseKnobLookAndFeel screwLNF;

	std::unique_ptr<SignalNoiseKnobPrecise> gainKnob;
	std::unique_ptr<SignalNoiseKnobPrecise> trimKnob;
	std::unique_ptr<SignalNoiseKnobPrecise> volumeKnob;

    SignalNoiseKnobLabel gainLabel;
    SignalNoiseKnobLabel trimLabel;
    SignalNoiseKnobLabel volumeLabel;

	std::unique_ptr<SignalNoisePeakMeter> inputMeter;
	std::unique_ptr<SignalNoisePeakMeter> outputMeter;
	SignalNoisePeakLed peakLed;

	std::unique_ptr<SignalNoiseKnobPrecise> setupKnobAndLabel(
		const ParamDesc& p,
		juce::LookAndFeel* lnF,
		SignalNoiseKnobLabel& label
	);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SignalNoiseOpampGUI)
};
