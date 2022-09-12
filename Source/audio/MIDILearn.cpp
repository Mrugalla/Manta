#include "MIDILearn.h"

namespace audio
{
	MIDILearn::CC::CC() :
		param(nullptr)
	{}

	void MIDILearn::CC::setValue(int _value)
	{
		auto p = param.load();
		if (p == nullptr)
			return;

		const auto value = static_cast<float>(_value) * ValInv;

		p->setValueWithGesture(value);
	}

	MIDILearn::MIDILearn(Params& _params, State& _state) :
		ccBuf(),
		ccIdx(-1),
		assignableParam(nullptr),
		params(_params),
		state(_state),
		c(-1)
	{
	}

	void MIDILearn::savePatch() const
	{
		for (auto i = 0; i < ccBuf.size(); ++i)
		{
			const auto& cc = ccBuf[i];
			const auto prm = cc.param.load();
			if (prm != nullptr)
				state.set(getIDString(i), "id", param::toID(param::toString(prm->id)), true);
		}
	}

	void MIDILearn::loadPatch()
	{
		for (auto i = 0; i < ccBuf.size(); ++i)
		{
			const auto var = state.get(getIDString(i), "id");
			if (var)
			{
				const auto idStr = var->toString();

				auto& cc = ccBuf[i];
				const auto pID = param::toPID(idStr);
				cc.param.store(params[pID]);
			}
		}
	}

	void MIDILearn::processBlockInit() noexcept
	{
		c = -1;
	}

	void MIDILearn::processBlockMIDICC(const MidiMessage& msg) noexcept
	{
		c = msg.getControllerNumber();
		if (c < ccBuf.size())
		{
			auto& cc = ccBuf[c];

			auto ap = assignableParam.load();
			if (ap != nullptr)
			{
				cc.param.store(ap);
				assignableParam.store(nullptr);
			}

			cc.setValue(msg.getControllerValue());
		}
	}

	void MIDILearn::processBlockEnd() noexcept
	{
		if (c != -1)
			ccIdx.store(c);
	}

	void MIDILearn::assignParam(Param* param) noexcept
	{
		assignableParam.store(param);
	}

	void MIDILearn::removeParam(Param* param) noexcept
	{
		for (auto& cc : ccBuf)
			if (param == cc.param)
				cc.param.store(nullptr);
	}

	String MIDILearn::getIDString(int idx) const
	{
		return "midilearn/cc" + String(idx);
	}
}

