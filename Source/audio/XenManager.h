#pragma once
#include "../arch/Conversion.h"
#include "../arch/Interpolation.h"
#include "../arch/State.h"
#include <array>
#include <atomic>

namespace audio
{
	struct XenManager
	{
		XenManager() :
			xen(12.f),
			masterTune(440.f),
			baseNote(69.f),
			temperaments()
		{
			for (auto& t : temperaments)
				t = 0.f;
		}

		void setTemperament(float tmprVal, int noteVal) noexcept
		{
			temperaments[noteVal] = tmprVal;
			const auto idx2 = noteVal + PPD_MaxXen;
			if (idx2 >= temperaments.size())
				temperaments[idx2] = tmprVal;
		}
		
		void operator()(float _xen, float _masterTune, float _baseNote) noexcept
		{
			xen = _xen;
			masterTune = _masterTune;
			baseNote = _baseNote;
		}

		template<typename Float>
		Float noteToFreqHz(Float note) const noexcept
		{
			const auto noteCap = juce::jlimit(static_cast<Float>(0), static_cast<Float>(PPD_MaxXen), note);
			const auto tmprmt = temperaments[static_cast<int>(std::round(noteCap))].load();

			return noteInFreqHz(note + tmprmt, baseNote, xen, masterTune);
		}

		template<typename Float>
		Float noteToFreqHzWithWrap(Float note, Float lowestFreq = static_cast<Float>(0), Float highestFreq = static_cast<Float>(22000)) const noexcept
		{
			auto freq = noteToFreqHz(note);
			while (freq < lowestFreq)
				freq *= static_cast<Float>(2);
			while (freq >= highestFreq)
				freq *= static_cast<Float>(.5);
			return freq;
		}

		template<typename Float>
		Float freqHzToNote(Float hz) noexcept
		{
			return freqHzInNote(hz, baseNote, xen, masterTune);
		}
		
		float getXen() const noexcept
		{
			return xen;
		}

	protected:
		float xen, masterTune, baseNote;
		std::array<std::atomic<float>, PPD_MaxXen + 1> temperaments;
	};
	
}

#include "Oscillator.h"

namespace audio
{
	struct TuningEditorSynth
	{
		using SIMD = juce::FloatVectorOperations;

		TuningEditorSynth(const XenManager& _xen) :
			pitch(69.f),
			gain(.25f),
			noteOn(false),
			
			xen(_xen),
			osc(),
			buffer()
		{

		}

		void loadPatch(sta::State& state)
		{
			const auto idStr = getIDString();
			auto g = state.get(idStr, "gain");
			if (g != nullptr)
				gain.store(static_cast<float>(*g));
		}

		void savePatch(sta::State& state)
		{
			const auto idStr = getIDString();
			state.set(idStr, "gain", gain.load());
		}

		void prepare(float Fs, int blockSize)
		{
			const auto fsInv = 1.f / Fs;
			osc.prepare(fsInv);

			buffer.resize(blockSize, 0.f);
		}

		void operator()(float** samples, int numChannels, int numSamples) noexcept
		{
			if (noteOn.load())
			{
				auto buf = buffer.data();

				auto g = gain.load();

				const auto freqHz = xen.noteToFreqHzWithWrap(pitch.load());
				osc.setFreqHz(freqHz);

				for (auto s = 0; s < numSamples; ++s)
					buf[s] = std::tanh(4.f * osc()) * g;

				for (auto ch = 0; ch < numChannels; ++ch)
					SIMD::add(samples[ch], buf, numSamples);
			}
		}

		std::atomic<float> pitch, gain;
		std::atomic<bool> noteOn;
	protected:
		const XenManager& xen;
		OscSine<float> osc;
		std::vector<float> buffer;

		static String getIDString()
		{
			return "tuningEditor";
		}
	};
}