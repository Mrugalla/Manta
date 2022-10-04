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
		using FlexButtons = std::vector<FlexButton>;

		MantaComp(Utils& u, OnSelectionChangeds& onSelectionChanged) :
			Comp(u, "", CursorType::Default),
			enabled()
		{
			onSelectionChanged.push_back([&](const NodePtrs& selected)
			{
				const auto numSelected = selected.size();
				if (numSelected == 0)
				{
					removeChildComponent(enabled.get());
					enabled.reset();
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

					setVisible(true);
				}
				
				resized();
			});

			layout.init
			(
				{ 1, 1, 1 },
				{ 1, 1, 1 }
			);
		}

		void paint(Graphics& g) override
		{
			auto thicc = utils.thicc;
			auto bounds = getLocalBounds().toFloat().reduced(thicc);

			g.setColour(Colours::c(ColourID::Txt));
			g.drawRoundedRectangle(bounds, thicc, thicc);
		}

		void resized() override
		{
			layout.resized();

			if(enabled)
				layout.place(*enabled, 1, 1, 1, 1, true);
		}
		
		FlexButton enabled;
	};
}