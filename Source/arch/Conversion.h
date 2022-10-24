#pragma once
#include <juce_core/juce_core.h>

namespace audio
{
    static constexpr float Tau = 6.28318530718f;
    static constexpr float Pi = 3.14159265359f;
	static constexpr float PiInv = 1.f / Pi;
    static constexpr float PiHalf = Pi * .5f;
    static constexpr float PiHalfInv = 1.f / PiHalf;
    using Char = juce::juce_wchar;
    using String = juce::String;
	
    template<typename Float>
    inline Float secsInSamples(Float secs, Float Fs) noexcept
    {
        return secs * Fs;
    }

    template<typename Float>
    inline Float msInSamples(Float ms, Float Fs) noexcept
    {
        return secsInSamples(ms * static_cast<Float>(.001), Fs);
    }

    template<typename Float>
    inline Float freqHzInSamples(Float hz, Float Fs) noexcept
    {
        return Fs / hz;
    }

    template<typename Float>
    inline float getRMS(const Float* ar, const int size) noexcept
    {
        auto rms = static_cast<Float>(0);
        for (auto i = 0; i < size; ++i)
            rms += ar[i] * ar[i];
        rms /= static_cast<Float>(size);
        return std::sqrt(rms);
    }

    template<typename Float>
    inline Float noteInFreqHz(Float note, Float rootNote = static_cast<Float>(69), Float xen = static_cast<Float>(12), Float masterTune = static_cast<Float>(440)) noexcept
    {
        return std::exp2((note - rootNote) / xen) * masterTune;
    }

	template<typename Float>
    inline Float noteInFreqHz2(Float note, Float rootNote = static_cast<Float>(69), Float masterTune = static_cast<Float>(440)) noexcept
    {
        return std::exp2((note - rootNote) * static_cast<Float>(.08333333333)) * masterTune;
    }

    template<typename Float>
    inline Float freqHzInNote(Float freqHz, Float rootNote = static_cast<Float>(69), Float xen = static_cast<Float>(12), Float masterTune = static_cast<Float>(440)) noexcept
    {
        return std::log2(freqHz / masterTune) * xen + rootNote;
    }

    template<typename Float>
    inline Float freqHzInNote2(Float freqHz, Float xen = static_cast<Float>(12), Float rootNote = static_cast<Float>(69)) noexcept
    {
        return std::log2(freqHz * static_cast<Float>(.00227272727)) * xen + rootNote;
    }

    template<typename Float>
    inline Float freqHzInFc(Float freq, Float Fs) noexcept
    {
		return freq / Fs;
    }

	template<typename Float>
	inline Float fcInFreqHz(Float fc, Float Fs) noexcept
	{
		return fc * Fs;
	}

	template<typename Float>
    inline Float gainToDecibel(Float gain) noexcept
    {
        return std::log10(gain) * static_cast<Float>(20);
    }

	template<typename Float>
	inline Float decibelToGain(Float db) noexcept
	{
		return std::pow(static_cast<Float>(10), db * static_cast<Float>(.05));
	}

    template<typename Float>
    inline Float decibelToGain(Float db, Float threshold) noexcept
    {
        if (db <= threshold)
            return 0.f;
        return std::pow(static_cast<Float>(10), db * static_cast<Float>(.05));
    }

    /* oct [-n, n], semi [-12, 12], fine [-1, 1]*/
    template<typename Float>
    inline Float getRetuneValue(Float oct, Float semi, Float fine) noexcept
    {
        return static_cast<Float>(12) * std::round(oct) + std::round(semi) + fine;
    }

	/* x, a [0, 1[ */
    template<typename Float>
    inline Float softclip(Float x, Float a) noexcept
    {
        const auto l = std::max(std::min(x, a), -a);
        const auto A = 1.f - a;
        const auto X = (x - l) / A;

        return l + A * std::tanh(X);
    }

    template<typename Float>
    inline Float softclip2(Float x_db, Float thresholddB, Float kneedB, Float ratio) noexcept
    {
        // soft knee VCA
        const auto one = static_cast<Float>(1);
        const auto two = static_cast<Float>(2);
        const auto kneeDiv2 = kneedB / two;

        Float gain_sc;
        if (x_db > (thresholddB + kneeDiv2))
        {
            const auto A = x_db - thresholddB;
            gain_sc = thresholddB + (A / ratio);
        }
        else if (x_db > (thresholddB - kneeDiv2))
        {
            const auto A = x_db - thresholddB;
            const auto B = A + kneeDiv2;
            const auto C = B * B;
            gain_sc = x_db + ((one / ratio - one) * C / (two * kneedB));
        }
        else
            gain_sc = x_db;

        return decibelToGain(gain_sc - x_db);
    }
	
	/* db[...,0]db, threshold[...,0]db, ratio [-1, 1], knee [0, 64]*/
	template<typename Float>
    inline Float softclip3(Float xDb, Float threshold, Float ratio, Float knee) noexcept
    {
        const auto half = static_cast<Float>(.5);
        const auto kneeHalf = knee * half;
        const auto thresh2 = threshold - kneeHalf;
        if (xDb < thresh2)
            return xDb;
        
        if (xDb < threshold + kneeHalf)
        {
            const auto one = static_cast<Float>(1);
            const auto two = static_cast<Float>(2);
            const auto c = xDb - threshold + kneeHalf;
			
            return thresh2 + c * (one - (((one - ratio) * c) / (two * knee)));
        }
		
        return threshold + ratio * (xDb - threshold);

    }
	
    inline bool isDigit(Char chr) noexcept
    {
		return chr >= '0' && chr <= '9';
    }

	inline bool isLetter(Char chr) noexcept
	{
		return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
	}

	inline bool isLetterOrDigit(Char chr) noexcept
	{
		return isLetter(chr) || isDigit(chr);
	}

	inline int getDigit(Char chr) noexcept
	{
		return chr - '0';
	}
	
	inline String pitchclassToString(int pitchclass) noexcept
	{
		switch (pitchclass)
		{
		case 0: return "C";
		case 1: return "C#";
		case 2: return "D";
		case 3: return "D#";
		case 4: return "E";
		case 5: return "F";
		case 6: return "F#";
		case 7: return "G";
		case 8: return "G#";
		case 9: return "A";
		case 10: return "A#";
		case 11: return "B";
		default: return "C";
		}
	}

    inline bool isWhiteKey(int pitchclass) noexcept
    {
		switch (pitchclass)
		{
		case 0: return true;
		case 1: return false;
		case 2: return true;
		case 3: return false;
		case 4: return true;
		case 5: return true;
		case 6: return false;
		case 7: return true;
		case 8: return false;
		case 9: return true;
		case 10: return false;
		case 11: return true;
		default: return true;
		}
    }

    inline bool isBlackKey(int pitchclass) noexcept
    {
		return !isWhiteKey(pitchclass);
    }

	template<typename Float>
    void applySomeWindowingFunction(Float* buffer, int size) noexcept
    {
        https://www.desmos.com/calculator/qzrswwvqfo
        const auto a0 = static_cast<Float>(0.35875f);
        const auto a1 = static_cast<Float>(0.48829f);
        const auto a2 = static_cast<Float>(0.14128f);
        const auto a3 = static_cast<Float>(0.01168f);
        const auto inc = Tau / static_cast<Float>(size);
        auto x = static_cast<Float>(0);
        for (auto i = 0; i < size; ++i, x += inc)
        {
            const auto w0 = a1 * std::cos(x);
            const auto w1 = a2 * std::cos(static_cast<Float>(2) * x);
            const auto w2 = a3 * std::cos(static_cast<Float>(3) * x);
            const auto w = a0 - w0 + w1 - a3 * w2;
            buffer[i] *= w;
        }
    }
}