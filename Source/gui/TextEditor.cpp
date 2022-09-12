#include "TextEditor.h"

namespace gui
{

	TextEditor::TextEditor(Utils& u, const String& _tooltip, Notify&& _notify, const String& _emptyString) :
		Comp(u, _tooltip, std::move(_notify)),
		Timer(),
		onEscape([]() {}),
		onReturn([]() {}),
		onType([]() {}),
		onRemove([]() {}),
		onClick([]() {}),

		label(u, ""),
		emptyString(_emptyString), txt(""),
		blinkyBoy(),
		tickIdx(0),
		drawTick(false)
	{
		addAndMakeVisible(label);
		label.mode = Label::Mode::TextToLabelBounds;
		setWantsKeyboardFocus(true);
	}

	TextEditor::TextEditor(Utils& u, const String& _tooltip, const String& _emptyString) :
		Comp(u, _tooltip),
		Timer(),
		onEscape([]() {}),
		onReturn([]() {}),
		onType([]() {}),
		onRemove([]() {}),
		onClick([]() {}),

		label(u, ""),
		emptyString(_emptyString), txt(""),
		blinkyBoy(),
		tickIdx(0),
		drawTick(false)
	{
		addAndMakeVisible(label);
		label.mode = Label::Mode::TextToLabelBounds;
		setWantsKeyboardFocus(true);
	}

	void TextEditor::setVisible(bool e)
	{
		if (e)
			Comp::setVisible(e);
		else
		{
			disable();
			clear();
			Comp::setVisible(e);
		}
	}

	void TextEditor::enable()
	{
		if (isEnabled())
			return;
		setVisible(true);
		tickIdx = getText().length();
		drawTick = true;
		grabKeyboardFocus();
		startTimerHz(PPDFPSTextEditor);
	}

	bool TextEditor::isEnabled() const noexcept
	{
		return isTimerRunning() && hasKeyboardFocus(false);
	}

	void TextEditor::disable()
	{
		stopTimer();
		drawTick = false;
		updateLabel();
	}

	const String& TextEditor::getText() const noexcept
	{
		return txt;
	}

	void TextEditor::setText(const String& str)
	{
		if (txt == str)
			return;
		txt = str;
		tickIdx = juce::jlimit(0, txt.length(), tickIdx);
		updateLabel();
	}

	bool TextEditor::isEmpty() const noexcept
	{
		return getText().isEmpty();
	}

	bool TextEditor::isNotEmpty() const noexcept
	{
		return getText().isNotEmpty();
	}

	void TextEditor::clear()
	{
		txt.clear();
		tickIdx = 0;
		repaintWithChildren(this);
	}

	void TextEditor::mouseUp(const Mouse& mouse)
	{
		if (txt.isNotEmpty())
		{
			if (label.mode == Label::Mode::TextToLabelBounds)
			{
				const auto x = mouse.position.x;
				const auto w = static_cast<float>(getWidth());
				const auto& font = label.font;
				const auto strWidth = font.getStringWidthFloat(txt);
				const auto xOff = (w - strWidth) * .5f;
				const auto xShifted = x - xOff;
				const auto strLen = static_cast<float>(txt.length());
				auto xRatio = xShifted / strWidth;
				auto xMapped = xRatio * strLen;
				auto xLimited = juce::jlimit(0.f, strLen, xMapped);
				tickIdx = static_cast<int>(std::rint(xLimited));
			}
			//else
			//	return; // not implemented yet cause not needed lol
		}

		enable();
		updateLabel();
		onClick();
	}

	void TextEditor::paint(Graphics& g)
	{
		const auto thicc = utils.thicc;
		auto col = blinkyBoy.getInterpolated(Colours::c(ColourID::Hover), Colours::c(ColourID::Interact));
		g.setColour(col);
		g.drawRoundedRectangle(getLocalBounds().toFloat(), thicc, thicc);
	}

	void TextEditor::resized()
	{
		label.setBounds(getLocalBounds());
	}

	void TextEditor::updateLabel()
	{
		if (txt.isEmpty())
		{
			label.setText(emptyString + (drawTick ? "|" : ""));
			label.textCID = ColourID::Hover;
		}
		else
		{
			label.textCID = ColourID::Txt;
			if (drawTick)
				label.setText(txt.substring(0, tickIdx) + "|" + txt.substring(tickIdx));
			else
				label.setText(txt);
		}
		label.repaint();
	}

	void TextEditor::timerCallback()
	{
		if (!hasKeyboardFocus(true))
			return disable();
		drawTick = !drawTick;
		updateLabel();
	}

	bool TextEditor::keyPressed(const KeyPress& key)
	{
		if (key == key.escapeKey)
		{
			onEscape();
			return true;
		}
		else if (key == key.returnKey)
		{
			onReturn();
			blinkyBoy.init(this, .25f);
			return true;
		}
		else if (key == key.leftKey)
		{
			if (tickIdx > 0)
				--tickIdx;
			drawTick = true;
			updateLabel();
			return true;
		}
		else if (key == key.rightKey)
		{
			if (tickIdx < txt.length())
				++tickIdx;
			drawTick = true;
			updateLabel();
			return true;
		}
		else if (key == key.backspaceKey)
		{
			onRemove();
			txt = txt.substring(0, tickIdx - 1) + txt.substring(tickIdx);
			if (tickIdx > 0)
				--tickIdx;
			drawTick = true;
			updateLabel();
			onType();
			return true;
		}
		else if (key == key.deleteKey)
		{
			onRemove();
			txt = txt.substring(0, tickIdx) + txt.substring(tickIdx + 1);
			drawTick = true;
			updateLabel();
			onType();
			return true;
		}
		else
		{
			const auto chr = key.getTextCharacter();
			txt = txt.substring(0, tickIdx) + chr + txt.substring(tickIdx);
			++tickIdx;
			drawTick = true;
			updateLabel();
			onType();
			return true;
		}
	}

}