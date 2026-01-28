#include "BinaryData.h"

class SN06PeakLed : public juce::Component
{
public:
    SN06PeakLed()
    {
        ledImage = juce::ImageCache::getFromMemory(
            BinaryData::sn04g_pk_png, BinaryData::sn04g_pk_pngSize);
    }

    void setLevel(float newLevel)
    {
        peakLevel = newLevel;
        repaint();
    }

    void paint(juce::Graphics& g) override
    {
        if (!ledImage.isValid())
            return;

        // LED frames are stacked vertically
        int frameHeight = ledImage.getHeight() / 2;
        int yOffset = (peakLevel > 0.9f) ? 0 : frameHeight;

        juce::Image frame = ledImage.getClippedImage(juce::Rectangle<int>(0, yOffset, ledImage.getWidth(), frameHeight));

        // Draw the frame scaled to component bounds
        g.drawImage(frame, getLocalBounds().toFloat());
    }

private:
    juce::Image ledImage;
    float peakLevel = 0.0f;
};

