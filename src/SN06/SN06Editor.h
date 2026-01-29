#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
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
	SN06KnobPrecise trimKnob;
	SN06KnobPrecise gainKnob;
	SN06KnobPrecise volumeKnob;

	juce::Label volumeLabel;
	juce::Label gainLabel;
	juce::Label trimLabel;

	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trimAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

	SN06PeakMeter inputMeter;
	SN06PeakMeter outputMeter;
	SN06PeakLed peakLed;

	void setupKnobLabel(SN06KnobPrecise& knob, juce::Label& label,
		const char* paramID, float scale, float offset);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SN06Editor)
};
