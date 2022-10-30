#pragma once
#include "../audio/Manta.h"
#include "EQPad.h"
#include "WaveTableDisplay.h"
#include "Knob.h"
#include "FormulaParser.h"

namespace gui
{
	struct MantaComp :
		public Comp
	{
		using OnSelectionChangeds = EQPad::OnSelectionChangeds;
		using NodePtrs = EQPad::NodePtrs;

		using FlexButton = std::unique_ptr<Button>;
		using FlexKnob = std::unique_ptr<Knob>;
		
		static constexpr int WTSize = audio::Manta::WaveTableSize;
		using WTDisplay = WaveTableDisplay<WTSize>;
		using FlexWTDisplay = std::unique_ptr<WTDisplay>;
		using FlexParser = std::unique_ptr<FormulaParser2>;

		MantaComp(Utils& u, OnSelectionChangeds& onSelectionChanged) :
			Comp(u, "", CursorType::Default),
			mainLabel(u, "Main"),
			filterLabel(u, "Filter"),
			feedbackLabel(u, "Comb"),
			heatLabel(u, "Heat"),
			ringModLabel(u, "Phase Distortion"),
			enabled(),
			gain(),
			pitch(),
			resonance(),
			snap(),
			slope(),
			delayOct(),
			delaySemi(),
			delayFeedback(),
			heat(),
			rmOct(),
			rmSemi(),
			rmDepth(),
			wtDisplay(),
			wtParser()
		{
			addAndMakeVisible(mainLabel);
			addAndMakeVisible(filterLabel);
			addAndMakeVisible(feedbackLabel);
			addAndMakeVisible(heatLabel);
			addAndMakeVisible(ringModLabel);

			mainLabel.textCID = ColourID::Txt;
			filterLabel.textCID = mainLabel.textCID;
			feedbackLabel.textCID = mainLabel.textCID;
			heatLabel.textCID = mainLabel.textCID;
			ringModLabel.textCID = mainLabel.textCID;
			
			mainLabel.font = getFontLobster();
			filterLabel.font = mainLabel.font;
			feedbackLabel.font = mainLabel.font;
			heatLabel.font = mainLabel.font;
			ringModLabel.font = mainLabel.font;

			mainLabel.mode = Label::Mode::TextToLabelBounds;
			filterLabel.mode = mainLabel.mode;
			feedbackLabel.mode = mainLabel.mode;
			heatLabel.mode = mainLabel.mode;
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

					removeChildComponent(snap.get());
					snap.reset();

					removeChildComponent(slope.get());
					slope.reset();

					removeChildComponent(delayOct.get());
					delayOct.reset();

					removeChildComponent(delaySemi.get());
					delaySemi.reset();

					removeChildComponent(delayFeedback.get());
					delayFeedback.reset();

					removeChildComponent(heat.get());
					heat.reset();

					removeChildComponent(rmOct.get());
					rmOct.reset();

					removeChildComponent(rmSemi.get());
					rmSemi.reset();

					removeChildComponent(rmDepth.get());
					rmDepth.reset();

					removeChildComponent(wtDisplay.get());
					wtDisplay.reset();

					removeChildComponent(wtParser.get());
					wtParser.reset();
					
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

						makeParameter(*enabled, pIDs, ButtonSymbol::Power);
					}

					gain = std::make_unique<Knob>(u);
					addAndMakeVisible(*gain);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->xyParam[EQPad::Y]->id);

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
							pIDs.push_back(selected[i]->scrollParam->id);

						makeParameter(*resonance, pIDs, "Reso");
					}

					snap = std::make_unique<Button>(u);
					addAndMakeVisible(*snap);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[8]);

						makeParameter(*snap, pIDs, "Snap", true);
					}

					slope = std::make_unique<Button>(u);
					addAndMakeVisible(*slope);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[0]);
						
						makeParameter(*slope, pIDs);
					}

					delayOct = std::make_unique<Knob>(u);
					addAndMakeVisible(*delayOct);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[1]);

						makeParameter(*delayOct, pIDs, "Oct", true, nullptr, Knob::LooksType::VerticalSlider);
					}

					delaySemi = std::make_unique<Knob>(u);
					addAndMakeVisible(*delaySemi);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[2]);

						makeParameter(*delaySemi, pIDs, "Semi", true, nullptr, Knob::LooksType::VerticalSlider);
					}

					delayFeedback = std::make_unique<Knob>(u);
					addAndMakeVisible(*delayFeedback);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[3]);

						makeParameter(*delayFeedback, pIDs, "Feedback");
					}

					heat = std::make_unique<Knob>(u);
					addAndMakeVisible(*heat);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[4]);

						makeParameter(*heat, pIDs, "Heat");
					}

					rmOct = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmOct);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[5]);

						makeParameter(*rmOct, pIDs, "Oct", true, nullptr, Knob::LooksType::VerticalSlider);
					}

					rmSemi = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmSemi);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[6]);

						makeParameter(*rmSemi, pIDs, "Semi", true, nullptr, Knob::LooksType::VerticalSlider);
					}

					rmDepth = std::make_unique<Knob>(u);
					addAndMakeVisible(*rmDepth);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->morePIDs[7]);

						makeParameter(*rmDepth, pIDs, "Depth", true, nullptr, Knob::LooksType::VerticalSlider);
					}

					{

						const auto pID = selected[0]->morePIDs[7];
						const auto tableIdx = pID == PID::Lane1RMDepth ? 0 :
							pID == PID::Lane2RMDepth ? 1 : 2;
						wtDisplay = std::make_unique<WTDisplay>(u, u.audioProcessor.manta.getWaveTable(tableIdx));
					}
					addAndMakeVisible(*wtDisplay);

					{
						std::vector<float*> tables;
						tables.reserve(numSelected);
						for (auto i = 0; i < numSelected; ++i)
						{
							const auto pID = selected[i]->morePIDs[7];
							const auto tableIdx = pID == PID::Lane1RMDepth ? 0 :
								pID == PID::Lane2RMDepth ? 1 : 2;
							tables.emplace_back(u.audioProcessor.manta.getWaveTable(tableIdx).data());
						}
						
						wtParser = std::make_unique<FormulaParser2>
						(
							u,
							"This wavetable's formula parser. Enter a math expression and hit enter to generate a wavetable.",
							tables,
							WTSize,
							audio::WaveTable<WTSize>::NumExtraSamples
						);
						
						auto oR = wtParser->parser.onReturn;
						wtParser->parser.onReturn = [&, oR]()
						{
							if (!oR())
								return false;

							wtDisplay->repaint();
							
							return true;
						};
					}
					addAndMakeVisible(*wtParser);

					setVisible(true);
				}
				
				resized();
			});

			layout.init
			(
				{ 1, 3, 5, 5, 2, 2, 5, 5, 2, 2, 13, 2, 1 },
				{ 1, 3, 13, 5, 1 }
			);
		}

		void paint(Graphics& g) override
		{
			auto thicc = utils.thicc;
			auto bounds = getLocalBounds().toFloat().reduced(thicc);

			Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::rounded);

			//g.setColour(Colours::c(ColourID::Hover));
			//layout.paint(g);
			
			g.setColour(Colours::c(ColourID::Txt));
			{
				auto mainBounds = layout(1, 1, 1, 3);
				drawRectEdges(g, mainBounds, thicc, stroke);
			}
			{
				auto filterBounds = layout(2, 1, 2, 3);
				drawRectEdges(g, filterBounds, thicc, stroke);
			}
			{
				auto feedbackBounds = layout(4, 1, 3, 3);
				drawRectEdges(g, feedbackBounds, thicc, stroke);
			}
			{
				auto heatBounds = layout(7, 1, 1, 3);
				drawRectEdges(g, heatBounds, thicc, stroke);
			}
			{
				auto ringmodBounds = layout(8, 1, 4, 3);
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
			layout.place(filterLabel, 2, 1, 2, 1, false);
			if(pitch)
				layout.place(*pitch, 2, 2, 1, 1, false);
			if (resonance)
				layout.place(*resonance, 3, 2, 1, 1, false);
			if (snap)
				layout.place(*snap, 2, 3, 1, 1, true);
			if (slope)
				layout.place(*slope, 3, 3, 1, 1, false);

			// delay
			layout.place(feedbackLabel, 4, 1, 3, 1, false);
			if (delayOct)
				layout.place(*delayOct, 4, 2, 1, 2, false);
			if (delaySemi)
				layout.place(*delaySemi, 5, 2, 1, 2, false);
			if (delayFeedback)
				layout.place(*delayFeedback, 6, 2, 1, 1, false);

			// heat
			layout.place(heatLabel, 7, 1, 1, 1, false);
			if (heat)
				layout.place(*heat, 7, 2, 1, 2, false);
			// ringmod
			layout.place(ringModLabel, 8, 1, 3, 1, false);
			if (rmOct)
				layout.place(*rmOct, 8, 2, 1, 2, false);
			if (rmSemi)
				layout.place(*rmSemi, 9, 2, 1, 2, false);
			if (rmDepth)
				layout.place(*rmDepth, 11, 2, 1, 2, false);
			if(wtDisplay)
				layout.place(*wtDisplay, 10, 2, 1, 1, false);
			if (wtParser)
				layout.place(*wtParser, 10, 3, 1, 1, false);
		}
		
	protected:
		// labels
		Label mainLabel, filterLabel, feedbackLabel, heatLabel, ringModLabel;
		// main
		FlexButton enabled;
		FlexKnob gain;
		// filter
		FlexKnob pitch, resonance;
		FlexButton snap, slope;
		// delay
		FlexKnob delayOct, delaySemi, delayFeedback;
		// heat
		FlexKnob heat;
		// ring mod
		FlexKnob rmOct, rmSemi, rmDepth;
		FlexWTDisplay wtDisplay;
		FlexParser wtParser;

		JUCE_HEAVYWEIGHT_LEAK_DETECTOR(MantaComp)
	};
}