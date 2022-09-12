#pragma once
#include "Knob.h"
#include "SplineEditor.h"

namespace gui
{
    struct LowLevel :
        public Comp
    {
        static constexpr int NumLanes = 3;

        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
			enabled{Knob(u), Knob(u), Knob(u) },
			frequency{ Knob(u), Knob(u), Knob(u) },
			resonance{ Knob(u), Knob(u), Knob(u) },
			slope{ Knob(u), Knob(u), Knob(u) },
			drive{ Knob(u), Knob(u), Knob(u) },
			delay{ Knob(u), Knob(u), Knob(u) },
			gain{ Knob(u), Knob(u), Knob(u) }
        {
            for (auto i = 0; i < NumLanes; ++i)
            {
                const auto offset = i * 7;

                makeParameter(enabled[i], param::offset(PID::Lane1Enabled, offset), "Enabled");
                makeParameter(frequency[i], param::offset(PID::Lane1Frequency, offset), "Frequency");
				makeParameter(resonance[i], param::offset(PID::Lane1Resonance, offset), "Resonance");
				makeParameter(slope[i], param::offset(PID::Lane1Slope, offset), "Slope");
				makeParameter(drive[i], param::offset(PID::Lane1Drive, offset), "Drive");
				makeParameter(delay[i], param::offset(PID::Lane1Delay, offset), "Delay");
				makeParameter(gain[i], param::offset(PID::Lane1Gain, offset), "Gain");

                addAndMakeVisible(enabled[i]);
				addAndMakeVisible(frequency[i]);
				addAndMakeVisible(resonance[i]);
				addAndMakeVisible(slope[i]);
				addAndMakeVisible(drive[i]);
				addAndMakeVisible(delay[i]);
				addAndMakeVisible(gain[i]);
            }

            layout.init
            (
                { 3, 5, 5, 5, 5, 5, 5, 5, 3 },
                { 3, 2, 5, 2, 5, 2, 5, 3 }
            );
        }

        void paint(Graphics& g) override
        {
            auto thicc = utils.thicc;
            auto thicc2 = thicc * 2.f;
            const Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

            g.setColour(Colours::c(ColourID::Txt));
            
            {
                const auto laneArea = layout(1, 1, 7, 2);
                g.drawFittedText("Lane 1", laneArea.toNearestInt(), Just::centredTop, 1);
                drawRectEdges(g, laneArea, thicc2, stroke);
            }
            {
                const auto laneArea = layout(1, 3, 7, 2);
                g.drawFittedText("Lane 2", laneArea.toNearestInt(), Just::centredTop, 1);
                drawRectEdges(g, laneArea, thicc2, stroke);
            }
			{
				const auto laneArea = layout(1, 5, 7, 2);
				g.drawFittedText("Lane 3", laneArea.toNearestInt(), Just::centredTop, 1);
				drawRectEdges(g, laneArea, thicc2, stroke);
			}
        }

        void resized() override
        {
            layout.resized();
            for (auto i = 0; i < NumLanes; ++i)
            {
                auto y = 2 + i * 2;

				layout.place(enabled[i], 1, y, 1, 1, false);
				layout.place(frequency[i], 2, y, 1, 1, false);
				layout.place(resonance[i], 3, y, 1, 1, false);
				layout.place(slope[i], 4, y, 1, 1, false);
				layout.place(drive[i], 5, y, 1, 1, false);
				layout.place(delay[i], 6, y, 1, 1, false);
				layout.place(gain[i], 7, y, 1, 1, false);
            }
        }

    protected:
        std::array<Knob, NumLanes> enabled, frequency, resonance, slope, drive, delay, gain;
    };
}