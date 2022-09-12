#include "PRM.h"

namespace audio
{
	PRM::PRM(float startVal) :
		smooth(startVal),
		buf()
	{}

	void PRM::prepare(float Fs, int blockSize, float smoothLenMs)
	{
		buf.resize(blockSize);
		smooth.makeFromDecayInMs(smoothLenMs, Fs);
	}

	float* PRM::operator()(float value, int numSamples) noexcept
	{
		smooth(buf.data(), value, numSamples);
		return buf.data();
	}

	float* PRM::operator()(int numSamples) noexcept
	{
		smooth(buf.data(), numSamples);
		return buf.data();
	}
}