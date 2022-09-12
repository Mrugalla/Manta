#pragma once
#include <juce_data_structures/juce_data_structures.h>
#include <juce_audio_processors/juce_audio_processors.h>

namespace sta
{
	class State
	{
		using String = juce::String;
		using ValueTree = juce::ValueTree;
		using Var = juce::var;
		using Undo = juce::UndoManager;
		using XML = std::unique_ptr<juce::XmlElement>;
		using XMLDoc = juce::XmlDocument;
		using Proc = juce::AudioProcessor;

	public:
		State();

		State(const String&);

		void savePatch(const Proc&, juce::MemoryBlock&) const;

		void savePatch(juce::File&) const;

		void loadPatch(const XML&);

		void loadPatch(const Proc&, const void* /*data*/ , int /*sizeInBytes*/);

		void loadPatch(const char* /*data*/, int /*sizeInBytes*/);

		void loadPatch(const juce::File&);

		void loadPatch(const ValueTree&);

		/*key, id, val, undoable*/
		void set(String&& /*key*/, String&& /*id*/, Var&&, bool /*undoable*/ = true);

		/*key, id, val, undoable*/
		void set(String&& /*key*/, const String& /*id*/, Var&&, bool /*undoable*/ = true);

		/*key, id, val, undoable*/
		void set(const String& /*key*/, String&& /*id*/, Var&&, bool /*undoable*/ = true);

		/*key, id, val, undoable*/
		void set(const String& /*key*/, const String& /*id*/, Var&&, bool /*undoable*/ = true);

		/*key, id*/
		const Var* get(String&& /*key*/, String&& /*id*/) const;

		/*key, id*/
		const Var* get(String&& /*key*/, const String& /*id*/) const;

		/*key, id*/
		const Var* get(const String& /*key*/, const String& /*id*/) const;

		void undo();

		void redo();

		ValueTree getState() const noexcept;

		// DEBUGGING:
		String toString() const;

		void dbg() const;

		String toString(String&& /*key*/, String&& /*id*/) const;

	protected:
		ValueTree state;
		Undo undoer;

	private:
		String toID(const String& /*txt*/) const;

		void setProperty(const String& /*key*/, const String& /*id*/, Var&&, ValueTree /*knot*/, Undo*);

		const Var* getProperty(const String& /*key*/, const String& /*id*/, ValueTree /*knot*/) const;
	};
}