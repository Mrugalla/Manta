#include "MIDICCMonitor.h"

namespace gui
{
	MIDICCMonitor::MIDICCMonitor(Utils& u, const Learn& _learn) :
		Comp(u, "Monitors your input controller number for MIDI Learn.", CursorType::Default),
		learn(_learn),
		idx(learn.ccIdx.load()),
		label(u, idx < 0 ? "cc: .." : toString())
	{
		addAndMakeVisible(label);
		label.textCID = ColourID::Hover;
		label.just = Just::left;
		startTimerHz(24);
	}

	void MIDICCMonitor::paint(Graphics&)
	{
		
	}

	void MIDICCMonitor::resized()
	{
		label.setBounds(getLocalBounds());
	}

	void MIDICCMonitor::timerCallback()
	{
		const auto nIdx = learn.ccIdx.load();
		if (nIdx < 0)
			return;

		if (idx != nIdx)
		{
			idx = nIdx;

			label.setText(toString());
			label.repaint();
		}
	}

	String MIDICCMonitor::toString()
	{
		return "cc: " + String(idx);
	}
}