/*
  ==============================================================================

    LatencyCompensation.cpp
    Created: 14 Sep 2022 6:17:08pm
    Author:  Hans

  ==============================================================================
*/

#include "LatencyCompensation.h"

namespace audio
{
	LatencyCompensation::LatencyCompensation() :
		ring(),
		wHead(),
		latency(0)
	{}

	void LatencyCompensation::prepare(int blockSize, int _latency)
	{
		latency = _latency;
		if (latency != 0)
		{
			ring.setSize(2, latency, false, true, false);
			wHead.prepare(blockSize, latency);
		}
		else
		{
			ring.setSize(0, 0);
			wHead.prepare(0, 0);
		}
	}

	void LatencyCompensation::operator()(float** dry, float** inputSamples, int numChannels, int numSamples) noexcept
	{
		if (latency != 0)
		{
			wHead(numSamples);

			for (auto ch = 0; ch < numChannels; ++ch)
			{
				const auto smpls = inputSamples[ch];

				auto rng = ring.getWritePointer(ch);
				auto dr = dry[ch];

				for (auto s = 0; s < numSamples; ++s)
				{
					const auto w = wHead[s];
					const auto r = (w + 1) % latency;

					rng[w] = smpls[s];
					dr[s] = rng[r];
				}
			}
		}
		else
			for (auto ch = 0; ch < numChannels; ++ch)
				SIMD::copy(dry[ch], inputSamples[ch], numSamples);
	}

	void LatencyCompensation::operator()(float** samples, int numChannels, int numSamples) noexcept
	{
		if (latency != 0)
		{
			wHead(numSamples);

			for (auto ch = 0; ch < numChannels; ++ch)
			{
				const auto smpls = samples[ch];
				auto rng = ring.getWritePointer(ch);

				for (auto s = 0; s < numSamples; ++s)
				{
					const auto w = wHead[s];
					const auto r = (w + 1) % latency;

					rng[w] = smpls[s];
					smpls[s] = rng[r];
				}
			}
		}
	}
	
}