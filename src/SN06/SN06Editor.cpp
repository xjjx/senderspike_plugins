#include <juce_audio_processors/juce_audio_processors.h>
#include "SN06Editor.h"
#include "SN06Processor.h"
#include "BinaryData.h"

SN06Editor::SN06Editor(SN06Processor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    // Load background image
    background = juce::ImageCache::getFromMemory(
        BinaryData::sn06g_bk_png,
        BinaryData::sn06g_bk_pngSize
    );

    // Set initial size based on background
    setSize(background.getWidth(), background.getHeight());

    // Knob styles
    for (auto* knob : { &gainKnob, &trimKnob, &volumeKnob })
    {
        knob->setSliderStyle(juce::Slider::RotaryVerticalDrag);
        knob->setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(*knob);
    }

    // Attachments (THIS replaces all manual setValue calls)
    auto& params = processor.getParameters();

    gainAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        params, "gain", gainKnob);
    trimAttachment   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        params, "trim", trimKnob);
    volumeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                        params, "volume", volumeKnob);

    addAndMakeVisible(inputMeter);
    addAndMakeVisible(outputMeter);
    addAndMakeVisible(peakLed);

    startTimerHz(30); // GUI refresh rate
}

//---------------------------------------------------------
void SN06Editor::paint(juce::Graphics& g)
{
    // Fill background with black if image fails
    g.fillAll(juce::Colours::black);

    if (background.isValid())
        g.drawImage(background, getLocalBounds().toFloat(), juce::RectanglePlacement::stretchToFit);
}

//---------------------------------------------------------
void SN06Editor::resized()
{
    // Scale knobs relative to editor size
//    float scaleX = (float)getWidth() / background.getWidth();
//    float scaleY = (float)getHeight() / background.getHeight();

    // Temporary positions (we’ll match original layout later)
    gainKnob.setBounds(80, 100, 80, 80);
    trimKnob.setBounds (36, 28, 48, 48);
    volumeKnob.setBounds(80, 230, 80, 80);

    inputMeter.setBounds (112, 42, 100, 5);
    outputMeter.setBounds (112, 52, 100, 5);
    peakLed.setBounds (180, 307, 10, 10);
}

void SN06Editor::timerCallback()
{
    inputMeter .setLevel(processor.getInputLevel());
    outputMeter.setLevel(processor.getOutputLevel());
    peakLed.setLevel(processor.getOutputLevel());

    repaint();
}
