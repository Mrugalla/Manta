#include "MidSide.h"
#include "AudioUtils.h"

namespace audio
{
	void encodeMS(float** samples, int numSamples, int ch0) noexcept
	{
		const auto ch1 = ch0 + 1;

		for (auto s = 0; s < numSamples; ++s)
		{
			const auto mid = samples[ch0][s] + samples[ch1][s];
			const auto side = samples[ch0][s] - samples[ch1][s];

			samples[ch0][s] = mid;
			samples[ch1][s] = side;
		}

		for (auto ch = ch0; ch < ch0 + 2; ++ch)
			SIMD::multiply(samples[ch], .5f, numSamples);
	}

	void decodeMS(float** samples, int numSamples, int ch0) noexcept
	{
		const auto ch1 = ch0 + 1;

		for (auto s = 0; s < numSamples; ++s)
		{
			const auto left = samples[ch0][s] + samples[ch1][s];
			const auto right = samples[ch0][s] - samples[ch1][s];

			samples[ch0][s] = left;
			samples[ch1][s] = right;
		}
	}
}