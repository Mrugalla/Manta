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
		void operator()(float**, float**, int, int) noexcept;

		/* samples, numChannels, numSamples */
		void operator()(float**, int, int) noexcept;

	protected:
		AudioBuffer ring;
		WHead wHead;
	public:
		int latency;
	};
}