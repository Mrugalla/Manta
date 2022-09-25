#pragma once
#include "../arch/Smooth.h"

namespace audio
{
	/*
	* expects rectified input signal
	*/
	struct EnvFol
	{
		enum class State { Rise, Fall, NumStates };

		using Smoothie = smooth::Lowpass<float>;

		EnvFol();

		// Fs
		void prepare(float) noexcept;

		/* buf, smpls, numSamples, riseInMs, fallInMs */
		void operator()(float*, const float*, int,
			float, float) noexcept;

		/* smpl, riseInMs, fallInMs */
		float process(float, float, float) noexcept;

	protected:
		Smoothie smoothie;
		float rise, fall, env, Fs;
		State state;

		float processRise(float) noexcept;

		float processFall(float) noexcept;
	};
}