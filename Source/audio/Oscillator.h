#pragma once
#include "AudioUtils.h"
#include "Phasor.h"

namespace audio
{
	template<typename Float>
	struct OscSine
	{
		OscSine() :
			phasor()
		{}
		
		void prepare(Float fsInv)
		{
			phasor.prepare(fsInv);
		}

		void setFreqHz(Float hz)
		{
			phasor.setFrequencyHz(hz);
		}
		
		Float* operator()(Float* buffer, int numSamples) noexcept
		{
			for (auto s = 0; s < numSamples; ++s)
				buffer[s] = synthesizeSample();
			return buffer;
		}

		Float operator()() noexcept
		{
			return synthesizeSample();
		}

	protected:
		Phasor<Float> phasor;

		Float synthesizeSample()
		{
			const auto phase = phasor().phase;
			return std::cos(phase * Tau);
		}
	};
	
	template<typename Float>
	struct RingModSimple
	{
		using Osc = OscSine<Float>;
		
		RingModSimple() :
			osc(),
			freqBuffer(),
			freqSmooth{ 20.f, 20.f }
		{}

		void prepare(Float Fs, int blockSize)
		{
			const auto fsInv = static_cast<Float>(1) / Fs;

			for(auto& osci: osc)
				osci.prepare(fsInv);
			freqBuffer.setSize(2, blockSize, false, false, false);
			for (auto& f : freqSmooth)
				f.makeFromDecayInMs(20.f, Fs);
		}
		
		void operator()(Float** samples, int numChannels, int numSamples,
			float** _freq) noexcept
		{
			auto freqBufs = freqBuffer.getArrayOfWritePointers();
			
			for (auto ch = 0; ch < numChannels; ++ch)
			{
				auto freq = _freq[ch];
				
				auto smpls = samples[ch];
				auto freqBuf = freqBufs[ch];
				auto& osci = osc[ch];
				
				freqSmooth[ch](freqBuf, freq, numSamples);
				
				for (auto s = 0; s < numSamples; ++s)
				{
					const auto smpl = smpls[s];
					osci.setFreqHz(freqBuf[s]);
					const auto mod = osci();
					
					smpls[s] = smpl * mod;
				}
			}
		}

	protected:
		std::array<Osc, 2> osc;
		AudioBuffer freqBuffer;
		std::array<Smooth, 2> freqSmooth;
	};
}