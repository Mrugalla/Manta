#pragma once
#include "WHead.h"
#include "EnvelopeFollower.h"
#include <array>
#include <atomic>

namespace audio
{
	class Meters
	{
		static constexpr float RiseInMs = .01f, FallInMs = 42.f;

		struct Val
		{
			Val();

			float rect, val;
			std::atomic<float> env;
			EnvFol envFol;
		};
	
	public:
		enum Type
		{
#if PPDHasGainIn
			In,
#endif
			Out,
			NumTypes
		};
	
		Meters();

		/*sampleRate, blockSize*/
		void prepare(float, int);

#if PPDHasGainIn
		/*samples,numChannels,numSamples*/
		void processIn(const float**, int, int) noexcept;
#endif
		/*samples,numChannels,numSamples*/
		void processOut(const float**, int, int) noexcept;

		const std::atomic<float>& operator()(int i) const noexcept;

	protected:
		std::array<Val, NumTypes> vals;
		WHead wHead;
		float lenInv;
		int length;

	private:
		/*val,samples,numChannels,numSamples*/
		void process(Val&, const float**, int, int) noexcept;
	};
}