#pragma once

#include <functional>

#include "juce_core/juce_core.h"
#include "juce_audio_processors/juce_audio_processors.h"

#include "../arch/State.h"
#include "../arch/Range.h"
#include "../audio/XenManager.h"

namespace param
{
	using String = juce::String;

	String toID(const String&);

	enum class PID
	{
		// high level params
		Macro,
#if PPDHasGainIn
		GainIn,
#endif
		Mix,
#if PPD_MixOrGainDry
		MuteDry,
#endif
		Gain,
#if PPDHasPolarity
		Polarity,
#endif
#if PPDHasUnityGain && PPDHasGainIn
		UnityGain,
#endif
#if PPDHasHQ
		HQ,
#endif
#if PPDHasStereoConfig
		StereoConfig,
#endif
#if PPDHasLookahead
		Lookahead,
#endif
#if PPDHasDelta
		Delta,
#endif
		
		// tuning parameters
		Xen,
		MasterTune,
		BaseNote,
		PitchbendRange,
		
		Power,

		// low level parameters
		Lane1Enabled,
		Lane1Pitch,
		Lane1Resonance,
		Lane1Slope,
		Lane1Feedback,
		Lane1DelayOct,
		Lane1DelaySemi,
		Lane1Drive,
		Lane1RMOct,
		Lane1RMSemi,
		Lane1RMDepth,
		Lane1Gain,
		
		Lane2Enabled,
		Lane2Pitch,
		Lane2Resonance,
		Lane2Slope,
		Lane2Feedback,
		Lane2DelayOct,
		Lane2DelaySemi,
		Lane2Drive,
		Lane2RMOct,
		Lane2RMSemi,
		Lane2RMDepth,
		Lane2Gain,

		Lane3Enabled,
		Lane3Pitch,
		Lane3Resonance,
		Lane3Slope,
		Lane3Feedback,
		Lane3DelayOct,
		Lane3DelaySemi,
		Lane3Drive,
		Lane3RMOct,
		Lane3RMSemi,
		Lane3RMDepth,
		Lane3Gain,

		NumParams
	};

	static constexpr int NumParams = static_cast<int>(PID::NumParams);
	static constexpr int MinLowLevelIdx = static_cast<int>(PID::Power) + 1;
	static constexpr int NumLowLevelParams = NumParams - MinLowLevelIdx;

	/* pID, offset */
	PID ll(PID, int) noexcept;

	/* pID, offset */
	PID offset(PID, int) noexcept;

	String toString(PID);

	PID toPID(const String&);

	/* pIDs, text, seperatorChr */
	void toPIDs(std::vector<PID>&, const String&, const String&);

	String toTooltip(PID);

	enum class Unit
	{
		Power,
		Solo,
		Mute,
		Percent,
		Hz,
		Beats,
		Degree,
		Octaves,
		Semi,
		Fine,
		Ms,
		Decibel,
		Ratio,
		Polarity,
		StereoConfig,
		Voices,
		Pan,
		Xen,
		Note,
		Pitch,
		Q,
		Slope,
		NumUnits
	};

	using CharPtr = juce::CharPointer_UTF8;

	String toString(Unit);

	using ValToStrFunc = std::function<String(float)>;
	using StrToValFunc = std::function<float(const String&)>;

	using Range = juce::NormalisableRange<float>;

	using ParameterBase = juce::AudioProcessorParameter;
	using State = sta::State;
	using Xen = audio::XenManager&;

	class Param :
		public ParameterBase
	{
		static constexpr float BiasEps = .000001f;
	public:
		Param(const PID, const Range&, const float/*_valDenormDefault*/,
			const ValToStrFunc&, const StrToValFunc&,
			State&, const Unit = Unit::NumUnits);

		void savePatch(juce::ApplicationProperties&) const;

		void loadPatch(juce::ApplicationProperties&);

		//called by host, normalized, thread-safe
		float getValue() const override;

		float getValueDenorm() const noexcept;

		// called by host, normalized, avoid locks, not used by editor
		void setValue(float/*normalized*/) override;
		
		// called by editor
		bool isInGesture() const noexcept;

		void setValueWithGesture(float/*norm*/);

		void beginGesture();

		void endGesture();

		float getMaxModDepth() const noexcept;

		void setMaxModDepth(float) noexcept;

		/*macro*/
		float calcValModOf(float) const noexcept;

		float getValMod() const noexcept;

		float getValModDenorm() const noexcept;

		void setModBias(float) noexcept;

		float getModBias() const noexcept;

		void setDefaultValue(float/*norm*/) noexcept;

		// called by processor to update modulation value(s)
		void modulate(float/*macro*/) noexcept;

		float getDefaultValue() const override;

		String getName(int) const override;

		// units of param (hz, % etc.)
		String getLabel() const override;

		// string of norm val
		String getText(float /*norm*/, int) const override;

		// string to norm val
		float getValueForText(const String&) const override;

		// string to denorm val
		float getValForTextDenorm(const String&) const;

		String _toString();

		int getNumSteps() const override;

		bool isLocked() const noexcept;
		void setLocked(bool) noexcept;
		void switchLock() noexcept;

		void setModDepthLocked(bool) noexcept;

		float biased(float /*start*/, float /*end*/, float /*bias [0,1]*/, float /*x*/) const noexcept;

		static String getIDString(PID);

		const PID id;
		const Range range;
	protected:
		State& state;
		float valDenormDefault;
		std::atomic<float> valNorm, maxModDepth, valMod, modBias;
		ValToStrFunc valToStr;
		StrToValFunc strToVal;
		Unit unit;

		std::atomic<bool> locked, inGesture;

		bool modDepthLocked;
	};

	struct Params
	{
		using AudioProcessor = juce::AudioProcessor;
		using Parameters = std::vector<Param*>;

		Params(AudioProcessor&, State&, const Xen&);

		void loadPatch(juce::ApplicationProperties&);

		void savePatch(juce::ApplicationProperties&) const;

		static String getIDString();

		int getParamIdx(const String& /*nameOrID*/) const;

		size_t numParams() const noexcept;

		bool isModDepthLocked() const noexcept;
		void setModDepthLocked(bool) noexcept;
		void switchModDepthLocked() noexcept;

		Param* operator[](int) noexcept;
		const Param* operator[](int) const noexcept;
		Param* operator[](PID) noexcept;
		const Param* operator[](PID) const noexcept;

		Parameters& data() noexcept;
		const Parameters& data() const noexcept;
	protected:
		Parameters params;

		State& state;
		std::atomic<float> modDepthLocked;
	};

	namespace strToVal
	{
		std::function<float(String, const float/*altVal*/)> parse();

		StrToValFunc power();
		StrToValFunc solo();
		StrToValFunc mute();
		StrToValFunc percent();
		StrToValFunc hz();
		StrToValFunc phase();
		StrToValFunc oct();
		StrToValFunc semi();
		StrToValFunc fine();
		StrToValFunc ratio();
		StrToValFunc lrms();
		StrToValFunc freeSync();
		StrToValFunc polarity();
		StrToValFunc ms();
		StrToValFunc db();
		StrToValFunc voices();
		StrToValFunc pan(const Params&);
		StrToValFunc note();
		StrToValFunc pitch(const Xen&);
		StrToValFunc q();
		StrToValFunc slope();
	}

	namespace valToStr
	{
		ValToStrFunc mute();
		ValToStrFunc solo();
		ValToStrFunc power();
		ValToStrFunc percent();
		ValToStrFunc hz();
		ValToStrFunc phase();
		ValToStrFunc phase360();
		ValToStrFunc oct();
		ValToStrFunc semi();
		ValToStrFunc fine();
		ValToStrFunc ratio();
		ValToStrFunc lrms();
		ValToStrFunc freeSync();
		ValToStrFunc polarity();
		ValToStrFunc ms();
		ValToStrFunc db();
		ValToStrFunc empty();
		ValToStrFunc voices();
		ValToStrFunc pan(const Params&);
		ValToStrFunc note();
		ValToStrFunc pitch(const Xen&);
		ValToStrFunc q();
		ValToStrFunc slope();
	}

	/* pID, state, valDenormDefault, range, Unit */
	Param* makeParam(PID, State&,
		float = 1.f, const Range& = Range(0.f, 1.f),
		Unit = Unit::Percent);

	/* pID, state, params */
	Param* makeParamPan(PID, State&, const Params&);

	/* pID, state, valDenormDefault, range, Xen */
	Param* makeParamPitch(PID, State&, float, const Range&, const Xen&);

	struct MacroProcessor
	{
		MacroProcessor(Params&);

		void operator()() noexcept;

		Params& params;
	};
	
}