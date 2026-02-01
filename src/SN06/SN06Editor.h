#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "SNPeakMeter.h"
#include "SNPeakLed.h"
#include "ParameterInfo.h"
#include "SNKnobLookAndFeel.h"
#include "SNKnobPrecise.h"

class SN06Processor;

class SN06Editor : public juce::AudioProcessorEditor,
				   private juce::Timer
{
public:
	explicit SN06Editor (SN06Processor&);
	~SN06Editor() override;

	void paint (juce::Graphics&) override;
	void resized() override;

private:
	SN06Processor& processor;
	void timerCallback() override;

	juce::Image background;

	SNKnobLookAndFeel largeLNF;
	SNKnobLookAndFeel screwLNF;

	std::unique_ptr<SNKnobPrecise> gainKnob;
	std::unique_ptr<SNKnobPrecise> trimKnob;
	std::unique_ptr<SNKnobPrecise> volumeKnob;

    juce::Label gainLabel;
    juce::Label trimLabel;
    juce::Label volumeLabel;

	SNPeakMeter inputMeter;
	SNPeakMeter outputMeter;
	SNPeakLed peakLed;

	std::unique_ptr<SNKnobPrecise> setupKnobAndLabel(
		const ParameterInfo& info,
		juce::LookAndFeel* lnF,
		juce::Label& label
	);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SN06Editor)
};
