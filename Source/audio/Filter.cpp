#include "Filter.h"

namespace audio
{
	// FilterBandpass
	
	FilterBandpass::FilterBandpass(float startVal) :
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

	void FilterBandpass::clear() noexcept
	{
		x1 = 0.f;
		x2 = 0.f;
		y1 = 0.f;
		y2 = 0.f;
	}
	
	void FilterBandpass::setFc(float fc, float q) noexcept
	{
		const auto omega = Tau * fc;
		cosOmega = -2.f * std::cos(omega);
		const auto sinOmega = std::sin(omega);
		alpha = sinOmega / (2.f * q);

		updateCoefficients();
	}

	void FilterBandpass::copy(const FilterBandpass& other) noexcept
	{
		a0 = other.a0;
		a1 = other.a1;
		a2 = other.a2;
		b1 = other.b1;
		b2 = other.b2;
	}

	float FilterBandpass::operator()(float x0) noexcept
	{
		return processSample(x0);
	}

	float FilterBandpass::processSample(float x0) noexcept
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

	void FilterBandpass::updateCoefficients() noexcept
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

	// FilterBandpassSlope

	template<size_t NumFilters>
	FilterBandpassSlope<NumFilters>::FilterBandpassSlope() :
		filters(),
		stage(1)
	{}

	template<size_t NumFilters>
	void FilterBandpassSlope<NumFilters>::clear() noexcept
	{
		for (auto& filter : filters)
			filter.clear();
	}

	template<size_t NumFilters>
	void FilterBandpassSlope<NumFilters>::setStage(int s) noexcept
	{
		stage = s;
	}

	template<size_t NumFilters>
	void FilterBandpassSlope<NumFilters>::setFc(float fc, float q) noexcept
	{
		filters[0].setFc(fc, q);
		for (auto i = 1; i < stage; ++i)
			filters[i].copy(filters[0]);
	}

	template<size_t NumFilters>
	void FilterBandpassSlope<NumFilters>::copy(FilterBandpassSlope<NumFilters>& other) noexcept
	{
		for (auto i = 0; i < stage; ++i)
			filters[i].copy(other.filters[i]);
		stage = other.stage;
	}

	template<size_t NumFilters>
	float FilterBandpassSlope<NumFilters>::operator()(float x) noexcept
	{
		for (auto i = 0; i < stage; ++i)
			x = filters[i](x);
		return x;
	}
	
	template struct FilterBandpassSlope<1>;
	template struct FilterBandpassSlope<2>;
	template struct FilterBandpassSlope<3>;
	template struct FilterBandpassSlope<4>;
	template struct FilterBandpassSlope<5>;
	template struct FilterBandpassSlope<6>;
	template struct FilterBandpassSlope<7>;
	template struct FilterBandpassSlope<8>;
}