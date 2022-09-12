#pragma once
#include "Label.h"
#include "../audio/MIDILearn.h"

namespace gui
{
	struct MIDICCMonitor :
		public Comp,
		public Timer
	{
		using Learn = audio::MIDILearn;

		MIDICCMonitor(Utils&, const Learn&);
		
	protected:
		const Learn& learn;
		int idx;
		Label label;

		void paint(Graphics&);

		void resized() override;

		void timerCallback() override;

		String toString();
	};
}