#pragma once
#include <array>
#include <functional>

#include "AudioUtils.h"
#include "../arch/Interpolation.h"
#include "../arch/State.h"

namespace audio
{
	template<size_t Size>
	struct WaveTable
	{
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;
		static constexpr int NumExtraSamples = 4;
		static constexpr int FullSize = Size + NumExtraSamples;

		using Table = std::array<float, FullSize>;
		using Func = std::function<float(float)>;

		WaveTable() :
			table()
		{
			create([](float x) { return std::cos(x * Pi); });
		}

		void create(const Func& func) noexcept
		{
			auto x = -1.f + SizeInv * .5f;
			const auto inc = 2.f * SizeInv;
			for (auto s = 0; s < Size; ++s, x += inc)
				table[s] = func(x);
			for (auto i = 0; i < NumExtraSamples; ++i)
				table[Size + i] = table[i];
		}

		void savePatch(sta::State& state, const String& key)
		{
			juce::MemoryBlock mb;
			const auto dataSize = FullSize * sizeof(float);
			mb.append(table.data(), dataSize);
			const auto base64 = mb.toBase64Encoding();
			state.set(key, "wt", base64, false);
		}

		void loadPatch(sta::State& state, const String& key)
		{
			auto var = state.get(key, "wt");
			if (var != nullptr)
			{
				const auto base64 = var->toString();
				juce::MemoryBlock mb;
				mb.fromBase64Encoding(base64);
#if JUCE_DEBUG
				const auto mbSize = mb.getSize();
#endif
				const auto dataSize = FullSize * sizeof(float);
				jassert(mbSize == dataSize);
				mb.copyTo(table.data(), 0, dataSize);
			}
		}

		float operator()(int idx) const noexcept
		{
			return table[idx];
		}

		float operator()(float phase) const noexcept
		{
			const auto idx = phase * SizeF;
			return interpolate::lerp(table.data(), idx);
		}

		float* data() noexcept
		{
			return table.data();
		}

		const float* data() const noexcept
		{
			return table.data();
		}
		
	protected:
		Table table;
	};

	template<size_t Size>
	inline void createWaveTableSine(WaveTable<Size>& table)
	{
		table.create([](float x)
		{
			return std::sin(x * Pi);
		});
	}

	template<size_t Size>
	inline void createWaveTableSaw(WaveTable<Size>& table)
	{
		table.create([](float x)
		{
			return x;
		});
	}

	template<size_t Size>
	inline void createWaveTableTriangle(WaveTable<Size>& table)
	{
		table.create([](float x)
		{
			return 2.f * std::asin(std::sin(x * Pi)) * PiInv;
		});
	}

	template<size_t Size>
	inline void createWaveTableSquare(WaveTable<Size>& table)
	{
		table.create([](float x)
		{
			return 1.f - 2.f * std::fmod(std::floor(x), 2.f);
		});
	}

	template<size_t Size>
	inline void createWaveTableNoise(WaveTable<Size>& table)
	{
		table.create([](float x)
		{
			juce::Random rand;
			return rand.nextFloat() * 2.f - 1.f;
		});
	}

}