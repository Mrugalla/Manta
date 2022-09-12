#include "MIDIVoicesComp.h"

namespace gui
{
	MIDIVoicesComp::MIDIVoicesComp(Utils& u) :
		Comp(u, "MIDI Voices", CursorType::Default),
		voices(u.getMIDIVoicesArray())
#if PPD_MIDINumVoices > 0
		, voicesActive()
#endif
	{
#if PPD_MIDINumVoices > 0
		startTimerHz(30);
#endif
	}

	void MIDIVoicesComp::timerCallback()
	{
#if PPD_MIDINumVoices > 0
		bool wannaRepaint = false;

		for (auto v = 0; v < PPD_MIDINumVoices; ++v)
		{
			auto& voice = voices[v];
			auto active = voice.curNote.noteOn;
			if (voicesActive[v] != active)
			{
				voicesActive[v] = active;
				wannaRepaint = true;
			}
		}

		if (wannaRepaint)
			repaint();
#endif
	}

	void MIDIVoicesComp::paint(Graphics& g)
	{
#if PPD_MIDINumVoices > 0
		const auto width = static_cast<float>(getWidth());
		const auto height = static_cast<float>(getHeight());

		auto x = 0.f;
		const auto w = width / static_cast<float>(PPD_MIDINumVoices);
		const auto y = 0.f;
		const auto h = height;

		for (auto v = 0; v < PPD_MIDINumVoices; ++v, x += w)
		{
			g.setColour(Colours::c(ColourID::Interact));
			g.drawVerticalLine(static_cast<int>(x), y, h);

			if (voicesActive[v])
				g.fillRect(x, y, w, h);
		}
#endif
	}
}