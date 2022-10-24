#pragma once

#include "BinaryData.h"

#include <juce_core/juce_core.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_extra/juce_gui_extra.h>

#include "../Processor.h"
#include "../param/Param.h"
#include "../arch/State.h"
#include "../audio/MIDIManager.h"

#include <array>
#include <functional>

namespace gui
{
    using Colour = juce::Colour;
    using Gradient = juce::ColourGradient;
    using String = juce::String;
    using Font = juce::Font;
    using Props = juce::PropertiesFile;
    using AppProps = juce::ApplicationProperties;
    using Cursor = juce::MouseCursor;
    using Image = juce::Image;
    using Graphics = juce::Graphics;
    using Mouse = juce::MouseEvent;
    using MouseWheel = juce::MouseWheelDetails;
    using Graphics = juce::Graphics;
    using Just = juce::Justification;
    using Timer = juce::Timer;
    using Path = juce::Path;
    using Point = juce::Point<int>;
    using PointF = juce::Point<float>;
    using Bounds = juce::Rectangle<int>;
    using BoundsF = juce::Rectangle<float>;
    using Line = juce::Line<int>;
    using LineF = juce::Line<float>;
    using Image = juce::Image;
    using Stroke = juce::PathStrokeType;
    using Affine = juce::AffineTransform;
    using Random = juce::Random;
    using KeyPress = juce::KeyPress;
    using ValueTree = juce::ValueTree;
    using File = juce::File;
    using PropertiesFile = juce::PropertiesFile;
    using RangedDirectoryIterator = juce::RangedDirectoryIterator;
    using Identifier = juce::Identifier;
    using Drawable = juce::Drawable;
    using UniqueDrawable = std::unique_ptr<Drawable>;
    using RangeF = juce::NormalisableRange<float>;
    using Time = juce::Time;
    using FileChooser = juce::FileChooser;
    using Component = juce::Component;
    using SystemClipboard = juce::SystemClipboard;
    using SIMD = juce::FloatVectorOperations;
    
    using Processor = audio::Processor;
    
    using PID = param::PID;
    using Param = param::Param;
    using Params = param::Params;
    using MIDIVoicesArray = audio::MIDIVoicesArray;

    static constexpr float Tau = 6.28318530718f;
    static constexpr float Pi = Tau * .5f;;
    static constexpr float PiHalf = Tau * .25f;
    static constexpr float PiQuart = Tau * .125f;
}