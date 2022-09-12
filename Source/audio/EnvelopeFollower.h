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

		EnvFol() :
			smoothie(0.f),
			rise(1.f),
			fall(1.f),
			env(0.f),
			Fs(1.f),
			state(State::Fall)
		{}

		void prepare(float _Fs) noexcept
		{
			Fs = _Fs;
			switch (state)
			{
			case State::Rise: return smoothie.makeFromDecayInMs(rise, Fs);
			case State::Fall: return smoothie.makeFromDecayInMs(fall, Fs);
			}
		}

		void operator()(float* buf, const float* smpls, int numSamples,
			float _riseInMs, float _fallInMs) noexcept
		{
			for (auto s = 0; s < numSamples; ++s)
				buf[s] = process(smpls[s], _riseInMs, _fallInMs);
		}

		float process(float smpl,
			float _riseInMs, float _fallInMs) noexcept
		{
			rise = _riseInMs;
			fall = _fallInMs;
			
			switch (state)
			{
			case State::Rise: return processRise(smpl);
			case State::Fall: return processFall(smpl);
			default: return smpl;
			}
		}

	protected:
		Smoothie smoothie;
		float rise, fall, env, Fs;
		State state;

		float processRise(float smpl) noexcept
		{
			if(env > smpl)
			{
				state = State::Fall;
				smoothie.makeFromDecayInMs(fall, Fs);
			}
			env = smoothie(smpl);
			return env;
		}

		float processFall(float smpl) noexcept
		{
			if (env < smpl)
			{
				state = State::Rise;
				smoothie.makeFromDecayInMs(rise, Fs);
			}
			env = smoothie(smpl);
			return env;
		}
	};
}