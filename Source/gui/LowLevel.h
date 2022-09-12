#pragma once
#include "Knob.h"
#include "SplineEditor.h"

namespace gui
{
    struct LowLevel :
        public Comp
    {
        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
			bpCutoff(u),
			bpQ(u),
            feedback(u),
            damp(u),
            oct(u),
            semi(u),
            fine(u)
        {
            makeParameter(bpCutoff, PID::BandpassCutoff, "BP Cutoff");
			makeParameter(bpQ, PID::BandpassQ, "BP Q");

            makeParameter(feedback, PID::ResonatorFeedback, "Feedback");
            makeParameter(damp, PID::ResonatorDamp, "Damp");
			makeParameter(oct, PID::ResonatorOct, "Oct");
			makeParameter(semi, PID::ResonatorSemi, "Semi");
			makeParameter(fine, PID::ResonatorFine, "Fine");

            layout.init
            (
                { 3, 5, 5, 5, 5, 5, 3 },
                { 3, 1, 5, 1, 5, 1, 3 }
            );

            addAndMakeVisible(bpCutoff);
			addAndMakeVisible(bpQ);
            addAndMakeVisible(feedback);
			addAndMakeVisible(damp);
			addAndMakeVisible(oct);
			addAndMakeVisible(semi);
            addAndMakeVisible(fine);
        }

        void paint(Graphics& g) override
        {
            auto thicc = utils.thicc;
            auto thicc2 = thicc * 2.f;
            const Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

            g.setColour(Colours::c(ColourID::Txt));

            const auto bpArea = layout(1, 1, 5, 2);
            g.drawFittedText("BP", bpArea.toNearestInt(), Just::centredTop, 1);
            drawRectEdges(g, bpArea, thicc2, stroke);

            const auto resonatorArea = layout(1, 3, 5, 2);
            g.drawFittedText("Resonator", resonatorArea.toNearestInt(), Just::centredTop, 1);
            drawRectEdges(g, resonatorArea, thicc2, stroke);
        }

        void resized() override
        {
            layout.resized();

            layout.place(bpCutoff, 1, 2, 1, 1, false);
			layout.place(bpQ, 2, 2, 1, 1, false);

            layout.place(feedback, 1, 4, 1, 1, false);
            layout.place(damp, 2, 4, 1, 1, false);
			layout.place(oct, 3, 4, 1, 1, false);
			layout.place(semi, 4, 4, 1, 1, false);
			layout.place(fine, 5, 4, 1, 1, false);
        }

    protected:
        Knob bpCutoff, bpQ;
        Knob feedback, damp, oct, semi, fine;
    };
}