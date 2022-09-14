#pragma once
#include "Filter.h"
#include "PRM.h"
#include "LatencyCompensation.h"
#include "../arch/Interpolation.h"
#include <functional>

namespace audio
{
	struct Manta
	{
		static constexpr int NumLanes = 3;
		static constexpr int MaxSlopeStage = 4; //4*12db/oct
		static constexpr int DelayLength = 240; //ms
		
		static constexpr float DelayLengthHalf = static_cast<float>(DelayLength / 2);

	private:
		class Filter
		{
			using Fltr = FilterBandpassSlope<MaxSlopeStage>;
		public:
			Filter() :
				filta()
			{}

			void operator()(float** laneBuf, float** samples, int numChannels, int numSamples,
				float* fcBuf, float* resoBuf, int stage) noexcept
			{
				{
					auto lane = laneBuf[0];
					auto smpls = samples[0];
					auto& fltr = filta[0];
					
					fltr.setStage(stage);

					for (auto s = 0; s < numSamples; ++s)
					{
						fltr.setFc(fcBuf[s], resoBuf[s]);
						lane[s] = fltr(smpls[s]);
					}
				}
				
				for (auto ch = 1; ch < numChannels; ++ch)
				{
					auto lane = laneBuf[ch];
					auto smpls = samples[ch];
					auto& fltr = filta[ch];

					for (auto s = 0; s < numSamples; ++s)
					{
						fltr.copy(filta[0]);
						lane[s] = fltr(smpls[s]);
					}
				}
			}

		protected:
			std::array<Fltr, 2> filta;
		};
		
		struct Delay
		{
			Delay() :
				ringBuffer(),
				size(0)
			{}

			void prepare(int delaySize)
			{
				size = delaySize;
				ringBuffer.setSize(2, size, false, true, false);
			}

			void operator()(float** samples, int numChannels, int numSamples, const int* wHead, const float* rHead) noexcept
			{
				auto ringBuffr = ringBuffer.getArrayOfWritePointers();

				for (auto ch = 0; ch < numChannels; ++ch)
				{
					auto smpls = samples[ch];
					auto ring = ringBuffr[ch];

					for (auto s = 0; s < numSamples; ++s)
					{
						const auto w = wHead[s];
						const auto r = rHead[s];

						ring[w] = smpls[s];
						smpls[s] = interpolate::lerp(ring, r, size);
					}
				}
			}

			AudioBuffer ringBuffer;
			int size;
		};
		
		struct Lane
		{
			Lane() :
				laneBuffer(),
				readHead(),
				filter(),
				
				frequency(.5f),
				resonance(40.f),
				drive(0.f),
				delay(0.f),
				gain(1.f),
				
				delayFF(),

				Fs(1.f),
				delaySizeF(1.f)
			{}

			void prepare(float sampleRate, int blockSize, int delaySize)
			{
				Fs = sampleRate;

				laneBuffer.setSize(2, blockSize, false, true, false);

				frequency.prepare(Fs, blockSize, 10.f);
				resonance.prepare(Fs, blockSize, 10.f);
				drive.prepare(Fs, blockSize, 10.f);
				delay.prepare(Fs, blockSize, 10.f);
				gain.prepare(Fs, blockSize, 10.f);

				delayFF.prepare(delaySize);
				readHead.resize(blockSize, 0.f);

				delaySizeF = static_cast<float>(delaySize);
			}

			void operator()(float** samples, int numChannels, int numSamples,
				bool enabled, float _frequency, float _resonance, int _slope, float _drive, float _delay, float _gain,
				const int* wHead, const XenManager& xen) noexcept
			{
				auto lane = laneBuffer.getArrayOfWritePointers();

				if (!enabled)
				{
					for (auto ch = 0; ch < numChannels; ++ch)
						SIMD::clear(lane[ch], numSamples);
					return;
				}

				for (auto ch = 0; ch < numChannels; ++ch)
					SIMD::copy(lane[ch], samples[ch], numSamples);

				const auto freqHz = xen.noteToFreqHzWithWrap(_frequency);
				const auto freqFc = freqHzInFc(freqHz, Fs);

				const auto fcBuf = frequency(freqFc, numSamples);
				const auto resoBuf = resonance(_resonance, numSamples);
				const auto driveBuf = drive(_drive, numSamples);
				const auto delayBuf = delay(std::rint(msInSamples(_delay, Fs)), numSamples);
				const auto gainBuf = gain(decibelToGain(_gain), numSamples);
				
				filter
				(
					lane, samples, numChannels, numSamples,
					fcBuf, resoBuf, _slope
				);

				distort(lane, numChannels, numSamples, driveBuf);

				const auto rHead = getRHead(numSamples, wHead, delayBuf);
				delayFF(lane, numChannels, numSamples, wHead, rHead);

				applyGain(lane, numChannels, numSamples, gainBuf);
			}

			void addTo(float** samples, int numChannels, int numSamples) noexcept
			{
				auto lane = laneBuffer.getArrayOfReadPointers();

				for (auto ch = 0; ch < numChannels; ++ch)
					SIMD::add(samples[ch], lane[ch], numSamples);
			}

		protected:
			AudioBuffer laneBuffer;
			std::vector<float> readHead;
			Filter filter;
			PRM frequency, resonance, drive, delay, gain;
			Delay delayFF;
			float Fs, delaySizeF;

			const float* getRHead(int numSamples, const int* wHead, const float* delayBuf) noexcept
			{
				auto rHead = readHead.data();
				for (auto s = 0; s < numSamples; ++s)
				{
					const auto w = static_cast<float>(wHead[s]);
					auto r = w - delayBuf[s];
					if (r < 0.f)
						r += delaySizeF;

					rHead[s] = r;
				}
				return rHead;
			}

			float distort(float x, float d) noexcept
			{
				auto w = std::tanh(128.f * x) / 128.f;
				return x + d * (w - x);
			}

			void distort(float** samples, int numChannels, int numSamples, const float* driveBuf) noexcept
			{
				for (auto ch = 0; ch < numChannels; ++ch)
				{
					auto smpls = samples[ch];

					for (auto s = 0; s < numSamples; ++s)
						smpls[s] = distort(smpls[s], driveBuf[s]);
				}
			}

			void applyGain(float** samples, int numChannels, int numSamples, const float* gainBuf) noexcept
			{
				for (auto ch = 0; ch < numChannels; ++ch)
					SIMD::multiply(samples[ch], gainBuf, numSamples);
			}
		};

	public:
		Manta(const XenManager& _xen) :
			xen(_xen),
			lanes(),
			writeHead(),
			delaySize(1)
		{}

		void prepare(float sampleRate, int blockSize)
		{
			delaySize = static_cast<int>(std::rint(msInSamples(static_cast<float>(DelayLength), sampleRate)));
			if (delaySize % 2 != 0)
				++delaySize;

			for (auto& lane : lanes)
				lane.prepare(sampleRate, blockSize, delaySize);

			writeHead.prepare(blockSize, delaySize);
		}

		/* samples, numChannels, numSamples,
		* l1Enabled [0, 1], l1Frequency [12, N]note, l1Resonance [1, N]q, l1Slope [1, 4]db/oct, l1Drive [0, 1]%, l1Delay [-20, 20]ms, l1Gain [-60, 60]db
		* l2Enabled [0, 1], l2Frequency [12, N]note, l2Resonance [1, N]q, l2Slope [1, 4]db/oct, l2Drive [0, 1]%, l2Delay [-20, 20]ms, l2Gain [-60, 60]db
		* l3Enabled [0, 1], l3Frequency [12, N]note, l3Resonance [1, N]q, l3Slope [1, 4]db/oct, l3Drive [0, 1]%, l3Delay [-20, 20]ms, l3Gain [-60, 60]db
		*/
		void operator()(float** samples, int numChannels, int numSamples,
			bool l1Enabled, float l1Frequency, float l1Resonance, int l1Slope, float l1Drive, float l1Delay, float l1Gain,
			bool l2Enabled, float l2Frequency, float l2Resonance, int l2Slope, float l2Drive, float l2Delay, float l2Gain,
			bool l3Enabled, float l3Frequency, float l3Resonance, int l3Slope, float l3Drive, float l3Delay, float l3Gain) noexcept
		{
			bool enabled[NumLanes] = { l1Enabled, l2Enabled, l3Enabled };
			float frequency[NumLanes] = { l1Frequency, l2Frequency, l3Frequency };
			float resonance[NumLanes] = { l1Resonance, l2Resonance, l3Resonance };
			int slope[NumLanes] = { l1Slope, l2Slope, l3Slope };
			float drive[NumLanes] = { l1Drive, l2Drive, l3Drive };
			float delay[NumLanes] = { l1Delay, l2Delay, l3Delay };
			float gain[NumLanes] = { l1Gain, l2Gain, l3Gain };

			writeHead(numSamples);
			const auto wHead = writeHead.data();

			for (auto i = 0; i < NumLanes; ++i)
			{
				auto& lane = lanes[i];

				lane
				(
					samples,
					numChannels,
					numSamples,
					
					enabled[i],
					frequency[i],
					resonance[i],
					slope[i],
					drive[i],
					delay[i],
					gain[i],
					
					wHead,
					xen
				);
			}
			
			for (auto ch = 0; ch < numChannels; ++ch)
				SIMD::clear(samples[ch], numSamples);
			for (auto& lane : lanes)
				lane.addTo(samples, numChannels, numSamples);
		}
		
	protected:
		const XenManager& xen;
		std::array<Lane, NumLanes> lanes;
		WHead writeHead;
	public:
		int delaySize;
	};
}

/*

todo:

lookahead + lane delays
	lookahead enable button exists (highlevel)
		adds dsp latency to global latency in processor's prepare
		changes wether delay parameters are [-120,120], or [0, 120]

tuningeditor keyboard component must play synth when note pressed
	for previewing current scale
	let play chromatic for previewing current scale

frequency parameter shows in notes and freq
	in regard to xenManager

*/