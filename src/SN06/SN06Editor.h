#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class SN06Processor;

class SN06Editor : public juce::AudioProcessorEditor
{
public:
    explicit SN06Editor (SN06Processor&);
    ~SN06Editor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override {}

private:
    SN06Processor& processor;
    juce::Image background;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SN06Editor)
};
