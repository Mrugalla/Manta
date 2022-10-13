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

		WaveTableDisplay(Utils& u, WT& _wt) :
			Button(u, "This Wavetable's display."),
			lineCID(ColourID::Txt),
			wt(_wt)
		{
		}

		void paint(Graphics& g) override
		{
			const auto bounds = getLocalBounds().toFloat();
			const auto width = bounds.getWidth();
			const auto height = bounds.getHeight();
			const auto centreY = height * .5f;
			const auto inc = width * SizeInv;

			g.setColour(Colours::c(lineCID));
			auto x = 0.f;
			for (auto s = 0; s < Size; ++s, x += inc)
			{
				auto y0 = centreY;
				auto y1 = centreY * (1.f - wt(s));
				if (y0 > y1)
					std::swap(y0, y1);
				g.drawVerticalLine(static_cast<int>(x), y0, y1);
			}
		}

		ColourID lineCID;
	protected:
		WT& wt;
	};
}