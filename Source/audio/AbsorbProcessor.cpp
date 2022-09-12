#include "AbsorbProcessor.h"

namespace audio
{
    AbsorbProcessor::Textures::Textures() :
        rm(0.f),
        am(0.f),
        shapr(0.f),
        crushr(0.f),
        foldr(0.f)
    {}

    void AbsorbProcessor::Textures::prepare(float sampleRate, int blockSize)
    {
        rm.prepare(sampleRate, blockSize, 10.f);
        am.prepare(sampleRate, blockSize, 10.f);
        shapr.prepare(sampleRate, blockSize, 10.f);
        crushr.prepare(sampleRate, blockSize, 10.f);
        foldr.prepare(sampleRate, blockSize, 10.f);
    }

    void AbsorbProcessor::Textures::operator()(float** samples, int numChannels, int numSamples,
        float** samplesSC, int numChannelsSC,
        float _rm, float _am, float _shapr, float _crushr, float _foldr) noexcept
    {
        auto rmBuf = rm(Decibels::decibelsToGain(_rm, -20.f), numSamples);
        auto amBuf = am(Decibels::decibelsToGain(_am, -20.f), numSamples);
        auto shaprBuf = shapr(Decibels::decibelsToGain(_shapr, -40.f), numSamples);
        auto crushrBuf = crushr(Decibels::decibelsToGain(_crushr, -40.f), numSamples);
        auto foldrBuf = foldr(Decibels::decibelsToGain(_foldr, -40.f), numSamples);

        for (auto ch = 0; ch < numChannels; ++ch)
        {
            const auto chSC = ch % numChannelsSC;
            const auto smplsSC = samplesSC[chSC];

            auto smpls = samples[ch];

            float A, B, C, D, E;

            for (auto s = 0; s < numSamples; ++s)
            {
                const auto main = smpls[s];
                const auto sc = smplsSC[s];

                A = main * sc * rmBuf[s];

                const auto scsc = sc * sc;
                const auto scscSqrt = std::sqrt(scsc);

                B = main * scscSqrt * amBuf[s];

                if (sc == 0.f)
                {
                    C = D = E = 0.f;
                }
                else
                {
                    const auto mainInvSc = main / sc;

                    C = std::tanh(mainInvSc) * sc * shaprBuf[s];
                    D = std::rint(mainInvSc) * sc * crushrBuf[s];

                    E = std::fmod(main, sc) * sc * foldrBuf[s];
                }

                smpls[s] = A + B + C + D + E;
            }
        }
    }

    //

    AbsorbProcessor::AbsorbProcessor() :
        textures()
    {
    }

    void AbsorbProcessor::prepare(float sampleRate, int blockSize)
    {
        textures.prepare(sampleRate, blockSize);
    }

    void AbsorbProcessor::operator()(float** samples, int numChannels, int numSamples,
        float** samplesSC, int numChannelsSC,
        float _rm, float _am, float _shapr, float _crushr, float _foldr) noexcept
    {
        textures(samples, numChannels, numSamples, samplesSC, numChannelsSC,
            _rm, _am, _shapr, _crushr, _foldr);
    }
}