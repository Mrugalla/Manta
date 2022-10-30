#pragma once
#include "Button.h"
#include "GUIParams.h"

namespace gui
{
	struct TextEditor :
		public Comp,
		public Timer
	{
		/* tooltip, empty string */
		TextEditor(Utils&, const String&, Notify&&, const String& = "enter value..");
		
		/* tooltip, empty string */
		TextEditor(Utils&, const String&, const String&);

		void setVisible(bool) override;

		void addText(const String&);

		void enable();

		bool isEnabled() const noexcept;

		void disable();

		const String& getText() const noexcept;

		void setText(const String&);

		bool isEmpty() const noexcept;

		bool isNotEmpty() const noexcept;

		void clear();

		std::function<bool()> onEscape, onReturn, onType, onRemove, onClick;
	protected:
		Label label;
		String emptyString, txt;
		BlinkyBoy blinkyBoy;
		int tickIdx;
		bool drawTick;
	public:
		bool multiLine;

		void mouseUp(const Mouse&) override;

		void paint(Graphics&) override;

		void resized() override;

		void updateLabel();

		void timerCallback() override;

		bool keyPressed(const KeyPress&) override;

		void removeMultiLine(String& text);
	};
}

