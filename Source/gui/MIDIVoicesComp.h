#pragma once
#include "Comp.h"

namespace gui
{
	struct MIDIVoicesComp :
		public Comp,
		public Timer
	{
		MIDIVoicesComp(Utils&);

		void timerCallback() override;

		void paint(Graphics&) override;

	protected:
		const MIDIVoicesArray& voices;
#if PPD_MIDINumVoices > 0
		std::array<bool, PPD_MIDINumVoices> voicesActive;
#endif
	};
}