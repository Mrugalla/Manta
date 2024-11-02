#pragma once
#include "LatencyCompensation.h"
#include <array>

namespace audio
{
	class DryWetMix
	{
		enum
		{
#if PPDHasGainIn
			GainIn,
#endif
			Mix,
			GainOut,
			NumBufs
		};

	public:
		DryWetMix();

		/* sampleRate, blockSize, latency */
		void prepare(float, int, int);

		/* samples, numChannels, numSamples, gainInP, unityGainP, mixP, gainOutP, polarityP*/
		void saveDry
		(
			float* const*, int, int,
#if PPDHasGainIn
			float,
#if PPDHasUnityGain
			float,
#endif
#endif
			float, float
#if PPDHasPolarity
			, float
#endif
		) noexcept;

		/* samples, numChannels, numSamples */
		void processBypass(float* const*, int, int) noexcept;

		/* samples, numChannels, numSamples */
		void processOutGain(float* const*, int, int) const noexcept;

		/* samples, numChannels, numSamples, delta */
		void processMix(float* const*, int, int
#if PPDHasDelta
			, bool
#endif
			) const noexcept;

	protected:
		LatencyCompensation latencyCompensation;

		AudioBuffer buffers;
		
#if PPDHasGainIn
		Smooth gainInSmooth;
#endif
		Smooth mixSmooth, gainOutSmooth;

		AudioBuffer dryBuf;
	};
}
