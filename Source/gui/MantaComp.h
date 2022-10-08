#pragma once
#include "../audio/Manta.h"
#include "EQPad.h"
#include "Knob.h"

namespace gui
{
	struct MantaComp :
		public Comp
	{
		using OnSelectionChangeds = EQPad::OnSelectionChangeds;
		using NodePtrs = EQPad::NodePtrs;

		using FlexButton = std::unique_ptr<Button>;
		using FlexKnob = std::unique_ptr<Knob>;

		MantaComp(Utils& u, OnSelectionChangeds& onSelectionChanged) :
			Comp(u, "", CursorType::Default),
			mainLabel(u, "Main"),
			filterLabel(u, "Filter"),
			feedbackLabel(u, "Feedback"),
			ringModLabel(u, "Ring Mod"),
			enabled(),
			gain(),
			pitch(),
			resonance(),
			slope(),
			delayOct(),
			delaySemi(),
			delayFeedback(),
			rmOct(),
			rmSemi(),
			rmDepth()
		{
			addAndMakeVisible(mainLabel);
			addAndMakeVisible(filterLabel);
			addAndMakeVisible(feedbackLabel);
			addAndMakeVisible(ringModLabel);

			mainLabel.textCID = ColourID::Txt;
			filterLabel.textCID = mainLabel.textCID;
			feedbackLabel.textCID = mainLabel.textCID;
			ringModLabel.textCID = mainLabel.textCID;
			
			mainLabel.font = getFontLobster();
			filterLabel.font = mainLabel.font;
			feedbackLabel.font = mainLabel.font;
			ringModLabel.font = mainLabel.font;

			mainLabel.mode = Label::Mode::TextToLabelBounds;
			filterLabel.mode = mainLabel.mode;
			feedbackLabel.mode = mainLabel.mode;
			ringModLabel.mode = mainLabel.mode;

			onSelectionChanged.push_back([&](const NodePtrs& selected)
			{
				const auto numSelected = selected.size();
				if (numSelected == 0)
				{
					removeChildComponent(enabled.get());
					enabled.reset();
					
					removeChildComponent(gain.get());
					gain.reset();
					
					removeChildComponent(pitch.get());
					pitch.reset();

					removeChildComponent(resonance.get());
					resonance.reset();

					removeChildComponent(slope.get());
					slope.reset();

					removeChildComponent(delayOct.get());
					delayOct.reset();

					removeChildComponent(delaySemi.get());
					delaySemi.reset();

					removeChildComponent(delayFeedback.get());
					delayFeedback.reset();

					removeChildComponent(rmOct.get());
					rmOct.reset();

					removeChildComponent(rmSemi.get());
					rmSemi.reset();

					removeChildComponent(rmDepth.get());
					rmDepth.reset();
					
					setVisible(false);
				}
				else
				{
					enabled = std::make_unique<Button>(u);
					addAndMakeVisible(*enabled);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->rightClickParam->id);

						makeParameterSwitchButton(*enabled, pIDs, ButtonSymbol::Power);
					}

					gain = std::make_unique<Knob>(u);
					addAndMakeVisible(*gain);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[0]);

						makeParameter(*gain, pIDs, "Gain");
					}
					
					pitch = std::make_unique<Knob>(u);
					addAndMakeVisible(*pitch);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->xyParam[EQPad::X]->id);

						makeParameter(*pitch, pIDs, "Pitch");
					}

					resonance = std::make_unique<Knob>(u);
					addAndMakeVisible(*resonance);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->xyParam[EQPad::Y]->id);

						makeParameter(*resonance, pIDs, "Reso");
					}

					slope = std::make_unique<Button>(u);
					addAndMakeVisible(*slope);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->scrollParam->id);
						
						makeParameter(*slope, pIDs);
					}

					delayOct = std::make_unique<Knob>(u);
					addAndMakeVisible(*delayOct);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[1]);

						makeParameter(*delayOct, pIDs, "Oct");
					}

					delaySemi = std::make_unique<Knob>(u);
					addAndMakeVisible(*delaySemi);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[2]);

						makeParameter(*delaySemi, pIDs, "Semi");
					}

					delayFeedback = std::make_unique<Knob>(u);
					addAndMakeVisible(*delayFeedback);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[3]);

						makeParameter(*delayFeedback, pIDs, "Feedback");
					}

					rmOct = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmOct);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[4]);

						makeParameter(*rmOct, pIDs, "Oct");
					}

					rmSemi = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmSemi);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[5]);

						makeParameter(*rmSemi, pIDs, "Semi");
					}

					rmDepth = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmDepth);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[6]);

						makeParameter(*rmDepth, pIDs, "Depth");
					}

					setVisible(true);
				}
				
				resized();
			});

			layout.init
			(
				{ 1, 8, 1, 5, 5, 1, 2, 2, 5, 1, 2, 2, 8, 1 },
				{ 1, 3, 8, 5, 1 }
			);
		}

		void paint(Graphics& g) override
		{
			auto thicc = utils.thicc;
			auto bounds = getLocalBounds().toFloat().reduced(thicc);

			Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

			g.setColour(Colours::c(ColourID::Hover));
			layout.paint(g);
			
			g.setColour(Colours::c(ColourID::Txt));
			{
				auto mainBounds = layout(1, 1, 1, 3);
				drawRectEdges(g, mainBounds, thicc, stroke);
			}
			{
				auto filterBounds = layout(3, 1, 2, 3);
				drawRectEdges(g, filterBounds, thicc, stroke);
			}
			{
				auto feedbackBounds = layout(6, 1, 3, 3);
				drawRectEdges(g, feedbackBounds, thicc, stroke);
			}
			{
				auto ringmodBounds = layout(10, 1, 3, 3);
				drawRectEdges(g, ringmodBounds, thicc, stroke);
			}
		}

		void resized() override
		{
			layout.resized();

			// main
			layout.place(mainLabel, 1, 1, 1, 1, false);
			if (gain)
				layout.place(*gain, 1, 2, 1, 1, false);
			if(enabled)
				layout.place(*enabled, 1, 3, 1, 1, true);
			
			// filter
			layout.place(filterLabel, 3, 1, 2, 1, false);
			if(pitch)
				layout.place(*pitch, 3, 2, 1, 1, false);
			if (resonance)
				layout.place(*resonance, 4, 2, 1, 1, false);
			if (slope)
				layout.place(*slope, 3, 3, 2, 1, false);

			// delay
			layout.place(feedbackLabel, 6, 1, 3, 1, false);
			if (delayOct)
				layout.place(*delayOct, 6, 2, 1, 2, false);
			if (delaySemi)
				layout.place(*delaySemi, 7, 2, 1, 2, false);
			if (delayFeedback)
				layout.place(*delayFeedback, 8, 2, 1, 1, false);

			// ringmod
			layout.place(ringModLabel, 10, 1, 3, 1, false);
			if (rmOct)
				layout.place(*rmOct, 10, 2, 1, 2, false);
			if (rmSemi)
				layout.place(*rmSemi, 11, 2, 1, 2, false);
			if (rmDepth)
				layout.place(*rmDepth, 12, 2, 1, 1, false);
		}
		
	protected:
		// labels
		Label mainLabel, filterLabel, feedbackLabel, ringModLabel;
		// main
		FlexButton enabled;
		FlexKnob gain;
		// filter
		FlexKnob pitch, resonance;
		FlexButton slope;
		// delay
		FlexKnob delayOct, delaySemi, delayFeedback;
		// ring mod
		FlexKnob rmOct, rmSemi, rmDepth;

		JUCE_HEAVYWEIGHT_LEAK_DETECTOR(MantaComp)
	};
}