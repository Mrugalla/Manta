#pragma once
#include "Comp.h"

namespace gui
{
	struct Label :
		public Comp
	{
		enum class Mode
		{
			None,
			WindowToTextBounds,
			TextToLabelBounds,
			NumModes
		};

		Label(Utils&, const String& /*text*/, Notify && = [](EvtType, const void*) {});

		void setText(const String&);

		const String& getText() const;

		void setMinFontHeight(float);

		bool empty() const noexcept;

		std::vector<Label*> group;
		ColourID textCID;
		Just just;
		Font font;
		float minFontHeight;
		Mode mode;
	protected:
		String text;

		void paint(Graphics&) override;

		void resized() override;

	private:
		void updateTextBounds();
	};
}