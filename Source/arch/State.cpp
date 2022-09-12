#include "State.h"

sta::State::State() :
	state("state"),
	undoer()
{
}

sta::State::State(const String& str) :
	state("state"),
	undoer()
{
	state = state.fromXml(str);
}

void sta::State::savePatch(const Proc& p, juce::MemoryBlock& destData) const
{
	std::unique_ptr<juce::XmlElement> xml(state.createXml());
	p.copyXmlToBinary(*xml, destData);
}

void sta::State::savePatch(juce::File& xmlFile) const
{
	if (!xmlFile.hasFileExtension(".xml")) return;
	if (xmlFile.existsAsFile())
		xmlFile.deleteFile();
	xmlFile.create();
	xmlFile.appendText(state.toXmlString());
}

void sta::State::loadPatch(const XML& xmlState)
{
	if (xmlState.get() != nullptr)
		if (xmlState->hasTagName(state.getType()))
			loadPatch(juce::ValueTree::fromXml(*xmlState));
}

void sta::State::loadPatch(const Proc& p, const void* data, int sizeInBytes)
{
	loadPatch(p.getXmlFromBinary(data, sizeInBytes));
}

void sta::State::loadPatch(const char* data, int sizeInBytes)
{
	loadPatch(juce::XmlDocument::parse(String(data, sizeInBytes)));
}

void sta::State::loadPatch(const juce::File& xmlFile)
{
	if (!xmlFile.hasFileExtension(".xml")) return;
	if (!xmlFile.existsAsFile()) return;
	const auto xml = XMLDoc::parse(xmlFile);
	if (xml == nullptr) return;
	if (!xml->hasTagName(state.getType())) return;
	loadPatch(ValueTree::fromXml(*xml));
}

void sta::State::loadPatch(const ValueTree& vt)
{
	state = vt;
}

void sta::State::set(String&& key, String&& id, Var&& val, bool undoable)
{
	if (undoable)
	{
		undoer.beginNewTransaction();
		setProperty(toID(key), toID(id), std::move(val), state, &undoer);
	}
	else
		setProperty(toID(key), toID(id), std::move(val), state, nullptr);
}

void sta::State::set(String&& key, const String& id, Var&& val, bool undoable)
{
	if (undoable)
	{
		undoer.beginNewTransaction();
		setProperty(toID(key), toID(id), std::move(val), state, &undoer);
	}
	else
		setProperty(toID(key), toID(id), std::move(val), state, nullptr);
}

void sta::State::set(const String& key, String&& id, Var&& val, bool undoable)
{
	if (undoable)
	{
		undoer.beginNewTransaction();
		setProperty(toID(key), toID(id), std::move(val), state, &undoer);
	}
	else
		setProperty(toID(key), toID(id), std::move(val), state, nullptr);
}

void sta::State::set(const String& key, const String& id, Var&& val, bool undoable)
{
	if (undoable)
	{
		undoer.beginNewTransaction();
		setProperty(toID(key), toID(id), std::move(val), state, &undoer);
	}
	else
		setProperty(toID(key), toID(id), std::move(val), state, nullptr);
}

const sta::State::Var* sta::State::get(String&& key, String&& id) const
{
	return getProperty(key, toID(id), state);
}

const sta::State::Var* sta::State::get(String&& key, const String& id) const
{
	return getProperty(key, toID(id), state);
}

const sta::State::Var* sta::State::get(const String& key, const String& id) const
{
	return getProperty(key, toID(id), state);
}

void sta::State::undo()
{
	if (undoer.canUndo())
		undoer.undo();
}

void sta::State::redo()
{
	if (undoer.canRedo())
		undoer.redo();
}

sta::State::ValueTree sta::State::getState() const noexcept
{
	return state;
}

// DEBUGGING:
sta::State::String sta::State::toString() const { return state.toXmlString(); }

void sta::State::dbg() const { DBG(toString()); }

sta::State::String sta::State::toString(String&& key, String&& id) const
{
	const auto var = get(std::move(key), std::move(id));
	if (var == nullptr)
		return "invalid request";
	else
		return var->toString();
}

sta::State::String sta::State::toID(const String& txt) const
{
	return txt.removeCharacters(" ").toLowerCase();
}

void sta::State::setProperty(const String& key, const String& id, Var&& val, ValueTree knot, Undo* mngr)
{
	if (knot.getType().toString() == key)
		knot.setProperty(id, val, mngr);
	else if (!key.contains("/"))
	{
		auto child = knot.getChildWithName(key);
		if (!child.isValid())
		{
			child = ValueTree(key);
			knot.appendChild(child, nullptr);
		}
		setProperty(key, id, std::move(val), child, mngr);
	}
	else
		for (auto i = 0; i < key.length(); ++i)
			if (key[i] == '/')
			{
				const auto childName = key.substring(0, i);
				auto child = knot.getChildWithName(childName);
				if (!child.isValid())
				{
					child = ValueTree(childName);
					knot.appendChild(child, nullptr);
				}
				return setProperty(key.substring(i + 1), id, std::move(val), child, mngr);
			}
}

const sta::State::Var* sta::State::getProperty(const String& key, const String& id, ValueTree knot) const
{
	if (key.contains("/"))
	{
		for (auto i = 0; i < key.length(); ++i)
			if (key[i] == '/')
			{
				const auto childName = key.substring(0, i);
				const auto child = knot.getChildWithName(childName);
				if (!child.isValid())
					return nullptr;
				return getProperty(key.substring(i + 1), id, child);
			}
	}
	const auto child = knot.getChildWithName(toID(key));
	if (!child.isValid())
		return nullptr;
	if (child.hasProperty(id))
		return &child.getProperty(id);
	return nullptr;
}