#pragma once
#include <array>
#include "WHead.h"
#include "AudioUtils.h"
#include "../arch/Interpolation.h"
#include "PRM.h"
#include "MIDIManager.h"
#include "XenManager.h"

namespace audio
{
	
	class Resonator
	{
		struct DelayFeedback
		{
			using Lowpass = smooth::Lowpass<float>;

			DelayFeedback() :
				ringBuffer(),
				lowpass{ 0.f, 0.f },
				size(0)
			{}

			void prepare(float Fs, int _size)
			{
				size = _size;

				ringBuffer.setSize(2, size, false, true, false);

				for (auto& lp : lowpass)
					lp.makeFromDecayInHz(1000.f, Fs);
			}

			//samples, numChannels, numSamples, wHead, feedbackBuffer[-1,1], readHead
			void operator()(float** samples, int numChannels, int numSamples,
				const int* wHead, const float* fbBuf, const float* dampBuf,
				const float** readHead) noexcept
			{
				auto ringBuf = ringBuffer.getArrayOfWritePointers();
				
				for (auto ch = 0; ch < numChannels; ++ch)
				{
					auto smpls = samples[ch];
					auto ring = ringBuf[ch];
					const auto rHead = readHead[ch];
					auto& lp = lowpass[ch];

					for (auto s = 0; s < numSamples; ++s)
					{
						lp.setX(dampBuf[s]);

						const auto w = wHead[s];
						const auto r = rHead[s];
						const auto fb = fbBuf[s];

						auto sOut = lp(interpolate::cubicHermiteSpline(ring, r, size));
						const auto sIn = smpls[s] + sOut * fb;
						
						ring[w] = sIn;
						smpls[s] = sOut;
					}
				}
			}

		protected:
			AudioBuffer ringBuffer;
			std::array<Lowpass, 2> lowpass;
			int size;
		};

		static constexpr float LowestFrequencyHz = 20.f;

	public:
		Resonator(MIDIVoices& _midiVoices, const XenManager& _xenManager) :
			midiVoices(_midiVoices),
			xenManager(_xenManager),

			writeHead(),
			readHeadBuffer(),
			delay(),

			feedbackP(0.f),
			dampP(1.f),
			retuneP(0.f),
			
			Fs(0.f), sizeF(0.f), curDelay(0.f), curNote(48.f),
			size(0)
		{}

		void prepare(float sampleRate, int blockSize)
		{
			Fs = sampleRate;

			sizeF = std::ceil(freqHzInSamples(LowestFrequencyHz, Fs));
			size = static_cast<int>(sizeF);

			writeHead.prepare(blockSize, size);
			readHeadBuffer.setSize(2, blockSize, false, false, false);
			delay.prepare(Fs, size);

			const auto freqHz = xenManager.noteToFreqHzWithWrap(curNote, LowestFrequencyHz);
			curDelay = freqHzInSamples(freqHz, Fs);

			feedbackP.prepare(sampleRate, blockSize, 10.f);
			dampP.prepare(sampleRate, blockSize, 10.f);
			retuneP.prepare(sampleRate, blockSize, 10.f);
		}

		void operator()(float** samples, int numChannels, int numSamples,
			float _feedback /*]-1,1[*/, float _damp /*]0, 22050[hz*/, float _retune/*[-n,n]semi*/) noexcept
		{
			writeHead(numSamples);
			const auto wHead = writeHead.data();

			const auto retuneBuf = retuneP(_retune, numSamples);

			{ // calculate readhead indexes from note buffer
				auto rHeadBuf = readHeadBuffer.getArrayOfWritePointers();

				auto& noteBuffer = midiVoices.voices[0].buffer;
				
				for (auto s = 0; s < numSamples; ++s)
				{
					auto nNote = static_cast<float>(noteBuffer[0].noteNumber);
					const auto pb = midiVoices.pitchbendBuffer.buffer[s];
					
					nNote = juce::jlimit(1.f, 127.f, nNote + retuneBuf[s] + pb);
					
					if (curNote != nNote)
					{
						curNote = nNote;
						const auto freqHz = xenManager.noteToFreqHzWithWrap(curNote, LowestFrequencyHz);
						curDelay = freqHzInSamples(freqHz, Fs);
					}

					const auto w = static_cast<float>(wHead[s]);
					auto r = w - curDelay;
					if (r < 0.f)
						r += sizeF;

					rHeadBuf[0][s] = r;
				}
				
				for (auto ch = 1; ch < numChannels; ++ch) // only until i have a better idea
					SIMD::copy(rHeadBuf[ch], rHeadBuf[0], numSamples);
			}

			const auto fbBuf = feedbackP(_feedback, numSamples);
			const auto rHeadBufConst = readHeadBuffer.getArrayOfReadPointers();

			const auto xFromHz = smooth::Lowpass<float>::getXFromHz(_damp, Fs);
			const auto dampBuf = dampP(xFromHz, numSamples);

			delay(samples, numChannels, numSamples,
				wHead, fbBuf, dampBuf, rHeadBufConst);
		}

	protected:
		MIDIVoices& midiVoices;
		const XenManager& xenManager;
		
		WHead writeHead;
		AudioBuffer readHeadBuffer;
		DelayFeedback delay;

		PRM feedbackP, dampP, retuneP;
		
		float Fs, sizeF, curDelay, curNote;
		int size;
	};
}