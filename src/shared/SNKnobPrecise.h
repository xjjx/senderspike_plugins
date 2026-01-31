#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ParameterInfo.h"

class SNKnobPrecise : public juce::Slider
{
public:
	explicit SNKnobPrecise (const ParameterInfo& i);
	void mouseDown (const juce::MouseEvent&) override;

	float normalizedToDb (float norm) { return info.normalizedToDb(norm); };
	float dbToNormalized (float db) { return info.dbToNormalized(db); };

	double snapDb(double db);

	void attachToParameter(juce::AudioProcessorValueTreeState& params) {
		attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
			params, info.paramID, *this
		);
	}

private:
	ParameterInfo info;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
	double snapValue(double attemptedValue, DragMode) override;
};
