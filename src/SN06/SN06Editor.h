#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "ParameterLabel.h"
#include <SN06PeakMeter.h>
#include <SN06PeakLed.h>
#include "SN06KnobLookAndFeel.h"
#include "SN06KnobPrecise.h"

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

	SN06KnobLookAndFeel largeLNF;
	SN06KnobLookAndFeel screwLNF;

	std::unique_ptr<SN06KnobPrecise> gainKnob;
	std::unique_ptr<SN06KnobPrecise> trimKnob;
	std::unique_ptr<SN06KnobPrecise> volumeKnob;

	std::unique_ptr<ParameterLabel> gainLabel;
	std::unique_ptr<ParameterLabel> trimLabel;
	std::unique_ptr<ParameterLabel> volumeLabel;

	SN06PeakMeter inputMeter;
	SN06PeakMeter outputMeter;
	SN06PeakLed peakLed;

	std::unique_ptr<SN06KnobPrecise> setupKnobAndLabel(
		const ParameterInfo& info,
		juce::LookAndFeel* lnF,
		ParameterLabel& label
	);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SN06Editor)
};
