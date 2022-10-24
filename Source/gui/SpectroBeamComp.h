#pragma once
#include "Comp.h"
#include "../audio/SpectroBeam.h"
#include "../audio/XenManager.h"
#include "../arch/Interpolation.h"
#include "../arch/Conversion.h"
#include <array>

namespace gui
{
	template<size_t Order>
	struct SpectroBeamComp :
		public Comp,
		public Timer
	{
		using SpecBeam = audio::SpectroBeam<Order>;
		static constexpr int Size = SpecBeam::Size;
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;
		static constexpr float SizeFHalf = SizeF * .5f;

		SpectroBeamComp(Utils& u, SpecBeam& _beam) :
			Comp(u, "Spectro Beam", CursorType::Default),
			mainColCID(ColourID::Hover),
			xen(u.audioProcessor.xenManager),
			beam(_beam),
			img(Image::RGB, Size, 1, true)
		{
			setInterceptsMouseClicks(false, false);
			startTimerHz(60);
			setOpaque(true);
		}

		void paint(Graphics& g) override
		{
			g.setImageResamplingQuality(Graphics::lowResamplingQuality);
			g.drawImage(img, getLocalBounds().toFloat());
		}

		void timerCallback() override
		{
			auto ready = beam.ready.load();
			if (!ready)
				return;
			
			const auto Fs = static_cast<float>(utils.audioProcessor.getSampleRate());
			const auto fsInv = 1.f / Fs;
			const auto colBase = Colours::c(ColourID::Bg);
			const auto col = Colours::c(mainColCID);
			const auto buf = beam.buffer.data();

			const auto lowestDb = -12.f;
			const auto highestDb = 6.f;
			const auto rangeDb = highestDb - lowestDb;
			const auto rangeDbInv = 1.f / rangeDb;
			
			for (auto x = 0; x < Size; ++x)
			{
				const auto norm = static_cast<float>(x) * SizeInv;
				const auto pitch = norm * 128.f;
				const auto freqHz = xen.noteToFreqHzWithWrap(pitch + xen.getXen());
				const auto binIdx = freqHz * fsInv * SizeF;
				
				const auto bin = interpolate::lerp(buf, binIdx);
				const auto magDb = audio::gainToDecibel(bin);
				const auto magMapped = juce::jlimit(0.f, 1.f, (magDb - lowestDb) * rangeDbInv);
				const auto nCol = colBase.interpolatedWith(col, magMapped);
				img.setPixelAt(x, 0, nCol);
			}
			
			beam.ready.store(false);
			repaint();
		}
		
		ColourID mainColCID;
	protected:
		const audio::XenManager& xen;
		SpecBeam& beam;
		Image img;
	};
}