#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include "ParameterInfo.h"

class ParameterLabel : public juce::Component,
                       public juce::AudioProcessorValueTreeState::Listener
{
public:
	ParameterLabel(juce::AudioProcessorValueTreeState&, const ParameterInfo&);
	~ParameterLabel() = default;

	void updateFromParameter();
	juce::Label& getLabel() { return label; };


private:
    juce::Label label;
	void configureLabel();
	void setupCallbacks();

	juce::AudioProcessorValueTreeState& params;
	const ParameterInfo& info;

	// Listener callback from AudioProcessorValueTreeState
	void parameterChanged(const juce::String& paramID, float newValue) override;
};
