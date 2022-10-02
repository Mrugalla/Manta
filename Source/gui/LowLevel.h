#pragma once
#include "Knob.h"
#include "../audio/Manta.h"
#include "../audio/Filter.h"
#include "SpectroBeamComp.h"
#include "EQPad.h"
#include "FilterResponseGraph.h"
#include "../audio/XenManager.h"

namespace gui
{
    struct LowLevel :
        public Comp
    {
        static constexpr int NumLanes = audio::Manta::NumLanes;
		static constexpr int ParamsPerLane = audio::Manta::NumParametersPerLane;

        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
			enabled{Knob(u), Knob(u), Knob(u) },
			frequency{ Knob(u), Knob(u), Knob(u) },
			resonance{ Knob(u), Knob(u), Knob(u) },
			slope{ Knob(u), Knob(u), Knob(u) },
			drive{ Knob(u), Knob(u), Knob(u) },
			feedback{ Knob(u), Knob(u), Knob(u) },
			oct{ Knob(u), Knob(u), Knob(u) },
			semi{ Knob(u), Knob(u), Knob(u) },
			rmDepth{ Knob(u), Knob(u), Knob(u) },
			gain{ Knob(u), Knob(u), Knob(u) },
            eqPad(u, "Adjust the filters on the eq pad."),
            spectroBeam(u, u.audioProcessor.spectroBeam),
            filterResponseGraph(u),
            
            filterParams()
        {
            for (auto i = 0; i < NumLanes; ++i)
            {
                const auto offset = i * ParamsPerLane;

                makeParameter(enabled[i], param::offset(PID::Lane1Enabled, offset), "Enabled");
                makeParameter(frequency[i], param::offset(PID::Lane1Pitch, offset), "Pitch");
				makeParameter(resonance[i], param::offset(PID::Lane1Resonance, offset), "Resonance");
				makeParameter(slope[i], param::offset(PID::Lane1Slope, offset), "Slope");
				makeParameter(drive[i], param::offset(PID::Lane1Drive, offset), "Drive");
				makeParameter(feedback[i], param::offset(PID::Lane1Feedback, offset), "Feedback");
				makeParameter(oct[i], param::offset(PID::Lane1DelayOct, offset), "Oct");
				makeParameter(semi[i], param::offset(PID::Lane1DelaySemi, offset), "Semi");
				makeParameter(rmDepth[i], param::offset(PID::Lane1RMDepth, offset), "Rm Depth");
				makeParameter(gain[i], param::offset(PID::Lane1Gain, offset), "Gain");

                addAndMakeVisible(enabled[i]);
				addAndMakeVisible(frequency[i]);
				addAndMakeVisible(resonance[i]);
				addAndMakeVisible(slope[i]);
				addAndMakeVisible(drive[i]);
				addAndMakeVisible(feedback[i]);
				addAndMakeVisible(oct[i]);
				addAndMakeVisible(semi[i]);
				addAndMakeVisible(rmDepth[i]);
				addAndMakeVisible(gain[i]);
            }

            addAndMakeVisible(spectroBeam);
			
            for (auto& f : filterParams)
                f = 0.f;

            addAndMakeVisible(filterResponseGraph);
            filterResponseGraph.update = [&](Path& responseCurve, float w, float h)
            {
                bool needsUpdate = false;

                const auto Fs = utils.audioProcessor.getSampleRate();
                const auto fsInv = 1.f / static_cast<float>(Fs);

                for (auto l = 0; l < NumLanes; ++l)
                {
                    auto offset = l * ParamsPerLane;

                    const auto nEnabled = utils.getParam(PID::Lane1Enabled, offset)->getValMod();
                    const auto nPitch = utils.getParam(PID::Lane1Pitch, offset)->getValModDenorm();
                    const auto nFc = utils.audioProcessor.xenManager.noteToFreqHzWithWrap(nPitch) * fsInv;
                    const auto nResonance = utils.getParam(PID::Lane1Resonance, offset)->getValModDenorm();
                    const auto nSlope = std::rint(utils.getParam(PID::Lane1Slope, offset)->getValModDenorm());
					const auto nGain = audio::decibelToGain(utils.getParam(PID::Lane1Gain, offset)->getValModDenorm());

                    offset = l * 5;
                    const auto i0 = offset;
                    const auto i1 = 1 + offset;
                    const auto i2 = 2 + offset;
                    const auto i3 = 3 + offset;
					const auto i4 = 4 + offset;

                    if (filterParams[i0] != nEnabled
                        || filterParams[i1] != nFc
                        || filterParams[i2] != nResonance
                        || filterParams[i3] != nSlope
						|| filterParams[i4] != nGain)
                    {
                        filterParams[i0] = nEnabled;
                        filterParams[i1] = nFc;
                        filterParams[i2] = nResonance;
                        filterParams[i3] = nSlope;
						filterParams[i4] = nGain;

                        needsUpdate = true;
                    }
                }

                if (needsUpdate)
                {
					auto& xen = utils.audioProcessor.xenManager;

                    audio::FilterBandpassSlope<4> fltr;
                    responseCurve.clear();
                    responseCurve.startNewSubPath(0.f, h);
                    for (auto x = 0.f; x < w; ++x)
                    {
                        const auto f = x / w;
						
                        const auto pitch = f * 128.f;
                        const auto freqHz = xen.noteToFreqHzWithWrap(pitch);
                        const auto rIdx = freqHz * fsInv;
                        
                        auto mag = 0.f;
                        for (auto l = 0; l < NumLanes; ++l)
                        {
                            auto offset = l * 5;
                            const auto i0 = offset;
                            const auto i1 = 1 + offset;
                            const auto i2 = 2 + offset;
                            const auto i3 = 3 + offset;
							const auto i4 = 4 + offset;

                            const auto nEnabled = filterParams[i0];
                            const auto nFc = filterParams[i1];
                            const auto nQ = filterParams[i2];
                            const auto nSlope = filterParams[i3];
							const auto nGain = filterParams[i4];
							
                            const auto g = std::rint(nEnabled) * nGain;

                            fltr.setStage(static_cast<int>(nSlope));
                            fltr.setFc(nFc, nQ);
                            mag += std::abs(fltr.response(rIdx)) * g;
                        }

                        const auto y = h - h * mag;

                        responseCurve.lineTo(x, y);
                    }
                    responseCurve.lineTo(w, h);
                }
				
                return needsUpdate;
            };
			
            addAndMakeVisible(eqPad);

            eqPad.addNode(PID::Lane1Pitch, PID::Lane1Resonance, PID::Lane1Slope, PID::Lane1Enabled);
			eqPad.addNode(PID::Lane2Pitch, PID::Lane2Resonance, PID::Lane2Slope, PID::Lane2Enabled);
			eqPad.addNode(PID::Lane3Pitch, PID::Lane3Resonance, PID::Lane3Slope, PID::Lane3Enabled);

            layout.init
            (
                { 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3 },
                { 3, 2, 5, 2, 5, 2, 5, 34, 3 }
            );
        }

        void paint(Graphics& g) override
        {
            auto thicc = utils.thicc;
            auto thicc2 = thicc * 2.f;
            const Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

            g.setColour(Colours::c(ColourID::Txt));
            
			///*
            {
                const auto laneArea = layout(1, 1, ParamsPerLane, 2);
                g.drawFittedText("Lane 1", laneArea.toNearestInt(), Just::centredTop, 1);
                drawRectEdges(g, laneArea, thicc2, stroke);
            }
            {
                const auto laneArea = layout(1, 3, ParamsPerLane, 2);
                g.drawFittedText("Lane 2", laneArea.toNearestInt(), Just::centredTop, 1);
                drawRectEdges(g, laneArea, thicc2, stroke);
            }
			{
				const auto laneArea = layout(1, 5, ParamsPerLane, 2);
				g.drawFittedText("Lane 3", laneArea.toNearestInt(), Just::centredTop, 1);
				drawRectEdges(g, laneArea, thicc2, stroke);
			}
            //*/
            
        }

        void resized() override
        {
            layout.resized();
            for (auto i = 0; i < NumLanes; ++i)
            {
                auto y = 2 + i * 2;
                ///*
                layout.place(enabled[i], 1, y, 1, 1, false);
				layout.place(frequency[i], 2, y, 1, 1, false);
				layout.place(resonance[i], 3, y, 1, 1, false);
				layout.place(slope[i], 4, y, 1, 1, false);
                layout.place(feedback[i], 5, y, 1, 1, false);
				layout.place(oct[i], 6, y, 1, 1, false);
                layout.place(semi[i], 7, y, 1, 1, false);
				layout.place(drive[i], 8, y, 1, 1, false);
                layout.place(rmDepth[i], 9, y, 1, 1, false);
				layout.place(gain[i], 10, y, 1, 1, false);
                //*/
            }

			layout.place(eqPad, 1, 7, ParamsPerLane, 1, false);
            auto eqPadBounds = eqPad.bounds.toNearestInt();
            eqPadBounds.setX(eqPadBounds.getX() + eqPad.getX());
			eqPadBounds.setY(eqPadBounds.getY() + eqPad.getY());
            filterResponseGraph.setBounds(eqPadBounds);
            spectroBeam.setBounds(eqPadBounds);
        }

    protected:
        std::array<Knob, NumLanes> enabled, frequency, resonance, slope, drive, feedback, oct, semi, rmDepth, gain;
        EQPad eqPad;
        SpectroBeamComp<11> spectroBeam;
        FilterResponseGraph2 filterResponseGraph;
		
        std::array<float, NumLanes * 5> filterParams;
    };
}