#pragma once
#include <array>
#include <functional>

#include "AudioUtils.h"
#include "../arch/Interpolation.h"

namespace audio
{
	struct WaveTable
	{
		static constexpr int Size = 1 << 11;
		static constexpr float SizeF = static_cast<float>(Size);
		static constexpr float SizeInv = 1.f / SizeF;

		using Table = std::array<float, Size + 4>;
		using CreatorFunc = std::function<float(float)>;

		WaveTable() :
			table()
		{
			for (auto& t : table)
				t = 0.f;
		}

		void create(const CreatorFunc& func) noexcept
		{
			auto x = -1.f;
			const auto inc = 2.f * SizeInv;
			for (auto s = 0; s < Size; ++s, x += inc)
				table[s] = func(x);

			for (auto i = Size; i < static_cast<int>(table.size()); ++i)
				table[i] = table[i - Size];
		}

		float operator[](int idx) noexcept
		{
			return table[idx];
		}

		float operator[](float phase) noexcept
		{
			const auto idx = phase * SizeF;
			return interpolate::lerp(table.data(), idx);
		}

	protected:
		Table table;
	};

	inline void createWaveTableSine(WaveTable& table)
	{
		table.create([](float x)
		{
			return std::sin(x * Pi);
		});
	}

	inline void createWaveTableSaw(WaveTable& table)
	{
		table.create([](float x)
		{
			return x;
		});
	}

	inline void createWaveTableTriangle(WaveTable& table)
	{
		table.create([](float x)
		{
			return x < -.5f ? -2.f * (x - 1.f) :
				x < .5f ? 2.f : -2.f * (x + 1.f);
		});
	}

	inline void createWaveTableSquare(WaveTable& table)
	{
		table.create([](float x)
		{
			return x < 0.f ? -1.f : 1.f;
		});
	}

}