#pragma once
#include "Comp.h"
#include <functional>
#include <array>
#include <complex>
#include "../audio/XenManager.h"

namespace gui
{
	struct FilterResponseGraph :
		public Comp,
		public Timer
	{
		using Complex = std::complex<float>;
		
#if JUCE_DEBUG
		static constexpr int Size = 1 << 10;
#else
		static constexpr int Size = 1 << 12;
#endif
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
			const auto wInv = 1.f / w;
			const auto buf = processBuffer.data();
			const auto& xen = utils.audioProcessor.xenManager;
			const auto fsInv = 1.f / static_cast<float>(utils.audioProcessor.getSampleRate());
			
			responseCurve.clear();
			{
				const auto y = h - h * buf[0];
				responseCurve.startNewSubPath(0.f, y);
			}
			for (auto x = thicc; x < w; x += 1.f)
			{
				const auto r = x * wInv;
				const auto pitch = r * 128.f;
				const auto freqHz = xen.noteToFreqHzWithWrap(pitch + xen.getXen());
				const auto fc = freqHz * fsInv;
				const auto idx = fc * SizeF;
				
				auto y = h - h * interpolate::lerp(buf, idx);
				y = juce::jlimit(0.f, h, y);

				responseCurve.lineTo(x, y);
			}
			responseCurve.lineTo(w, h - h * buf[Size - 1]);
		}

	};

	struct FilterResponseGraph2 :
		public Comp,
		public Timer
	{
		FilterResponseGraph2(Utils& u) :
			Comp(u, "", CursorType::Default),
			responseCurveCID(ColourID::Hover),
			update(),
			responseCurve()
		{
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

		void timerCallback()
		{
			const auto w = static_cast<float>(getWidth());
			const auto h = static_cast<float>(getHeight());

			if (update(responseCurve, w, h))
				repaint();
		}

		ColourID responseCurveCID;
		/* responseCurve, width, height */
		std::function<bool(Path&, float, float)> update;
	protected:
		Path responseCurve;
	};
}