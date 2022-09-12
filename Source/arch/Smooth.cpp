#include "Smooth.h"
#include <cmath>
#include <juce_audio_basics/juce_audio_basics.h>

namespace smooth
{
	using SIMD = juce::FloatVectorOperations;
	
	// Block
	
	template<typename Float>
	Block<Float>::Block(float startVal) :
		curVal(startVal)
	{
		
	}
	
	template<typename Float>
	void Block<Float>::operator()(Float* bufferOut, Float* bufferIn, int numSamples) noexcept
	{
		auto x = static_cast<Float>(0);
		auto inc = 1.f / static_cast<Float>(numSamples);
		
		for (auto s = 0; s < numSamples; ++s, x += inc)
		{
			curVal += inc;
			const auto sIn = bufferIn[s];
			
			bufferOut[s] = curVal + x * (sIn - curVal);
		}
		
		curVal = bufferOut[numSamples - 1];
	}
	
	template<typename Float>
	void Block<Float>::operator()(Float* buffer, Float val, int numSamples) noexcept
	{
		if (curVal == val)
			return operator()(buffer, numSamples);
		
		const auto dist = val - curVal;
		const auto inc = dist / static_cast<Float>(numSamples);
		
		for (auto s = 0; s < numSamples; ++s)
		{
			buffer[s] = curVal;
			curVal += inc;
		}
	}
	
	template<typename Float>
	void Block<Float>::operator()(Float* buffer, int numSamples) noexcept
	{
		SIMD::fill(buffer, curVal, numSamples);
	}
	
	template struct Block<float>;
	template struct Block<double>;

	// Lowpass
	
	template<typename Float>
	Float Lowpass<Float>::getXFromFc(Float fc) noexcept
	{
		return std::exp(-Tau * fc);
	}

	template<typename Float>
	Float Lowpass<Float>::getXFromHz(Float d, Float Fs) noexcept
	{
		return getXFromFc(d / Fs);
	}

	//
	
	template<typename Float>
	void Lowpass<Float>::makeFromDecayInSamples(Float d) noexcept
	{
		setX(std::exp(static_cast<Float>(-1) / d));
	}

	template<typename Float>
	void Lowpass<Float>::makeFromDecayInSecs(Float d, Float Fs) noexcept
	{
		makeFromDecayInSamples(d * Fs);
	}

	template<typename Float>
	void Lowpass<Float>::makeFromDecayInFc(Float fc) noexcept
	{
		getXFromFc(fc);
	}

	template<typename Float>
	void Lowpass<Float>::makeFromDecayInHz(Float d, Float Fs) noexcept
	{
		getXFromHz(d, Fs);
	}

	template<typename Float>
	void Lowpass<Float>::makeFromDecayInMs(Float d, Float Fs) noexcept
	{
		makeFromDecayInSamples(d * Fs * static_cast<Float>(.001));
	}

	template<typename Float>
	void Lowpass<Float>::copyCutoffFrom(const Lowpass<Float>& other) noexcept
	{
		a0 = other.a0;
		b1 = other.b1;
		eps = other.eps;
	}

	template<typename Float>
	Lowpass<Float>::Lowpass(const Float _startVal) :
		a0(static_cast<Float>(1)),
		b1(static_cast<Float>(0)),
		y1(_startVal),
		eps(static_cast<Float>(0)),
		startVal(_startVal)
	{}

	template<typename Float>
	void Lowpass<Float>::reset()
	{
		a0 = static_cast<Float>(1);
		b1 = static_cast<Float>(0);
		y1 = startVal;
		eps = static_cast<Float>(0);
	}

	template<typename Float>
	void Lowpass<Float>::operator()(Float* buffer, Float val, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
			buffer[s] = processSample(val);
	}

	template<typename Float>
	void Lowpass<Float>::operator()(Float* buffer, int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s)
			buffer[s] = processSample(buffer[s]);
	}

	template<typename Float>
	Float Lowpass<Float>::operator()(Float sample) noexcept
	{
		return processSample(sample);
	}

	template<typename Float>
	Float Lowpass<Float>::processSample(Float x0) noexcept
	{
		y1 = x0 * a0 + y1 * b1;
		return y1;
	}

	template<typename Float>
	void Lowpass<Float>::setX(Float x) noexcept
	{
		a0 = static_cast<Float>(1) - x;
		b1 = x;
		eps = a0 * static_cast<Float>(1.5);
	}

	template struct Lowpass<float>;
	template struct Lowpass<double>;

	// Smooth

	template<typename Float>
	void Smooth<Float>::makeFromDecayInMs(Float smoothLenMs, Float Fs)
	{
		lowpass.makeFromDecayInMs(smoothLenMs, Fs);
	}
	
	template<typename Float>
	Smooth<Float>::Smooth(float startVal) :
		block(startVal),
		lowpass(startVal)
	{
	}

	template<typename Float>
	void Smooth<Float>::operator()(Float* bufferOut, Float* bufferIn, int numSamples) noexcept
	{
		block(bufferOut, bufferIn, numSamples);
		lowpass(bufferOut, numSamples);
	}
	
	template<typename Float>
	void Smooth<Float>::operator()(Float* buffer, Float val, int numSamples) noexcept
	{
		block(buffer, val, numSamples);
		lowpass(buffer, numSamples);
	}
	
	template<typename Float>
	void Smooth<Float>::operator()(Float* buffer, int numSamples) noexcept
	{
		block(buffer, numSamples);
		lowpass(buffer, numSamples);
	}
	
	template struct Smooth<float>;
	template struct Smooth<double>;
}