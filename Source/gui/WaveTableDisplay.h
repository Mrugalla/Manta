#pragma once
#include "../audio/WaveTable.h"
#include "Button.h"

namespace gui
{
	template<size_t Size>
	struct WaveTableDisplay :
		public Button
	{
		enum class Mode { Wave, SpectralResponse, NumModes };
		
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
				else if (t == EvtType::FormulaUpdated)
				{
					wtd.repaint();
				}
			};
		}

		WaveTableDisplay(Utils& u, WT& _wt) :
			Button(u, "This Wavetable's display.", makeNotify(*this)),
			outlineCID(ColourID::Hover),
			lineCID(ColourID::Txt),
			mode(Mode::Wave),
			wt(_wt)
		{
			setBufferedToImage(true);

			//makeToggleButton(*this, "");
			//onClick.push_back([&](Button&)
			//{
			//	mode = toggleState == 0 ? Mode::Wave : Mode::SpectralResponse;
			//	repaint();
			//});
			//toggleState = 0;
		}

		void paint(Graphics& g) override
		{
			const auto thicc = utils.thicc;
			const auto bounds = getLocalBounds().toFloat().reduced(thicc);

			g.fillAll(Colours::c(ColourID::Darken));

			g.setColour(Colours::c(outlineCID));
			g.drawRoundedRectangle(bounds, thicc, thicc);
			g.setColour(Colours::c(lineCID));

			const auto width = bounds.getWidth();
			const auto height = bounds.getHeight();

			if (mode == Mode::Wave)
			{
				const auto centreY = height * .5f;
				const auto cenY2 = bounds.getY() + centreY;
				const auto inc = width * SizeInv;
				auto x = bounds.getX();
				for (auto s = 0; s < Size; ++s, x += inc)
				{
					auto y0 = cenY2;
					auto y1 = bounds.getY() + centreY * (1.f - wt(s));
					if (y0 == y1)
						++y1;
					else if (y0 > y1)
						std::swap(y0, y1);
					g.drawVerticalLine(static_cast<int>(x), y0, y1);
				}
			}
			else if (mode == Mode::SpectralResponse)
			{
				const auto wInt = static_cast<int>(width);
				const auto btm = bounds.getBottom();

				for (auto i = 0; i < wInt; ++i)
				{
					const auto iF = static_cast<float>(i);
					const auto iTau = iF * Tau;
					
					auto mag = 0.f;
					for (auto s = 0; s < Size; ++s)
					{
						const auto sF = static_cast<float>(s);
						const auto phase = iTau * sF * SizeInv;
						const auto re = std::cos(phase);
						const auto im = std::sin(phase);
						const auto smpl = wt(s);
						const auto re2 = smpl * re;
						const auto im2 = smpl * im;
						mag += std::sqrt(re2 * re2 + im2 * im2);
					}
					mag = juce::jlimit(0.f, 1.f, mag * SizeInv);
					
					const auto x = static_cast<int>(bounds.getX() + iF);
					const auto y = height - height * mag;
					g.drawVerticalLine(x, y, btm);
				}
			}
		}

		ColourID outlineCID, lineCID;
		Mode mode;
	protected:
		WT& wt;
	};
}