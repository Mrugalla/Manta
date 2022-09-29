#pragma once
#include "Comp.h"
#include <functional>
#include <array>
#include <complex>

namespace gui
{
	struct FilterResponseGraph :
		public Comp,
		public Timer
	{
		using Complex = std::complex<float>;
		
		static constexpr int Size = 1 << 11;
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;

		using Buffer = std::array<float, Size>;
		using DFTBuffer = std::array<Complex, Size>;

		FilterResponseGraph(Utils& u) :
			Comp(u, "", CursorType::Default),
			responseCurveCID(ColourID::Hover),
			processFilters(nullptr),
			needsUpdate(nullptr),
			
			impulse(),
			processBuffer(),
			dftBuffer(),

			responseCurve()
		{
			SIMD::clear(impulse.data(), Size);
			impulse[0] = 1.f;
			
			setInterceptsMouseClicks(false, false);
			startTimerHz(4);
		}

		void paint(Graphics& g) override
		{
			if (responseCurve.isEmpty())
				return;
			const auto thicc = utils.thicc;
			const Stroke stroke(thicc, Stroke::PathStrokeType::JointStyle::curved, Stroke::PathStrokeType::EndCapStyle::rounded);

			g.setColour(Colours::c(responseCurveCID));
			g.strokePath(responseCurve, stroke);
		}

		void resized() override
		{
			if (responseCurve.isEmpty())
				return;

			updateResponseCurve();
		}

		void timerCallback() override
		{
			if (needsUpdate())
			{
				updateResponseCurve();
				repaint();
			}
		}

		void updateResponseCurve()
		{
			resetBuffers();
			
			processFilters(processBuffer.data(), impulse, Size);

			applyForwardDFT();

			generateResponseCurve();
		}

		ColourID responseCurveCID;
		/* samples, impulse, numSamples */
		std::function<void(float*, Buffer&, int)> processFilters;
		std::function<bool()> needsUpdate;
	protected:
		Buffer impulse;
		Buffer processBuffer;
		DFTBuffer dftBuffer;

		Path responseCurve;

		void resetBuffers()
		{	
			SIMD::clear(processBuffer.data(), processBuffer.size());

			for (auto& d : dftBuffer)
				d = { 0.f, 0.f };
		}
		
		void applyForwardDFT()
		{
			auto buf = processBuffer.data();
			auto dft = dftBuffer.data();
			
			for (auto n = 0; n < Size; ++n)
			{
				const auto f = Pi * static_cast<float>(n) * SizeInv;
				auto real = 0.f;
				auto imag = 0.f;
				for (auto s = 0; s < Size; ++s)
				{
					const auto sf = s * f;
					real += buf[s] * std::cos(sf);
					imag -= buf[s] * std::sin(sf);
				}
				dft[n] += { real, imag };
			}

			for (auto i = 0; i < Size; ++i)
				buf[i] = std::abs(dft[i]);
		}

		void generateResponseCurve()
		{
			const auto thicc = utils.thicc;
			const auto w = static_cast<float>(getWidth());
			const auto h = static_cast<float>(getHeight());
			const auto h2 = h * .5f;
			const auto wInv = 1.f / w;
			const auto buf = processBuffer.data();

			responseCurve.clear();
			{
				const auto y = h2 - h2 * buf[0];
				responseCurve.startNewSubPath(0.f, y);
			}
			for (auto x = thicc; x < w; x += thicc)
			{
				const auto r = x * wInv;
				const auto i = static_cast<int>(r * Size);
				
				auto y = h2 - h2 * buf[i];
				y = juce::jlimit(0.f, h, y);

				responseCurve.lineTo(x, y);
			}
		}

	};
}