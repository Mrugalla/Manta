#pragma once
#include "Filter.h"
#include "PRM.h"

namespace audio
{
	struct Manta
	{
		struct Filter
		{
			Filter() :
				filter()
			{}

			void setFc(float fc, float q) noexcept
			{
				filter[0].setFc(fc, q);
				filter[1].copy(filter[0]);
			}
			
			void operator()(float** samples, int numChannels, int numSamples, bool enabled) noexcept
			{
				if(enabled)
					for (auto ch = 0; ch < numChannels; ++ch)
					{
						auto smpls = samples[ch];
						auto& fltr = filter[ch];

						for (auto s = 0; s < numSamples; ++s)
							smpls[s] = fltr(smpls[s]);
					}
			}

		protected:
			std::array<FilterBandpass, 2> filter;
		};
		
		Manta() :
			filter(),
			
			cutoff(freqHzInFc(420.f, 44100.f)),
			q(40.f),
			gain(1.f)
		{}

		void prepare(float, int)
		{
			
		}

		void operator()(float** samples, int numChannels, int numSamples) noexcept
		{
			filter(samples, numChannels, numSamples, true);
		}

	protected:
		Filter filter;

		PRM cutoff, q, gain;
	};
}