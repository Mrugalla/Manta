#pragma once
#include "../audio/WaveTable.h"
#include "Button.h"

namespace gui
{
	template<size_t Size>
	struct WaveTableDisplay :
		public Button
	{
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;
		using WT = audio::WaveTable<Size>;

		Notify makeNotify(WaveTableDisplay& _wtd)
		{
			return [&wtd = _wtd](EvtType t, const void*)
			{
				if (t == EvtType::PatchUpdated)
				{
					wtd.repaint();
				}
			};
		}

		WaveTableDisplay(Utils& u, WT& _wt) :
			Button(u, "This Wavetable's display.", makeNotify(*this)),
			outlineCID(ColourID::Hover),
			lineCID(ColourID::Txt),
			wt(_wt)
		{
			setBufferedToImage(true);
		}

		void paint(Graphics& g) override
		{
			const auto thicc = utils.thicc;
			const auto bounds = getLocalBounds().toFloat().reduced(thicc);
			const auto width = bounds.getWidth();
			const auto height = bounds.getHeight();
			const auto centreY = height * .5f;
			const auto cenY2 = bounds.getY() + centreY;
			const auto inc = width * SizeInv;

			g.setColour(Colours::c(outlineCID));
			g.drawRoundedRectangle(bounds, thicc, thicc);

			g.setColour(Colours::c(lineCID));
			auto x = bounds.getX();
			for (auto s = 0; s < Size; ++s, x += inc)
			{
				auto y0 = cenY2;
				auto y1 = bounds.getY() + centreY * (1.f - wt(s));
				if (y0 > y1)
					std::swap(y0, y1);
				g.drawVerticalLine(static_cast<int>(x), y0, y1);
			}
		}

		ColourID outlineCID, lineCID;
	protected:
		WT& wt;
	};
}