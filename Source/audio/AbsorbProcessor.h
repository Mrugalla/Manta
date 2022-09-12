#pragma once
#include "AudioUtils.h"
#include "PRM.h"

namespace audio
{
    class AbsorbProcessor
    {
        struct Textures
        {
            Textures();

            void prepare(float, int);

            /*
            samples, numChannels, numSamples, samplesSC, numChannelsSC, rm, am, shapr, crushr, foldr
            */
            void operator()(float**, int, int, float**, int, float, float, float, float, float) noexcept;

        protected:
            PRM rm, am, shapr, crushr, foldr;
        };

    public:
        AbsorbProcessor();

        void prepare(float, int);

        /*
        samples, numChannels, numSamples, samplesSC, numChannelsSC, rm, am, shapr, crushr, foldr
        */
        void operator()(float**, int, int, float**, int, float, float, float, float, float) noexcept;

    protected:
        Textures textures;
    };

}