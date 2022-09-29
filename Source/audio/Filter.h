#pragma once
#include "../arch/Conversion.h"
#include <cmath>

namespace audio
{
	struct FilterBandpass
	{
		/* startVal */
		FilterBandpass(float = 0.f);

		void clear() noexcept;

		/* frequency fc [0, .5[, q-factor q [1, 160..] */
		void setFc(float, float) noexcept;
		
		void copy(const FilterBandpass&) noexcept;

		float operator()(float) noexcept;

		float processSample(float) noexcept;

	protected:
		float alpha, cosOmega;
		float a0, a1, a2, b1, b2;
		float     x1, x2, y1, y2;

		void updateCoefficients() noexcept;
	};

	template<size_t NumFilters>
	struct FilterBandpassSlope
	{
		FilterBandpassSlope();

		void clear() noexcept;

		void setStage(int) noexcept;

		/* frequency fc [0, .5[, q-factor q [1, 160..] */
		void setFc(float fc, float q) noexcept;

		void copy(FilterBandpassSlope<NumFilters>&) noexcept;

		float operator()(float) noexcept;

	protected:
		std::array<FilterBandpass, NumFilters> filters;
		int stage;
	};
}