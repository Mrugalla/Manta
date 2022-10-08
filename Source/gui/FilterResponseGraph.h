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

		FilterResponseGraph(Utils&);

		void paint(Graphics&) override;

		void resized() override;

		void timerCallback() override;

		void updateResponseCurve();

		ColourID responseCurveCID;
		/* samples, impulse, numSamples */
		std::function<void(float*, Buffer&, int)> processFilters;
		std::function<bool()> needsUpdate;
	protected:
		Buffer impulse;
		Buffer processBuffer;
		DFTBuffer dftBuffer;

		Path responseCurve;

		void resetBuffers();
		
		void applyForwardDFT();

		void generateResponseCurve();

	};

	struct FilterResponseGraph2 :
		public Comp,
		public Timer
	{
		FilterResponseGraph2(Utils&);

		void paint(Graphics&) override;

		void timerCallback();

		void resized() override;

		ColourID responseCurveCID;
		std::function<bool()> shallUpdate;
		/* responseCurve, width, height */
		std::function<void(Path&, float, float)> update;
	protected:
		Path responseCurve;
	};
}