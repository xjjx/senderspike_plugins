#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class ParameterLabel : public juce::Component,
                       public juce::AudioProcessorValueTreeState::Listener
{
public:
	ParameterLabel(juce::AudioProcessorValueTreeState&, const juce::String&);
	~ParameterLabel() = default;

	void updateFromParameter();
	juce::Label& getLabel() { return label; };


private:
	juce::AudioProcessorValueTreeState& params;
	juce::String paramId;

    juce::Label label;
	void configureLabel();
	void setupCallbacks();

	// Listener callback from AudioProcessorValueTreeState
	void parameterChanged(const juce::String& paramID, float newValue) override;
};
