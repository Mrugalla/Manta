#pragma once
#include "Comp.h"

namespace gui
{
	struct LogoComp :
		public Comp
	{
		LogoComp(Utils& u, const char* _data, const int _size) :
			Comp(u, "logo tooltip", CursorType::Default),
			drawable(Drawable::createFromImageData(_data, _size))
		{
			addAndMakeVisible(*drawable);
		}

		UniqueDrawable drawable;

		void paint(Graphics&) override
		{
		}

		void resized() override
		{
			drawable->setBounds(getLocalBounds());
		}
	};
}
