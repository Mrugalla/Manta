#pragma once
#include "Utils.h"
#include "Layout.h"

namespace gui
{
    struct Shader :
        juce::ImageEffectFilter,
        Timer
    {
        Shader(Utils&, Component&);

        void applyEffect(Image&, Graphics&, float, float) override;

        Component& comp;
        Utils& utils;
        Evt notify;
        bool bypassed;

        void paintBypassed(Image&, Graphics&);

        void timerCallback() override;
    };
}