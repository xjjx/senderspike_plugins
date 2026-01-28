#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include "sn_core.h"

class SN06PeakMeter : public juce::Component
{
public:
    SN06PeakMeter()
    {
        // Load bitmap once
        ledImage = juce::ImageCache::getFromMemory(
            BinaryData::sn06g_vu_png,
            BinaryData::sn06g_vu_pngSize
        );
    }

    void setLevel(float val)
    {
        // clamp 0..1
        val = juce::jlimit(0.0f, 1.0f, val);
        if (val > level)
            level = val;  // only increase
        repaint();
    }

    void setRange(float dB) { range = dB; }

    void paint(juce::Graphics& g) override
    {
        if (!ledImage.isValid())
            return;

        level = 1.0f;

        auto boundsF = getLocalBounds().toFloat();
        int imgW = ledImage.getWidth();
        int imgH = ledImage.getHeight();

        // draw unlit (bottom half of image)
        juce::Rectangle<float> srcUnlit(0.0f, imgH * 0.5f, imgW, imgH * 0.5f);
        g.drawImageWithin(
                ledImage,                 // image
                0, 0, boundsF.getWidth(), boundsF.getHeight(),
                juce::RectanglePlacement::stretchToFit,
                true                      // fill alpha
        );

        // draw value (top half of image)
        float dB = lin2dB(level);
        dB = juce::jlimit(-range, 0.0f, dB);         // clamp
        float norm = (dB + range) / range;           // normalize 0..1
        float fillWidth = boundsF.getWidth() * norm;

        juce::Rectangle<float> target(boundsF.getX(), boundsF.getY(),
                                      fillWidth, boundsF.getHeight());

        juce::Rectangle<float> srcLit(0.0f, 0.0f, imgW * norm, imgH * 0.5f);
        g.drawImageWithin(
                ledImage,                 // image
                0, 0, boundsF.getWidth(), boundsF.getHeight(),
                juce::RectanglePlacement::stretchToFit,
                true                      // fill alpha
        );

        // decay like original
        level *= 0.95f;
    }

private:
    juce::Image ledImage;
    float level = 0.0f; // linear 0..1
    float range = 72.0f;
};
;
