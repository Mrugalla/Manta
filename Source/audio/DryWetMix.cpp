#include "DryWetMix.h"

namespace audio
{
	// DryWetMix

	DryWetMix::DryWetMix() :
		latencyCompensation(),

		buffers(),
#if PPDHasGainIn
		gainInSmooth(0.f),
#endif
		mixSmooth(1.f),
		gainOutSmooth(1.f),

		dryBuf()
	{}

	void DryWetMix::prepare(float sampleRate, int blockSize, int latency)
	{
		latencyCompensation.prepare(blockSize, latency);

#if PPDHasGainIn
		gainInSmooth.makeFromDecayInMs(20.f, sampleRate);
#endif
		mixSmooth.makeFromDecayInMs(20.f, sampleRate);
		gainOutSmooth.makeFromDecayInMs(20.f, sampleRate);

		dryBuf.setSize(2, blockSize, false, true, false);

		buffers.setSize(NumBufs, blockSize, false, true, false);
	}

	void DryWetMix::saveDry(float** samples, int numChannels, int numSamples,
#if PPDHasGainIn
		float gainInP,
#if PPDHasUnityGain
		float unityGainP,
#endif
#endif
		float mixP, float gainP
#if PPDHasPolarity
		, float polarityP
#endif
	) noexcept
	{
		auto bufs = buffers.getArrayOfWritePointers();

		latencyCompensation
		(
			dryBuf.getArrayOfWritePointers(),
			samples,
			numChannels,
			numSamples
		);

#if PPDHasGainIn
		auto gainInBuf = bufs[GainIn];
		gainInSmooth(gainInBuf, juce::Decibels::decibelsToGain(gainInP), numSamples);
		for (auto ch = 0; ch < numChannels; ++ch)
			for (auto s = 0; s < numSamples; ++s)
				samples[ch][s] *= gainInBuf[s];

#if PPDHasUnityGain
		gainP -= gainInP * unityGainP;
#endif
#endif
		auto mixBuf = bufs[Mix];
#if PPD_MixOrGainDry == 0
		mixSmooth(mixBuf, mixP, numSamples);
#else
		mixSmooth(mixBuf, decibelToGain(mixP, -80.f), numSamples);
#endif

		gainP = Decibels::decibelsToGain(gainP);
#if PPDHasPolarity
		gainP *= polarityP;
#endif
		gainOutSmooth(bufs[GainOut], gainP, numSamples);
	}

	void DryWetMix::processBypass(float** samples, int numChannels, int numSamples) noexcept
	{
		latencyCompensation
		(
			dryBuf.getArrayOfWritePointers(),
			samples,
			numChannels,
			numSamples
		);

		for (auto ch = 0; ch < numChannels; ++ch)
		{
			const auto dry = dryBuf.getReadPointer(ch);
			auto smpls = samples[ch];

			SIMD::copy(smpls, dry, numSamples);
		}
	}

	void DryWetMix::processOutGain(float** samples, int numChannels, int numSamples) const noexcept
	{
		auto bufs = buffers.getArrayOfReadPointers();
		const auto gainBuf = bufs[GainOut];
		
		for (auto ch = 0; ch < numChannels; ++ch)
			SIMD::multiply(samples[ch], gainBuf, numSamples);
	}

	void DryWetMix::processMix(float** samples, int numChannels, int numSamples
#if PPDHasDelta
		, bool deltaP
#endif
		) const noexcept
	{
		auto bufs = buffers.getArrayOfReadPointers();
		const auto mix = bufs[Mix];

		for (auto ch = 0; ch < numChannels; ++ch)
		{
			const auto dry = dryBuf.getReadPointer(ch);
			auto smpls = samples[ch];

			for (auto s = 0; s < numSamples; ++s)
			{
				const auto d = dry[s];
				const auto w = smpls[s];
				const auto m = mix[s];
#if PPD_MixOrGainDry == 0
				smpls[s] = d + m * (w - d);
#else
				smpls[s] = m * d + w;
#endif
			}
		}
#if PPDHasDelta
		if(deltaP)
		{
			for (auto ch = 0; ch < numChannels; ++ch)
			{
				const auto dry = dryBuf.getReadPointer(ch);
				auto smpls = samples[ch];

				for (auto s = 0; s < numSamples; ++s)
				{
					const auto d = dry[s];
					const auto w = smpls[s];
					
					smpls[s] = w - d;
				}
			}
		}
#endif
	}
}