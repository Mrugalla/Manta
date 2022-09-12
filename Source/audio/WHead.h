#pragma once
#include <vector>

namespace audio
{
	struct WHead
	{
		WHead();

		/* blockSize, delaySize */
		void prepare(int, int);

		/* numSamples */
		void operator()(int numSamples) noexcept;

		int operator[](int) const noexcept;

		const int* data() const noexcept;
	protected:
		std::vector<int> buf;
		int wHead, delaySize;
	};
}