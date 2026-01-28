#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <SN06PeakMeter.h>
#include <SN06PeakLed.h>

class SN06Processor;

class SN06Editor : public juce::AudioProcessorEditor,
                   private juce::Timer
{
public:
    explicit SN06Editor (SN06Processor&);
    ~SN06Editor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    SN06Processor& processor;
    void timerCallback() override;

    juce::Image background;

    juce::Slider gainKnob;
    juce::Slider trimKnob;
    juce::Slider volumeKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> gainAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> trimAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeAttachment;

    SN06PeakMeter inputMeter;
    SN06PeakMeter outputMeter;
    SN06PeakLed peakLed;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SN06Editor)
};
