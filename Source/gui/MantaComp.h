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
			enabled(),
			pitch(),
			resonance(),
			slope()
		{
			onSelectionChanged.push_back([&](const NodePtrs& selected)
			{
				const auto numSelected = selected.size();
				if (numSelected == 0)
				{
					removeChildComponent(enabled.get());
					enabled.reset();
					
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

					slope = std::make_unique<Knob>(u);
					addAndMakeVisible(*slope);
					{
						std::vector<PID> pIDs;
						for (auto i = 0; i < numSelected; ++i)
							pIDs.push_back(selected[i]->scrollParam->id);

						makeParameter(*slope, pIDs, "Slope");
					}

					setVisible(true);
				}
				
				resized();
			});

			layout.init
			(
				{ 1, 8, 1, 5, 5, 1, 1 },
				{ 1, 13, 8, 1 }
			);
		}

		void paint(Graphics& g) override
		{
			auto thicc = utils.thicc;
			auto bounds = getLocalBounds().toFloat().reduced(thicc);

			g.setColour(Colours::c(ColourID::Txt));
			layout.paint(g);
			//g.drawRoundedRectangle(bounds, thicc, thicc);
		}

		void resized() override
		{
			layout.resized();

			if(enabled)
				layout.place(*enabled, 1, 1, 1, 1, true);
			if(pitch)
				layout.place(*pitch, 3, 1, 1, 1, false);
			if (resonance)
				layout.place(*resonance, 4, 1, 1, 1, false);
			if (slope)
				layout.place(*slope, 3, 2, 2, 1, false);
		}
		
		FlexButton enabled;
		FlexKnob pitch, resonance, slope;
	};
}