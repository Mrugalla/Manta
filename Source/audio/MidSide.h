#pragma once

namespace audio
{
	/*samples, numSamples, chStart*/
	void encodeMS(float**, int, int) noexcept;
	
	/*samples, numSamples, chStart*/
	void decodeMS(float**, int, int) noexcept;
}