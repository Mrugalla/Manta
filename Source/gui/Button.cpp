#include "Button.h"

namespace gui
{
	BlinkyBoy::BlinkyBoy() :
		comp(nullptr),
		env(0.f),
		inv(0.f)
	{}

	void BlinkyBoy::init(Comp* _comp, float timeInSecs) noexcept
	{
		const auto fps = 30.f;
		comp = _comp;
		env = 1.f;
		inv = 1.f / (timeInSecs * fps);
		startTimerHz(static_cast<int>(fps));
	}

	Colour BlinkyBoy::getInterpolated(Colour c0, Colour c1) const noexcept
	{
		const auto e = env * env;
		return c0.interpolatedWith(c1, e < 0.f ? 0.f : e);
	}

	void BlinkyBoy::timerCallback()
	{
		env -= inv;
		if (env < 0.f)
			stopTimer();
		comp->repaint();
	}

	void Button::enableLabel(const String& txt)
	{
		label.setText(txt);
		label.textCID = ColourID::Interact;
		addAndMakeVisible(label);
	}

	void Button::enableLabel(std::vector<String>&& txt)
	{
		toggleTexts = txt;
		label.setText("");
		label.textCID = ColourID::Interact;
		addAndMakeVisible(label);
	}

	void Button::enableParameterSwitch(PID _pID)
	{
		pID = _pID;

		stopTimer();

		onClick.push_back([param = utils.getParam(pID)](Button&)
		{
			const auto ts = param->getValue() > .5f ? 0.f : 1.f;
			param->setValueWithGesture(ts);
		});

		onTimer.push_back([this](Button&)
			{
				bool shallRepaint = false;

				const auto param = utils.getParam(pID);

				const auto lckd = param->isLocked();
				if (locked != lckd)
				{
					locked = lckd;
					label.textCID = locked ? ColourID::Inactive : ColourID::Interact;
					shallRepaint = true;
				}

				const auto nTs = param->getValue() > .5f ? 1 : 0;
				if (toggleState != nTs)
				{
					toggleState = nTs;

					if (toggleTexts.size() > toggleState)
						label.setText(toggleTexts[toggleState]);

					shallRepaint = true;
				}

				if (shallRepaint)
					repaintWithChildren(this);
			});

		startTimerHz(24);
	}

	void Button::enableParameter(PID _pID, int val)
	{
		pID = _pID;

		stopTimer();

		onClick.push_back([param = utils.getParam(pID), v = static_cast<float>(val)](Button&)
		{
			const auto pVal = std::rint(param->getValueDenorm());
			const auto ts = pVal == v ? 0.f : v;
			param->setValueWithGesture(param->range.convertTo0to1(ts));
		});

		onTimer.push_back([this, val](Button&)
			{
				bool shallRepaint = false;

				const auto param = utils.getParam(pID);

				const auto lckd = param->isLocked();
				if (locked != lckd)
				{
					locked = lckd;
					label.textCID = locked ? ColourID::Inactive : ColourID::Interact;
					shallRepaint = true;
				}

				const auto pVal = std::rint(param->getValueDenorm());
				const auto nTs = static_cast<int>(pVal);
				if (toggleState != nTs)
				{
					toggleState = nTs;

					if (toggleTexts.size() > toggleState)
						label.setText(toggleTexts[toggleState]);

					shallRepaint = true;
				}

				if (shallRepaint)
					repaintWithChildren(this);
			});

		startTimerHz(24);
	}

	Button::Button(Utils& _utils, String&& _tooltip) :
		Comp(_utils, _tooltip),
		onClick(),
		onRightClick(),
		onTimer(),
		onPaint(),
		onMouseWheel(),
		blinkyBoy(),
		toggleState(-1),
		pID(PID::NumParams),
		locked(false),
		label(utils, ""),
		toggleTexts()
	{
	}

	Label& Button::getLabel() noexcept { return label; }

	const String& Button::getText() const noexcept
	{
		return label.getText();
	}

	void Button::resized()
	{
		if (label.isVisible())
		{
			const auto thicc = utils.thicc;
			const auto thicc4 = thicc * 2.f;
			const auto bounds = getLocalBounds().toFloat().reduced(thicc4);

			label.setBounds(bounds.toNearestInt());
		}
	}

	void Button::paint(Graphics& g)
	{
		for (auto& op : onPaint)
			op(g, *this);
	}

	void Button::mouseEnter(const Mouse& mouse)
	{
		Comp::mouseEnter(mouse);
		repaint();
	}

	void Button::mouseExit(const Mouse&)
	{
		repaint();
	}

	void Button::mouseUp(const Mouse& mouse)
	{
		if (mouse.mouseWasDraggedSinceMouseDown())
			return;

		if (mouse.mods.isLeftButtonDown())
		{
			if (locked)
				return;

			blinkyBoy.init(this, .25f);

			for (auto& oc : onClick)
				oc(*this);
			notify(EvtType::ButtonClicked, this);
		}
		else
		{
			for (auto& oc : onRightClick)
				oc(*this);
			notify(EvtType::ButtonRightClicked, this);
		}
	}

	void Button::mouseWheelMove(const Mouse& mouse, const MouseWheel& mouseWheel)
	{
		for (auto& mw : onMouseWheel)
			mw(mouse, mouseWheel);
	}

	void Button::timerCallback()
	{
		for (auto& ot : onTimer)
			ot(*this);
	}

	Button::OnPaint buttonOnPaintDefault()
	{
		return [](Graphics& g, Button& button)
		{
			auto col = button.blinkyBoy.getInterpolated(juce::Colours::darkgrey, juce::Colours::white);
			g.fillAll(col);
		};
	}

	void makeTextButton(Button& b, const String& txt, bool withToggle, int targetToggleState)
	{
		b.enableLabel(txt);

		b.onPaint.push_back([withToggle, targetToggleState](Graphics& g, Button& button)
			{
				const auto& utils = button.getUtils();
				const auto& blinkyBoy = button.blinkyBoy;

				auto thicc = utils.thicc;
				const auto thiccHalf = thicc * .5f;
				const bool isOver = button.isMouseOver();
				const bool isDown = button.isMouseButtonDown();
				thicc *= (isOver ? 1.1f : 1.f);

				const auto area = button.getLocalBounds().toFloat().reduced(thiccHalf);

				const auto col = blinkyBoy.getInterpolated(Colours::c(ColourID::Bg), juce::Colours::white);

				g.setColour(col);
				g.fillRoundedRectangle(area, thicc);

				g.setColour(Colours::c(ColourID::Hover));
				if (withToggle && button.toggleState == targetToggleState)
					g.fillRoundedRectangle(area, thicc);

				if (button.isMouseOver())
				{
					g.fillRoundedRectangle(area, thicc);
					if (isDown)
						g.fillRoundedRectangle(area, thicc);
				}
			});
	}
	
	void paintAbort(Graphics& g, BoundsF bounds)
	{
		g.setFont(getFontNEL());
		g.drawFittedText("X", bounds.toNearestInt(), Just::centred, 1, 0.f);
	}

	void makeSymbolButton(Button& b, ButtonSymbol symbol, int targetToggleState)
	{
		bool withToggle = true;
		if (symbol == ButtonSymbol::PatchMode)
			withToggle = false;
		else if (symbol == ButtonSymbol::StereoConfig)
		{
			withToggle = false;
			b.enableLabel({ "L/R", "M/S" });
		}

		b.onPaint.push_back([symbol, withToggle, targetToggleState](Graphics& g, Button& button)
			{
				const auto& utils = button.getUtils();
				const auto& blinkyBoy = button.blinkyBoy;

				auto thicc = utils.thicc;
				const bool isOver = button.isMouseOver();
				const bool isDown = button.isMouseButtonDown();
				thicc *= (isOver ? 1.1f : 1.f);

				auto bounds = button.getLocalBounds().toFloat().reduced(thicc);
				Colour col;
				if (symbol == ButtonSymbol::Empty)
					col = blinkyBoy.getInterpolated(Colours::c(ColourID::Transp), juce::Colours::white);
				else
					col = blinkyBoy.getInterpolated(Colours::c(ColourID::Bg), juce::Colours::white);

				g.setColour(col);
				g.fillRoundedRectangle(bounds, thicc);

				g.setColour(Colours::c(ColourID::Hover));
				if (withToggle && button.toggleState == targetToggleState)
					g.fillRoundedRectangle(bounds, thicc);

				if (button.isMouseOver())
				{
					g.fillRoundedRectangle(bounds, thicc);
					if (isDown)
						g.fillRoundedRectangle(bounds, thicc);
				}

				bool abortable = symbol == ButtonSymbol::Settings || symbol == ButtonSymbol::TuningFork;
				if (abortable && button.toggleState == 1 || symbol == ButtonSymbol::Abort)
					col = Colours::c(ColourID::Abort);
				else
					col = Colours::c(button.locked ? ColourID::Inactive : ColourID::Interact);
				g.setColour(col);

				if (symbol == ButtonSymbol::Polarity)
				{
					const auto thicc3 = thicc * 3.f;

					bounds = maxQuadIn(bounds).reduced(thicc3);
					g.drawEllipse(bounds, thicc);

					const LineF line(bounds.getBottomLeft(), bounds.getTopRight());
					g.drawLine(line, thicc);
				}
				else if (symbol == ButtonSymbol::UnityGain)
				{
					const auto thicc3 = thicc * 3.f;

					bounds = bounds.reduced(thicc3);

					const auto x0 = bounds.getX();
					const auto y0 = bounds.getY();

					const auto w = bounds.getWidth() * .666f;
					const auto h = bounds.getHeight() * .666f;

					const auto x1 = x0 + w * .5f;
					const auto y1 = y0 + h * .5f;

					g.drawEllipse({ x0, y0, w, h }, thicc);
					g.drawEllipse({ x1, y1, w, h }, thicc);
				}
				else if (symbol == ButtonSymbol::Power)
				{
					const auto thicc3 = thicc * 3.f;

					bounds = bounds.reduced(thicc3);

					const auto x = bounds.getX();
					const auto y = bounds.getY();
					const auto rad = bounds.getWidth() * .5f;

					const PointF centre(
						x + rad,
						y + rad
					);

					const auto pi = 3.14159265359f;

					const auto fromRads = pi * .2f;
					const auto toRads = 2.f * pi - fromRads;

					Path path;
					path.addCentredArc(
						centre.x,
						centre.y,
						rad,
						rad,
						0.f,
						fromRads,
						toRads,
						true
					);

					g.strokePath(path, juce::PathStrokeType(thicc));

					const LineF line(centre, centre.withY(y));

					g.drawLine(line, thicc);
				}
				else if (symbol == ButtonSymbol::PatchMode)
				{
					if (button.toggleState == 0)
					{
						const auto thicc3 = thicc * 3.f;
						bounds = maxQuadIn(bounds).reduced(thicc3);

						g.drawEllipse(bounds, thicc);
						const auto rad = bounds.getWidth() * .5f;
						PointF centre(
							bounds.getX() + rad,
							bounds.getY() + rad
						);
						const auto tick = LineF::fromStartAndAngle(centre, rad, PiQuart);
						g.drawLine(tick, thicc);
					}
					else
					{
						const auto thicc3 = thicc * 2.f;
						bounds = maxQuadIn(bounds).reduced(thicc3);

						const auto x0 = bounds.getX();
						const auto y0 = bounds.getY() + bounds.getHeight() * .5f;
						const auto x1 = x0 + bounds.getWidth() * .2f;
						const auto y1 = y0;
						g.drawLine(x0, y0, x1, y1, thicc);
						const auto x2 = x0 + bounds.getWidth() * .3f;
						const auto yA = bounds.getY() + bounds.getHeight() * .2f;
						const auto yB = bounds.getBottom() - bounds.getHeight() * .2f;
						g.drawLine(x1, y1, x2, yA, thicc);
						g.drawLine(x1, y1, x2, yB, thicc);
						const auto x3 = x0 + bounds.getWidth() * .7f;
						g.drawLine(x2, yA, x3, yA, thicc);
						g.drawLine(x2, yB, x3, yB, thicc);
						const auto x4 = x0 + bounds.getWidth() * .8f;
						const auto y4 = y0;
						g.drawLine(x3, yA, x4, y4, thicc);
						g.drawLine(x3, yB, x4, y4, thicc);
						const auto x5 = bounds.getRight();
						g.drawLine(x4, y4, x5, y4, thicc);
					}
				}
				else if (symbol == ButtonSymbol::Settings)
				{
					if (button.toggleState == 1)
					{
						paintAbort(g, maxQuadIn(bounds).reduced(thicc * 3.f));
					}
					else
					{
						const auto thicc3 = thicc * 4.f;
						bounds = maxQuadIn(bounds).reduced(thicc3);

						const auto x = bounds.getX();
						const auto y = bounds.getY();
						const auto w = bounds.getWidth();
						const auto h = bounds.getHeight();
						const auto btm = y + h;
						const auto rght = x + w;

						Stroke stroke(
							thicc,
							Stroke::JointStyle::curved,
							Stroke::EndCapStyle::rounded
						);

						const auto tickWidth = .2f;
						const auto rad = w * tickWidth;
						const auto angle0 = 0.f - PiQuart;
						const auto angle1 = PiHalf + PiQuart;

						{
							const auto centreX = x;
							const auto centreY = btm;

							Path path;
							path.addCentredArc(
								centreX, centreY,
								rad, rad,
								0.f, angle0, angle1,
								true
							);

							g.strokePath(path, stroke);
						}

						{
							const auto centreX = rght;
							const auto centreY = y;
							Path path;
							path.addCentredArc(
								centreX, centreY,
								rad, rad,
								Pi, angle0, angle1,
								true
							);

							g.strokePath(path, stroke);
						}

						{
							const auto padding = rad;

							const auto x0 = x + padding;
							const auto y0 = btm - padding;
							const auto x1 = rght - padding;
							const auto y1 = y + padding;

							g.drawLine(x0, y0, x1, y1, thicc);
						}
					}
				}
				else if (symbol == ButtonSymbol::Abort)
				{
					paintAbort(g, maxQuadIn(bounds).reduced(thicc * 3.f));
				}
				else if (symbol == ButtonSymbol::Random)
				{
					const auto thicc3 = thicc * 2.f;
					bounds = maxQuadIn(bounds).reduced(thicc3);

					const auto minDimen = std::min(bounds.getWidth(), bounds.getHeight());
					const auto radius = minDimen * .5f;
					const auto pointSize = radius * .4f;
					const auto pointRadius = pointSize * .5f;
					const auto d4 = minDimen / 4.f;
					const auto x0 = d4 * 1.2f + bounds.getX();
					const auto x1 = d4 * 2.8f + bounds.getX();
					for (auto i = 1; i < 4; ++i)
					{
						const auto y = d4 * i + bounds.getY();
						g.fillEllipse(x0 - pointRadius, y - pointRadius, pointSize, pointSize);
						g.fillEllipse(x1 - pointRadius, y - pointRadius, pointSize, pointSize);
					}
				}
				else if (symbol == ButtonSymbol::SwapParamModDepth)
				{
					const auto thicc3 = thicc * 3.f;

					bounds = maxQuadIn(bounds).reduced(thicc3);

					float xVal[2] =
					{
						bounds.getRight(),
						bounds.getX()
					};

					for (auto i = 0; i < 2; ++i)
					{
						const auto iF = static_cast<float>(i);
						const auto r = (iF + 1.f) * .33333f;

						const auto y = bounds.getY() + bounds.getHeight() * r;
						const auto x0 = xVal[i];
						const auto x1 = xVal[(i + 1) % 2];

						const LineF line(x0, y, x1, y);
						g.drawLine(line, thicc);

						const auto pt = line.getEnd();

						const auto ang0 = iF * PiHalf;

						for (auto j = 0; j < 2; ++j)
						{
							const auto jF = static_cast<float>(j);
							const auto jF2 = jF * 2.f;

							const auto angle = (1.f + jF2) * PiQuart + ang0 * (-1.f + jF2);

							const auto tick = LineF::fromStartAndAngle(pt, thicc, angle)
								.withLengthenedStart(thicc * .5f)
								.withLengthenedEnd(thicc);
							g.drawLine(tick, thicc);
						}
					}
				}
				else if (symbol == ButtonSymbol::ModDepthLock)
				{
					const auto thicc3 = thicc * 3.f;

					bounds = maxQuadIn(bounds).reduced(thicc3);

					const auto arcHeight = bounds.getHeight() * .4f;
					const auto arcWidth = bounds.getWidth() * .6f;

					const BoundsF arcBounds(
						bounds.getX() + (bounds.getWidth() - arcWidth) * .5f,
						bounds.getY(),
						arcWidth,
						arcHeight
					);

					Path path;
					path.addArc(arcBounds.getX(), arcBounds.getY(),
						arcBounds.getWidth(), arcBounds.getHeight() * 2.1f, -PiHalf, PiHalf, true);

					Stroke stroke(thicc, Stroke::JointStyle::beveled, Stroke::EndCapStyle::butt);

					g.strokePath(path, stroke);

					const BoundsF bodyBounds(
						bounds.getX(),
						arcBounds.getBottom(),
						bounds.getWidth(),
						bounds.getHeight() - arcBounds.getHeight()
					);

					g.fillRoundedRectangle(bodyBounds, thicc);
				}
				else if (symbol == ButtonSymbol::Save)
				{
					const auto thicc3 = thicc * 2.f;
					bounds = maxQuadIn(bounds).reduced(thicc3);

					Path path;
					const auto x = bounds.getX();
					const auto y = bounds.getY();
					const auto width = bounds.getWidth();
					const auto height = bounds.getHeight();
					const auto btm = bounds.getBottom();
					const auto right = bounds.getRight();
					
					const auto x2 = x + width * .2f;
					const auto x8 = x + width * .8f;
					const auto y2 = y + height * .2f;
					const auto y6 = y + height * .6f;

					path.startNewSubPath(bounds.getTopLeft());
					path.lineTo(x, btm);
					path.lineTo(right, btm);
					path.lineTo(right, y2);
					path.lineTo(x8, y);
					path.closeSubPath();

					path.startNewSubPath(x2, btm);
					path.lineTo(x2, y6);
					path.lineTo(x8, y6);
					path.lineTo(x8, btm);

					Stroke stroke(thicc, Stroke::JointStyle::beveled, Stroke::EndCapStyle::butt);
					g.strokePath(path, stroke);
				}
				else if (symbol == ButtonSymbol::Load)
				{
					const auto thicc3 = thicc * 2.f;
					bounds = maxQuadIn(bounds).reduced(thicc3);

					const auto x = bounds.getX();
					const auto y = bounds.getY();
					const auto width = bounds.getWidth();
					const auto height = bounds.getHeight();
					const auto btm = bounds.getBottom();
					const auto right = bounds.getRight();

					const auto x4 = x + width * .4f;
					const auto x5 = x + width * .5f;
					const auto x6 = x + width * .6f;

					const auto y5 = x + height * .5f;
					const auto y7 = x + height * .7f;

					const auto y8 = x + height * .8f;

					Path path;

					path.startNewSubPath(x5, y);
					path.lineTo(x5, y7);
					
					path.startNewSubPath(x4, y5);
					path.lineTo(x5, y7);
					path.lineTo(x6, y5);

					path.startNewSubPath(x, y8);
					path.lineTo(x, btm);
					path.lineTo(right, btm);
					path.lineTo(right, y8);

					Stroke stroke(thicc, Stroke::JointStyle::beveled, Stroke::EndCapStyle::butt);
					g.strokePath(path, stroke);
				}
				else if (symbol == ButtonSymbol::Remove)
				{
					const auto thicc3 = thicc * 2.f;
					bounds = maxQuadIn(bounds).reduced(thicc3);

					const auto x = bounds.getX();
					const auto y = bounds.getY();
					const auto width = bounds.getWidth();
					const auto height = bounds.getHeight();
					const auto btm = bounds.getBottom();
					const auto right = bounds.getRight();

					const auto x1 = x + width * .1f;
					const auto x2 = x + width * .2f;
					const auto x8 = x + width * .8f;
					const auto x9 = x + width * .9f;

					const auto y2 = x + height * .2f;
					const auto y3 = x + height * .3f;

					Path path;

					path.startNewSubPath(x1, y2);
					path.lineTo(x9, y2);
					path.lineTo(right, y3);
					path.lineTo(x, y3);
					path.closeSubPath();

					path.startNewSubPath(x1, y3);
					path.lineTo(x2, btm);
					path.lineTo(x8, btm);
					path.lineTo(x9, y3);

					path.startNewSubPath(x2, y2);
					path.lineTo(x2, y);
					path.lineTo(x8, y);
					path.lineTo(x8, y2);

					Stroke stroke(thicc, Stroke::JointStyle::beveled, Stroke::EndCapStyle::butt);
					g.strokePath(path, stroke);
				}
				else if (symbol == ButtonSymbol::TuningFork)
				{
					const auto thicc3 = thicc * 3.f;
					bounds = maxQuadIn(bounds).reduced(thicc3);
					
					if (button.toggleState == 1)
					{
						paintAbort(g, bounds);
						return;
					}

					const auto x = bounds.getX();
					const auto y = bounds.getY();
					const auto width = bounds.getWidth();
					const auto rad = width * .5f;

					PointF centre(x + rad, y + rad);
					auto x1 = centre.x - .1f * width;
					auto x2 = centre.x + .1f * width;
					auto y1 = y + width * .6f;
					auto y2 = y + width;
					g.drawLine(x1, y, x1, y1, thicc);
					g.drawLine(x2, y, x2, y1, thicc);
					g.drawLine(x1, y1, x2, y1, thicc);
					g.drawLine(centre.x, y1, centre.x, y2, thicc);
					g.fillEllipse(x1, y2, x2 - x1, width * .1f);
				}
			});
	}

	void makeToggleButton(Button& b, const String& txt)
	{
		makeTextButton(b, txt, true);
		b.onClick.push_back([](Button& btn)
			{
				btn.toggleState = btn.toggleState == 0 ? 1 : 0;
			});
	}

	void makeParameterSwitchButton(Button& b, PID pID, String&& txt)
	{
		makeTextButton(b, std::move(txt), true);
		b.enableParameterSwitch(pID);
	}

	void makeParameterSwitchButton(Button& b, PID pID, ButtonSymbol symbol)
	{
		makeSymbolButton(b, symbol);
		b.enableParameterSwitch(pID);
	}

	template<size_t NumButtons>
	void makeParameterButtonsGroup(std::array<Button, NumButtons>& btns, PID pID, const char* txt, bool onlyText)
	{
		for (auto i = 0; i < NumButtons; ++i)
		{
			auto& btn = btns[i];

			const auto ts = i + 1;

			makeTextButton(btn, String::charToString(txt[i]), true, onlyText, ts);
			btn.enableParameter(pID, ts);
		}

	}

	void makeButtonsGroup(std::vector<std::unique_ptr<Button>>& btns, int defaultToggleStateIndex)
	{
		for (auto& btn : btns)
			btn->toggleState = 0;
		btns[defaultToggleStateIndex]->toggleState = 1;

		for (auto i = 0; i < btns.size(); ++i)
		{
			auto& button = *btns[i];

			button.onClick.push_back([&buttons = btns](Button& btn)
				{
					for (auto& b : buttons)
					{
						b->toggleState = 0;
						repaintWithChildren(b.get());
					}

					btn.toggleState = 1;
				});
		}
	}

	void makeURLButton(Button& b, String&& urlPath)
	{
		const juce::URL url(urlPath);

		b.onClick.push_back([url](Button&)
			{
				url.launchInDefaultBrowser();
			});
	}
}

/*

toggleState == 1
	has glow

*/
