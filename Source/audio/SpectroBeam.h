#pragma once
#include "AudioUtils.h"
#include "XenManager.h"
#include <juce_dsp/juce_dsp.h>
#include <array>

namespace audio
{
	template<size_t Order>
	struct SpectroBeam
	{
		static constexpr size_t Size = 1 << Order;
		static constexpr size_t Size2 = Size * 2;
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;
		
		using FFT = juce::dsp::FFT;
		using Fifo = std::array<float, Size2>;

		SpectroBeam() :
			fft(Order),
			fifo(),
			window(),
			buffer(),
			ready(false),
			idx(0)
		{
			SIMD::clear(buffer.data(), Size2);

			// gaussian window
			for (auto i = 0; i < Size; ++i)
			{
				const auto norm = static_cast<float>(i) * SizeInv;
				const auto x = norm * 2.f - 1.f;
				const auto w = std::exp(-x * x * 16.f);
				window[i] = w;
			}
		}

		void operator()(float** samples, int numChannels, int numSamples) noexcept
		{
			const auto chInv = 1.f / numChannels;

			for (auto s = 0; s < numSamples; ++s)
			{
				auto mid = samples[0][s];
				for (auto ch = 1; ch < numChannels; ++ch)
					mid += samples[ch][s];
				mid *= chInv;

				fifo[idx] = mid * window[idx];
				++idx;
				if (idx == Size)
				{
					idx = 0;
					auto buf = buffer.data();
					SIMD::copy(buf, fifo.data(), Size);
					fft.performRealOnlyForwardTransform(buf, true);
					ready.store(true);
				}
			}
		}

	protected:
		FFT fft;
		Fifo fifo, window;
	public:
		Fifo buffer;
		std::atomic<bool> ready;
	protected:
		int idx;
	};
}