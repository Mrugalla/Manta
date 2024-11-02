#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_events/juce_events.h>

#include "audio/XenManager.h"
#include "audio/MIDIManager.h"
#include "audio/MIDILearn.h"
#include "audio/ProcessSuspend.h"
#include "audio/DryWetMix.h"
#include "audio/MidSide.h"
#include "audio/Oversampling.h"
#include "audio/Meter.h"

#include "audio/Manta.h"
#include "audio/SpectroBeam.h"

#include "audio/AudioUtils.h"

namespace audio
{
    using MacroProcessor = param::MacroProcessor;
    using Timer = juce::Timer;

    struct ProcessorBackEnd :
        public juce::AudioProcessor,
        public Timer
    {
        using ChannelSet = juce::AudioChannelSet;
        using AppProps = juce::ApplicationProperties;

        ProcessorBackEnd();

        const juce::String getName() const override;
        double getTailLengthSeconds() const override;
        int getNumPrograms() override;
        int getCurrentProgram() override;
        void setCurrentProgram(int) override;
        const juce::String getProgramName(int) override;
        void changeProgramName(int, const juce::String&) override;
        bool isBusesLayoutSupported(const BusesLayout&) const override;
        AppProps* getProps() noexcept;
        bool canAddBus(bool) const override;

        void savePatch();
        void loadPatch();

        bool hasEditor() const override;
        bool acceptsMidi() const override;
        bool producesMidi() const override;
        bool isMidiEffect() const override;

        juce::AudioProcessor::BusesProperties makeBusesProperties();

        AppProps props;
        ProcessSuspender sus;

        XenManager xenManager;
        State state;
        Params params;
        MacroProcessor macroProcessor;
        MIDIManager midiManager;
        DryWetMix dryWetMix;
#if PPDHasHQ
        Oversampler oversampler;
#endif
        Meters meters;
        MIDIVoices midiVoices;
        TuningEditorSynth tuningEditorSynth;

        void forcePrepareToPlay();

        void timerCallback() override;

        void processBlockBypassed(AudioBuffer&, juce::MidiBuffer&) override;

#if PPDHasStereoConfig
        bool midSideEnabled;
#endif
#if PPDHasLookahead
		bool lookaheadEnabled;
#endif

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ProcessorBackEnd)
    };

    struct Processor :
        public ProcessorBackEnd
    {
        Processor();

        void prepareToPlay(double, int) override;

        void processBlock(AudioBuffer&, juce::MidiBuffer&);
        
        /* samples, numChannels, numSamples, samplesSC, numChannelsSC */
        void processBlockDownsampled(float* const*, int numChannels, int numSamples
#if PPDHasSidechain
            , float* const*, int
#endif
        ) noexcept;

        /* samples, numChannels, numSamples, samplesSC, numChannelsSC */
        void processBlockUpsampled(float* const*, int, int
#if PPDHasSidechain
            , float* const*, int
#endif
        ) noexcept;

        void releaseResources() override;
		
        /////////////////////////////////////////////
        /////////////////////////////////////////////
        void getStateInformation(juce::MemoryBlock&) override;
		/* data, sizeInBytes */
        void setStateInformation(const void*, int) override;
		
        void savePatch();

        void loadPatch();

        juce::AudioProcessorEditor* createEditor() override;

        Manta manta;
        SpectroBeam<11> spectroBeam;
    };
}
