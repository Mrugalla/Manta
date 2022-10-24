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
		static constexpr size_t SizeHalf = Size / 2;
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;
		
		
		using FFT = juce::dsp::FFT;
		using Fifo = std::array<float, Size>;
		using Fifo2 = std::array<float, Size2>;

		SpectroBeam() :
			smpls(),
			fft(Order),
			fifo(),
			window(),
			buffer(),
			ready(false),
			idx(0)
		{
			SIMD::clear(buffer.data(), Size2);
			SIMD::clear(fifo.data(), Size2);

			// gaussian window
			for (auto i = 0; i < Size; ++i)
			{
				const auto norm = static_cast<float>(i) * SizeInv;
				const auto x = norm * 2.f - 1.f;
				const auto w = std::exp(-x * x * 16.f);
				window[i] = w;
			}
		}

		void prepare(int blockSize)
		{
			smpls.resize(blockSize);
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
				smpls[s] = mid;
			}

			process(numSamples);
		}

		void process(int numSamples) noexcept
		{
			auto fif = fifo.data();
			for (auto s = 0; s < numSamples; ++s)
			{
				fif[idx] = smpls[s];
				++idx;
				if (idx == Size)
				{
					const auto wndw = window.data();
					auto buf = buffer.data();
					
					SIMD::multiply(fif, wndw, Size);
					fft.performRealOnlyForwardTransform(fif, true);
					SIMD::copy(buf, fif, Size);
					ready.store(true);
					idx = 0;
				}
			}
		}

	protected:
		std::vector<float> smpls;
		FFT fft;
		Fifo2 fifo;
		Fifo window;
	public:
		Fifo2 buffer;
		std::atomic<bool> ready;
	protected:
		int idx;
	};
}