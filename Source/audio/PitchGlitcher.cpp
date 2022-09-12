#include "PitchGlitcher.h"

namespace audio
{
	// PitchGlitcher::Phasor

	PitchGlitcher::Phasor::Phasor() :
		inc(),
		buf(),
		phase(0.f)
	{
	}

	void PitchGlitcher::Phasor::prepare(int blockSize)
	{
		inc.resize(blockSize);
		buf.resize(blockSize);
	}

	void PitchGlitcher::Phasor::operator()(int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
		{
			buf[s] = phase;

			phase += inc[s];
			if (phase >= 1.f)
				--phase;
			if (phase < 0.f)
				++phase;
		}
	}

	const float* PitchGlitcher::Phasor::data() const noexcept
	{
		return buf.data();
	}

	float& PitchGlitcher::Phasor::operator[](int i) noexcept
	{
		return buf[i];
	}

	const float& PitchGlitcher::Phasor::operator[](int i) const noexcept
	{
		return buf[i];
	}

	// PitchGlitcher::Window

	PitchGlitcher::Window::Window() :
		table(),
		buf(),
		tableSizeF(static_cast<float>(TableSize))
	{
		for (auto i = 0; i < TableSize; ++i)
		{
			const auto x = static_cast<float>(i) / static_cast<float>(table.size());
			table[i] = std::cos(x * Tau + Pi) * .5f + .5f;
		}
	}

	void PitchGlitcher::Window::prepare(int blockSize)
	{
		buf.resize(blockSize);
	}

	void PitchGlitcher::Window::operator()(const float* phasor, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
		{
			const auto idx = phasor[s] * tableSizeF;
			buf[s] = interpolate::lerp(table.data(), idx, TableSize);
		}
	}

	const float* PitchGlitcher::Window::data() const noexcept
	{
		return buf.data();
	}

	// PitchGlitcher::ReadHead

	PitchGlitcher::ReadHead::ReadHead() :
		buf(),
		sizeF(0.f)
	{}

	void PitchGlitcher::ReadHead::prepare(int blockSize, float _sizeF) noexcept
	{
		buf.resize(blockSize);
		sizeF = _sizeF;
	}

	void PitchGlitcher::ReadHead::operator()(const int* wHead, const float* modulator, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
		{
			auto r = static_cast<float>(wHead[s]) - modulator[s] * sizeF;
			if (r < 0.)
				r += sizeF;
			buf[s] = r;
		}
	}

	const float* PitchGlitcher::ReadHead::data() const noexcept
	{
		return buf.data();
	}

	// PitchGlitcher::Delay

	PitchGlitcher::Delay::Delay() :
		ringBuffer(),

		sizeF(0.f),
		size(0)
	{
	}

	void PitchGlitcher::Delay::prepare(int _size)
	{
		size = _size;
		for (auto& ch : ringBuffer)
			ch.resize(size + 4, 0.f);
	}

	void PitchGlitcher::Delay::operator()(float** samples, int numChannels, int numSamples,
		const int* wHead, const float* readHead/*[0, size[*/,
		const float* window, float feedback/*[-1,1]*/) noexcept
	{
		for (auto ch = 0; ch < numChannels; ++ch)
		{
			auto smpls = samples[ch];
			auto ring = ringBuffer[ch].data();

			for (auto s = 0; s < numSamples; ++s)
			{
				const auto w = wHead[s];
				const auto r = readHead[s];
				const auto wndw = window[s];

				const auto sOut = interpolate::lerp(ring, r, size) * wndw;
				const auto sIn = smpls[s] + sOut * feedback;

				ring[w] = sIn;
				smpls[s] = sOut;
			}
		}
	}

	// PitchGlitcher::Shifter

	PitchGlitcher::Shifter::Shifter() :
		audioBuffer(),

		phasor(),
		window(),
		readHead(),
		delay(),

		tuneParam(0.f),

		sizeInv(1.f),
		Fs(0.f)
	{
	}

	void PitchGlitcher::Shifter::prepare(float _Fs, int blockSize, int size)
	{
		Fs = _Fs;

		audioBuffer.setSize(2, blockSize, true, true, false);

		phasor.prepare(blockSize);
		window.prepare(blockSize);
		readHead.prepare(blockSize, static_cast<float>(size));
		delay.prepare(size);

		sizeInv = 1.f / static_cast<float>(size);

		tuneParam.prepare(Fs, blockSize, 70.f);
	}

	void PitchGlitcher::Shifter::operator()(float** samples, int numChannels, int numSamples,
		const int* wHead, const float* grainBuf,
		float tune, float feedback) noexcept
	{
		for (auto ch = 0; ch < numChannels; ++ch)
		{
			auto buffer = audioBuffer.getWritePointer(ch);
			const auto smpls = samples[ch];

			SIMD::copy(buffer, smpls, numSamples);
		}

		const auto tuneN = tune * Inv12;
		const auto tuneBuf = tuneParam(std::pow(2.f, tuneN), numSamples);

		for (auto s = 0; s < numSamples; ++s)
			phasor.inc[s] = (1.f - tuneBuf[s]) / grainBuf[s];

		phasor(numSamples);

		window(phasor.data(), numSamples);

		for (auto s = 0; s < numSamples; ++s)
			phasor[s] *= grainBuf[s] * sizeInv;

		readHead(wHead, phasor.data(), numSamples);

		delay
		(
			audioBuffer.getArrayOfWritePointers(),
			numChannels,
			numSamples,
			wHead,
			readHead.data(),
			window.data(),
			feedback
		);
	}

	void PitchGlitcher::Shifter::copyTo(float** samples, int numChannels, int numSamples) noexcept
	{
		for (auto ch = 0; ch < numChannels; ++ch)
		{
			const auto buffer = audioBuffer.getReadPointer(ch);
			auto smpls = samples[ch];

			SIMD::copy(smpls, buffer, numSamples);
		}
	}

	void PitchGlitcher::Shifter::addTo(float** samples, int numChannels, int numSamples) noexcept
	{
		for (auto ch = 0; ch < numChannels; ++ch)
		{
			const auto buffer = audioBuffer.getReadPointer(ch);
			auto smpls = samples[ch];

			SIMD::add(smpls, buffer, numSamples);
		}
	}

	// PitchGlitcher

	PitchGlitcher::PitchGlitcher() :
		wHead(),

		shifter(),

		grainParam(20.f),

		Fs(0.f)
	{}

	void PitchGlitcher::prepare(float _Fs, int _blockSize)
	{
		Fs = _Fs;

		const auto size = static_cast<int>(msInSamples(static_cast<float>(PPDPitchShifterSizeMs), Fs));

		wHead.prepare(_blockSize, size);

		for (auto& s : shifter)
			s.prepare(Fs, _blockSize, size);

		grainParam.prepare(Fs, _blockSize, 140.f);
	}

	void PitchGlitcher::operator()(float** samples, int numChannels, int numSamples,
		float tuneP/*[-24,24]*/, float grainSizeP/*[0, sizeF]*/, float feedbackP/*[0,1]*/,
		int numVoicesP/*[1,NumVoices]*/, float spreadTuneP/*[0,1]*/) noexcept
	{
		wHead(numSamples);

		const auto grainBuf = grainParam(msInSamples(grainSizeP, Fs), numSamples);

		shifter[0]
		(
			samples, numChannels, numSamples,
			wHead.data(), grainBuf,
			tuneP, feedbackP
			);

		for (auto i = 1; i < numVoicesP; ++i)
		{
			auto& shftr = shifter[i];

			const auto flip = i % 2 == 0 ? 1.f : -1.f;
			const auto x = static_cast<float>(i) / numVoicesP;

			const auto spreadTune = x * spreadTuneP * flip;

			shftr
			(
				samples, numChannels, numSamples,
				wHead.data(), grainBuf,
				tuneP + spreadTune, feedbackP
			);
		}

		shifter[0].copyTo(samples, numChannels, numSamples);
		for (auto i = 1; i < numVoicesP; ++i)
			shifter[i].addTo(samples, numChannels, numSamples);

		const auto gain = 1.f / std::sqrt(static_cast<float>(numVoicesP));
		for (auto ch = 0; ch < numChannels; ++ch)
			SIMD::multiply(samples[ch], gain, numSamples);
	}
}