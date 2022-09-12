#include "WHead.h"

namespace audio
{

	WHead::WHead() :
		buf(),
		wHead(0),
		delaySize(1)
	{}

	void WHead::prepare(int blockSize, int _delaySize)
	{
		delaySize = _delaySize;
		if (delaySize != 0)
		{
			wHead = wHead % delaySize;
			buf.resize(blockSize);
		}
	}

	void WHead::operator()(int numSamples) noexcept
	{
		for (auto s = 0; s < numSamples; ++s, wHead = (wHead + 1) % delaySize)
			buf[s] = wHead;
	}

	int WHead::operator[](int i) const noexcept
	{
		return buf[i];
	}

	const int* WHead::data() const noexcept
	{
		return buf.data();
	}

}