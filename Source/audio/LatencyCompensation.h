#pragma once
#include "AudioUtils.h"
#include "WHead.h"

namespace audio
{
	struct LatencyCompensation
	{
		LatencyCompensation();

		/* blockSize, latency */
		void prepare(int, int);

		/* dry, inputSamples, numChannels, numSamples */
		void operator()(float* const*, float* const*, int, int) noexcept;

		/* samples, numChannels, numSamples */
		void operator()(float* const*, int, int) noexcept;

	protected:
		AudioBuffer ring;
		WHead wHead;
	public:
		int latency;
	};
}
