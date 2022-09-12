#include "DryWetMix.h"

namespace audio
{
	// DryWetMix::LatencyCompensation

	DryWetMix::LatencyCompensation::LatencyCompensation() :
		ring(),
		wHead(),
		latency(0)
	{}

	void DryWetMix::LatencyCompensation::prepare(int blockSize, int _latency)
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

	void DryWetMix::LatencyCompensation::operator()(float** dry, float** inputSamples, int numChannels, int numSamples) noexcept
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
		mixSmooth(mixBuf, mixP, numSamples);

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

	void DryWetMix::processMix(float** samples, int numChannels, int numSamples) const noexcept
	{
		auto bufs = buffers.getArrayOfReadPointers();
		const auto mix = bufs[Mix];
		
		for (auto ch = 0; ch < numChannels; ++ch)
		{
			const auto dry = dryBuf.getReadPointer(ch);
			auto smpls = samples[ch];

			for (auto s = 0; s < numSamples; ++s)
			{
				auto d = dry[s];
				auto w = smpls[s];
				auto m = mix[s];

				smpls[s] = d + m * (w - d);
			}
		}
	}
}