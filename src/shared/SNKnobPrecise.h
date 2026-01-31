#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class SNKnobPrecise : public juce::Slider
{
public:
	explicit SNKnobPrecise (float defaultDb);
	void mouseDown (const juce::MouseEvent&) override;

	void attachToParameter(juce::AudioProcessorValueTreeState& params, const juce::String& paramId) {
		attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
			params, paramId, *this
		);
	}

private:
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
	double snapValue(double attemptedValue, DragMode) override;
};
