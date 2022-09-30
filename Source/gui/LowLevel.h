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

        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
			enabled{Knob(u), Knob(u), Knob(u) },
			frequency{ Knob(u), Knob(u), Knob(u) },
			resonance{ Knob(u), Knob(u), Knob(u) },
			slope{ Knob(u), Knob(u), Knob(u) },
			drive{ Knob(u), Knob(u), Knob(u) },
			delay{ Knob(u), Knob(u), Knob(u) },
			gain{ Knob(u), Knob(u), Knob(u) },
            eqPad(u, "Adjust the filters on the eq pad."),
            spectroBeam(u, u.audioProcessor.spectroBeam),
            filterResponseGraph(u),
            
            filterParams()
        {
            for (auto i = 0; i < NumLanes; ++i)
            {
                const auto offset = i * 7;

                makeParameter(enabled[i], param::offset(PID::Lane1Enabled, offset), "Enabled");
                makeParameter(frequency[i], param::offset(PID::Lane1Pitch, offset), "Pitch");
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
                    auto offset = l * 7;

                    const auto nPitch = utils.getParam(PID::Lane1Pitch, offset)->getValModDenorm();
                    const auto nFc = utils.audioProcessor.xenManager.noteToFreqHzWithWrap(nPitch) * fsInv;
                    const auto nResonance = utils.getParam(PID::Lane1Resonance, offset)->getValModDenorm();
                    const auto nSlope = std::rint(utils.getParam(PID::Lane1Slope, offset)->getValModDenorm());

                    offset = l * 3;
                    const auto i0 = offset;
                    const auto i1 = 1 + offset;
                    const auto i2 = 2 + offset;

                    if (filterParams[i0] != nFc || filterParams[i1] != nResonance || filterParams[i2] != nSlope)
                    {
                        filterParams[i0] = nFc;
                        filterParams[i1] = nResonance;
                        filterParams[i2] = nSlope;

                        needsUpdate = true;
                    }
                }

                if (needsUpdate)
                {
                    audio::FilterBandpassSlope<4> fltr;
                    responseCurve.clear();
                    responseCurve.startNewSubPath(0.f, h);
                    for (auto x = 0.f; x < w; ++x)
                    {
                        auto f = x / w;
                        auto mag = 0.f;
						
                        for (auto l = 0; l < NumLanes; ++l)
                        {
                            auto offset = l * 3;
                            const auto i0 = offset;
                            const auto i1 = 1 + offset;
                            const auto i2 = 2 + offset;

                            const auto nFc = filterParams[i0];
                            const auto nQ = filterParams[i1];
                            const auto nSlope = filterParams[i2];
							
                            fltr.setStage(static_cast<int>(nSlope));
                            fltr.setFc(nFc, nQ);
                            mag += std::abs(fltr.response(f));
                        }

                        const auto y = h - h * mag;

                        responseCurve.lineTo(x, y);
                    }
                    responseCurve.lineTo(w, h);
                }
				
                return needsUpdate;
            };
			/*
            filterResponseGraph.needsUpdate = [&]()
            {
				bool needsUpdate = false;

                const auto Fs = utils.audioProcessor.getSampleRate();
                const auto fsInv = 1.f / static_cast<float>(Fs);

                for (auto l = 0; l < NumLanes; ++l)
                {
                    auto offset = l * 7;

                    const auto nPitch = utils.getParam(PID::Lane1Pitch, offset)->getValModDenorm();
                    const auto nFc = utils.audioProcessor.xenManager.noteToFreqHzWithWrap(nPitch) * fsInv;
                    const auto nResonance = utils.getParam(PID::Lane1Resonance, offset)->getValModDenorm();
                    const auto nSlope = std::rint(utils.getParam(PID::Lane1Slope, offset)->getValModDenorm());

                    offset = l * 3;
                    const auto i0 = offset;
                    const auto i1 = 1 + offset;
                    const auto i2 = 2 + offset;

                    if (filterParams[i0] != nFc || filterParams[i1] != nResonance || filterParams[i2] != nSlope)
                    {
                        filterParams[i0] = nFc;
                        filterParams[i1] = nResonance;
                        filterParams[i2] = nSlope;

                        needsUpdate = true;
                    }
                }

                return needsUpdate;
            };
            filterResponseGraph.processFilters = [&](float* samples, FilterResponseGraph::Buffer& impulse, int numSamples)
            {
                for (auto l = 0; l < NumLanes; ++l)
                {	
                    const auto offset = l * 3;
                    const auto i0 = offset;
                    const auto i1 = 1 + offset;
                    const auto i2 = 2 + offset;
                    
                    const auto fc = filterParams[i0];
					const auto res = filterParams[i1];
                    const auto slope = static_cast<int>(filterParams[i2]);
					
					audio::FilterBandpassSlope<4> fltr;
                    fltr.setStage(slope);
                    fltr.setFc(fc, res);

                    for (auto s = 0; s < numSamples; ++s)
                    {
                        auto x = impulse[s];
                        auto y = fltr(x);
                        samples[s] += y;
                    }					
                }
            };
            */

            addAndMakeVisible(eqPad);

            eqPad.addNode(PID::Lane1Pitch, PID::Lane1Resonance, PID::Lane1Slope);
			eqPad.addNode(PID::Lane2Pitch, PID::Lane2Resonance, PID::Lane2Slope);
			eqPad.addNode(PID::Lane3Pitch, PID::Lane3Resonance, PID::Lane3Slope);

            layout.init
            (
                { 3, 5, 5, 5, 5, 5, 5, 5, 3 },
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
				layout.place(drive[i], 5, y, 1, 1, false);
				layout.place(delay[i], 6, y, 1, 1, false);
				layout.place(gain[i], 7, y, 1, 1, false);
                //*/
            }

			layout.place(eqPad, 1, 7, 7, 1, false);
            auto eqPadBounds = eqPad.bounds.toNearestInt();
            eqPadBounds.setX(eqPadBounds.getX() + eqPad.getX());
			eqPadBounds.setY(eqPadBounds.getY() + eqPad.getY());
            filterResponseGraph.setBounds(eqPadBounds);
            spectroBeam.setBounds(eqPadBounds);
        }

    protected:
        std::array<Knob, NumLanes> enabled, frequency, resonance, slope, drive, delay, gain;
        EQPad eqPad;
        SpectroBeamComp<11> spectroBeam;
        FilterResponseGraph2 filterResponseGraph;
		
        std::array<float, NumLanes * 3> filterParams;
    };
}