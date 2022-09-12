#pragma once
#include "AudioUtils.h"

namespace audio
{
	struct PRM
	{
		/* startVal */
		PRM(float);

		/*Fs, blockSize, smoothLenMs */
		void prepare(float, int, float);

		/* value, numSamples */
		float* operator()(float, int) noexcept;

		/* numSamples */
		float* operator()(int) noexcept;

		Smooth smooth;
		std::vector<float> buf;
	};
}