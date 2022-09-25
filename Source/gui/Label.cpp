#include "Label.h"

namespace gui
{
	Label::Label(Utils& u, const String& _text, Notify&& _notify) :
		Comp(u, "", std::move(_notify), CursorType::Default),
		group(),
		textCID(ColourID::Txt),
		just(Just::centred),
		font(getFontDosisExtraBold()),
		minFontHeight(12.f),
		mode(Mode::WindowToTextBounds),
		text(_text)
	{
		font.setHeight(minFontHeight);
		setInterceptsMouseClicks(false, false);
	}

	void Label::setText(const String& txt)
	{
		if (txt == text)
			return;

		text = txt;

		if (empty() || getWidth() == 0 || getHeight() == 0)
			return;

		updateTextBounds();
	}

	const String& Label::getText() const
	{
		return text;
	}

	void Label::setMinFontHeight(float h)
	{
		minFontHeight = h;
		updateTextBounds();
	}

	bool Label::empty() const noexcept
	{
		return text.isEmpty();
	}

	void Label::paint(Graphics& g)
	{
		const auto bounds = getLocalBounds().toFloat();
		g.setColour(Colours::c(textCID));
		g.setFont(font);
		g.drawFittedText(text, bounds.toNearestInt(), just, 1);
	}

	void Label::resized()
	{
		updateTextBounds();
	}

	void Label::updateTextBounds()
	{
		float nHeight = minFontHeight;

		if (mode == Mode::WindowToTextBounds)
		{
			const auto val = utils.fontHeight();
			nHeight = std::max(nHeight, val);
		}

		else if (mode == Mode::TextToLabelBounds)
		{
			const auto thicc = utils.thicc;
			const auto width = static_cast<float>(getWidth());
			const auto height = static_cast<float>(getHeight());
			
			const auto fontBounds = boundsOf(font, text);
			
			if (fontBounds.getWidth() != 0.f)
			{
				const PointF dif
				(
					fontBounds.getWidth() - width,
					fontBounds.getHeight() - height
				);

				float ratio;
				if (dif.x > dif.y)
					ratio = width / fontBounds.getWidth();
				else
					ratio = height / fontBounds.getHeight();

				nHeight = std::max(minFontHeight, font.getHeight() * ratio - thicc);
			}
		}

		else if (mode == Mode::None)
		{
			font.setHeight(nHeight);
		}

		font.setHeight(nHeight);
	}

}

