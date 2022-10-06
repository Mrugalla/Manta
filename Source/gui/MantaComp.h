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
			enabled(),
			gain(),
			pitch(),
			resonance(),
			slope()
		{
			addAndMakeVisible(mainLabel);
			addAndMakeVisible(filterLabel);

			mainLabel.textCID = ColourID::Txt;
			filterLabel.textCID = mainLabel.textCID;
			
			mainLabel.font = getFontLobster();
			filterLabel.font = mainLabel.font;

			mainLabel.mode = Label::Mode::TextToLabelBounds;
			filterLabel.mode = mainLabel.mode;

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

					setVisible(true);
				}
				
				resized();
			});

			layout.init
			(
				{ 1, 8, 1, 5, 5, 1 },
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
				layout.place(*slope, 3, 3, 2, 1, true);
		}
		
	protected:
		// labels
		Label mainLabel, filterLabel;
		// main
		FlexButton enabled;
		FlexKnob gain;
		// filter
		FlexKnob pitch, resonance;
		FlexButton slope;
		//
	};
}