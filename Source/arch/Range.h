#pragma once
#include "juce_core/juce_core.h"
#include "Conversion.h"

namespace makeRange
{
	using Range = juce::NormalisableRange<float>;

	inline Range biased(float start, float end, float bias/*[-1, 1]*/) noexcept
	{
		// https://www.desmos.com/calculator/ps8q8gftcr
		const auto a = bias * .5f + .5f;
		const auto a2 = 2.f * a;
		const auto aM = 1.f - a;

		const auto r = end - start;
		const auto aR = r * a;
		if (bias != 0.f)
			return
		{
				start, end,
				[a2, aM, aR](float min, float, float x)
				{
					const auto denom = aM - x + a2 * x;
					if (denom == 0.f)
						return min;
					return min + aR * x / denom;
				},
				[a2, aM, aR](float min, float, float x)
				{
					const auto denom = a2 * min + aR - a2 * x - min + x;
					if (denom == 0.f)
						return 0.f;
					auto val = aM * (x - min) / denom;
					return val > 1.f ? 1.f : val;
				},
				[](float min, float max, float x)
				{
					return x < min ? min : x > max ? max : x;
				}
		};
		else return { start, end };
	}

	inline Range toggle() noexcept
	{
		return
		{
			0.f, 1.f,
			[](float, float, float x)
			{
				return x;
			},
			[](float, float, float x)
			{
				return x;
			},
			[](float, float, float x)
			{
				return x > .5 ? 1.f : 0.f;
			}
		};
	}

	inline Range stepped(float start, float end, float steps = 1.f) noexcept
	{
		return
		{
				start, end,
				[range = end - start](float min, float, float normalized)
				{
					return min + normalized * range;
				},
				[rangeInv = 1.f / (end - start)](float min, float, float denormalized)
				{
					return (denormalized - min) * rangeInv;
				},
				[steps, stepsInv = 1.f / steps](float min, float max, float x)
				{
					return juce::jlimit(min, max, std::rint(x * stepsInv) * steps);
				}
		};
	}

	inline Range lin(float start, float end) noexcept
	{
		const auto range = end - start;

		return
		{
				start,
				end,
				[range](float min, float, float normalized)
				{
					return min + normalized * range;
				},
				[inv = 1.f / range](float min, float, float denormalized)
				{
					return (denormalized - min) * inv;
				},
				[](float min, float max, float x)
				{
					return juce::jlimit(min, max, x);
				}
		};
	}

	// advanced one(s):

	inline Range withCentre(float start, float end, float centre) noexcept
	{
		const auto r = end - start;
		const auto v = (centre - start) / r;

		return makeRange::biased(start, end, 2.f * v - 1.f);
	}

}