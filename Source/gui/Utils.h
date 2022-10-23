#pragma once
#include "Using.h"
#include "Shared.h"
#include "Events.h"
#include "../audio/MIDILearn.h"

namespace gui
{
	using Notify = evt::Notify;
	using Evt = evt::System::Evt;
	using EvtType = evt::Type;
	using EventSystem = evt::System;

	enum class CursorType
	{
		Default,
		Interact,
		Inactive,
		Mod,
		Bias,
		NumTypes
	};

	juce::MouseCursor makeCursor(CursorType);

	void hideCursor();
	void showCursor(const Component&);
	void centreCursor(const Component&, juce::MouseInputSource&);

	class Utils
	{
		static constexpr float DragSpeed = .5f;
	public:
		Utils(Component& /*pluginTop*/, Processor&);

		Param* getParam(PID pID) noexcept;
		const Param* getParam(PID pID) const noexcept;

		/* pID, offset */
		Param* getParam(PID pID, int) noexcept;
		/* pID, offset */
		const Param* getParam(PID pID, int) const noexcept;
		
		std::vector<Param*>& getAllParams() noexcept;
		const std::vector<Param*>& getAllParams() const noexcept;

		Params& getParams() noexcept;
		const Params& getParams() const noexcept;

		juce::ValueTree getState() const noexcept;

		void assignMIDILearn(PID pID) noexcept;
		void removeMIDILearn(PID pID) noexcept;
		const audio::MIDILearn& getMIDILearn() const noexcept;

		float getDragSpeed() const noexcept;

		float fontHeight() const noexcept;

		EventSystem& getEventSystem();
	
		const std::atomic<float>& getMeter(int i) const noexcept;

		Point getScreenPosition() const noexcept;

		void resized();

		ValueTree savePatch();

		void loadPatch(const ValueTree&);

		AppProps& getProps() noexcept;

		const MIDIVoicesArray& getMIDIVoicesArray() const noexcept
		{
			return audioProcessor.midiVoices.voices;
		}

		void giveDAWKeyboardFocus();

		Component& pluginTop;
		float thicc;
		Processor& audioProcessor;
	protected:
		Params& params;
		EventSystem eventSystem;
		Evt evt;
	};

	void appendRandomString(String&, Random&, int/*length*/,
		const String& /*legalChars*/ = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");

	/*number to snap, max order of sequence 1 << x*/
	int snapToJordanPolyaSequence(int, int) noexcept;
}