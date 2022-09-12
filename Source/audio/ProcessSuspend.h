#pragma once
#include "AudioUtils.h"

namespace audio
{
	struct ProcessSuspender
	{
		enum class Stage
		{
			Running,
			Suspending,
			Suspended,
			NumStages
		};

		ProcessSuspender(juce::AudioProcessor& p) :
			processor(p),
			stage(Stage::Running)
		{

		}

		void suspend() noexcept
		{
			stage.store(Stage::Suspending);
		}

		/* returns true if suspending is needed (= return from processBlock) */
		bool suspendIfNeeded(AudioBuffer& buf) noexcept
		{
			auto samples = buf.getArrayOfWritePointers();
			const auto numChannels = buf.getNumChannels();
			const auto numSamples = buf.getNumSamples();

			const auto stg = stage.load();
			if (stg == Stage::Running)
				return false;

			if(numSamples != 0)
				for (auto ch = 0; ch < numChannels; ++ch)
					SIMD::fill(samples[ch], 0.f, numSamples);

			if (stg == Stage::Suspending)
			{
				stage.store(Stage::Suspended);
				processor.prepareToPlay
				(
					processor.getSampleRate(),
					processor.getBlockSize()
				);
			}

			return true;
		}
		
		void prepareToPlay() noexcept
		{
			stage.store(Stage::Running);
		}

	protected:
		juce::AudioProcessor& processor;
		std::atomic<Stage> stage;
	};
}
