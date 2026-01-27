#include <juce_audio_processors/juce_audio_processors.h>
#include "SN06Editor.h"
#include "SN06Processor.h"
#include "BinaryData.h"

SN06Editor::SN06Editor (SN06Processor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Load background image
    background = juce::ImageCache::getFromMemory(
	BinaryData::sn06g_bk_png,
	BinaryData::sn06g_bk_pngSize
    );

    setSize (background.getWidth(), background.getHeight());
}

void SN06Editor::paint (juce::Graphics& g)
{
    g.drawImageAt (background, 0, 0);
}
