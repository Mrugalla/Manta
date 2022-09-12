#pragma once
#include "WHead.h"
#include "PRM.h"
#include "../arch/Interpolation.h"
#include "AudioUtils.h"

#include <array>

namespace audio
{
	class PitchGlitcher
	{
		static constexpr int NumVoices = PPDPitchShifterNumVoices;
		static constexpr float Inv12 = 1.f / 12.f;

		struct Phasor
		{
			Phasor();

			/*blockSize*/
			void prepare(int);

			/*numSamples*/
			void operator()(int) noexcept;

			const float* data() const noexcept;

			float& operator[](int i) noexcept;
			const float& operator[](int i) const noexcept;

			std::vector<float> inc;
		protected:
			std::vector<float> buf;
			float phase;
		};

		struct Window
		{
			static constexpr int TableSize = 1 << 13;

			Window();

			/*blockSize*/
			void prepare(int);

			/*phasor, numSamples*/
			void operator()(const float*, int) noexcept;

			const float* data() const noexcept;

		protected:
			std::array<float, TableSize> table;
			std::vector<float> buf;
			float tableSizeF;
		};

		struct ReadHead
		{
			ReadHead();

			/*blockSize, sizeF*/
			void prepare(int, float) noexcept;

			/*wHead, modulator, numSamples*/
			void operator()(const int*, const float*, int) noexcept;

			const float* data() const noexcept;

		protected:
			std::vector<float> buf;
			float sizeF;
		};

		struct Delay
		{
			Delay();

			/*size*/
			void prepare(int);

			/*samples, numChannels, numSamples, wHead, readHead [0, size[, window, feedback[-1,1]*/
			void operator()(float**, int, int, const int*, const float*, const float*, float) noexcept;

		protected:
			std::array<std::vector<float>, 2> ringBuffer;

			float sizeF;
			int size;
		};

		struct Shifter
		{
			Shifter();

			/*Fs, blockSize, size*/
			void prepare(float, int, int);

			/*samples, numChannels, numSamples, wHead, grainBuf, tune, feedback*/
			void operator()(float**, int, int, const int*, const float*, float, float) noexcept;

			/*samples, numChannels, numSamples*/
			void copyTo(float**, int, int) noexcept;

			/*samples, numChannels, numSamples*/
			void addTo(float**, int, int) noexcept;

		protected:
			AudioBuffer audioBuffer;

			Phasor phasor;
			Window window;
			ReadHead readHead;
			Delay delay;

			PRM tuneParam;

			float sizeInv, Fs;
		};

	public:
		PitchGlitcher();

		/*Fs, blockSize*/
		void prepare(float, int);

		/*samples, numChannels, numSamples, tuneP[-24,24], grainSizeP[0, sizeF], feedback[0,1], numVoicesP[1,NumVoices],spreadTuneP[0,1]*/
		void operator()(float**, int, int, float, float, float, int, float) noexcept;

	protected:
		WHead wHead;
		
		std::array<Shifter, NumVoices> shifter;

		PRM grainParam;

		float Fs;
	};
}



/*

voice 1 need no copy to self buffer

alter pitch per voice instead of grain size

*/