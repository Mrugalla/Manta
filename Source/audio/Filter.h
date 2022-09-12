#pragma once
#include "../arch/Conversion.h"
#include <cmath>

namespace audio
{
	struct FilterBandpass
	{
		/* startVal */
		FilterBandpass(float startVal = 0.f) :
			alpha(0.f),
			cosOmega(0.f),

			a0(0.f),
			a1(0.f),
			a2(0.f),
			b1(0.f),
			b2(0.f),
			x1(0.f),
			x2(0.f),
			y1(startVal),
			y2(startVal)
		{
			
		}

		/* frequency fc [0, .5[, q-factor q [1, 160..] */
		void setFc(float fc, float q) noexcept
		{
			const auto omega = Tau * fc;
			cosOmega = -2.f * std::cos(omega);
			const auto sinOmega = std::sin(omega);
			alpha = sinOmega / (2.f * q);
			
			updateCoefficients();
		}
		
		void copy(const FilterBandpass& other) noexcept
		{
			a0 = other.a0;
			a1 = other.a1;
			a2 = other.a2;
			b1 = other.b1;
			b2 = other.b2;
		}

		float operator()(float x0) noexcept
		{
			return processSample(x0);
		}

		float processSample(float x0) noexcept
		{
			auto y0 =
				x0 * a0 +
				x1 * a1 +
				x2 * a2 +
				y1 * b1 +
				y2 * b2;

			x2 = x1;
			x1 = x0;
			y2 = y1;
			y1 = y0;
			
			return y0;
		}

	protected:
		float alpha, cosOmega;
		float a0, a1, a2, b1, b2;
		float     x1, x2, y1, y2;

		void updateCoefficients() noexcept
		{
			a0 = alpha;
			a1 = 0.f;
			a2 = -alpha;

			b1 = cosOmega;
			b2 = 1.f - alpha;

			const auto b0 = 1.f + alpha;
			const auto b0Inv = 1.f / b0;

			a0 *= b0Inv;
			//a1 *= b0Inv;
			a2 *= b0Inv;
			b1 *= -b0Inv;
			b2 *= -b0Inv;
		}
	};
}