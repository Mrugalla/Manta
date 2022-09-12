#include "Comp.h"

namespace gui
{
	Comp::Comp(Utils& _utils, const String& _tooltip, CursorType _cursorType) :
		utils(_utils),
		layout(*this),
		evts(),
		tooltip(_tooltip),
		cursorType(_cursorType)
	{
		evts.reserve(1);
		evts.emplace_back(utils.getEventSystem(), makeNotifyBasic(this));

		setMouseCursor(makeCursor(cursorType));
	}

	Comp::Comp(Utils& _utils, const String& _tooltip, Notify&& _notify, CursorType _cursorType) :
		utils(_utils),
		layout(*this),
		evts(),
		tooltip(_tooltip),
		cursorType(_cursorType)
	{
		evts.reserve(2);
		evts.emplace_back(utils.getEventSystem(), makeNotifyBasic(this));
		evts.emplace_back(utils.getEventSystem(), _notify);

		setMouseCursor(makeCursor(cursorType));
	}

	const Utils& Comp::getUtils() const noexcept { return utils; }
	Utils& Comp::getUtils() noexcept { return utils; }

	const String* Comp::getTooltip() const noexcept { return &tooltip; }
	String* Comp::getTooltip() noexcept { return &tooltip; }

	void Comp::setTooltip(String&& t)
	{
		tooltip = t;
		setInterceptsMouseClicks(true, true);
	}

	void Comp::setCursorType(CursorType ct)
	{
		if (cursorType != ct)
		{
			cursorType = ct;
			updateCursor();
		}
	}

	void Comp::updateCursor()
	{
		setMouseCursor(makeCursor(cursorType));
	}

	const Layout& Comp::getLayout() const noexcept { return layout; };

	void Comp::initLayout(const std::vector<int>& xL, const std::vector<int>& yL)
	{
		layout.init(xL, yL);
	}

	void Comp::initLayout(const String& xL, const String& yL)
	{
		layout.fromStrings(xL, yL);
	}

	void Comp::notify(EvtType type, const void* stuff)
	{
		evts[0](type, stuff);
	}

	void Comp::paint(Graphics& g)
	{
		g.setColour(juce::Colour(0xffff0000));
		g.drawRect(getLocalBounds().toFloat(), 1.f);
	}

	void Comp::mouseEnter(const Mouse&)
	{
		notify(EvtType::TooltipUpdated, &tooltip);
	}

	void Comp::mouseUp(const Mouse&)
	{
		notify(EvtType::ClickedEmpty, this);
	}

	Notify Comp::makeNotifyBasic(Comp* c)
	{
		return[&comp = *c](const EvtType type, const void*)
		{
			if (type == EvtType::ColourSchemeChanged)
			{
				comp.updateCursor();
				comp.repaint();
			}
			else if (type == EvtType::PatchUpdated)
			{
				comp.resized();
				comp.repaint();
			}
		};
	}

	
	
	CompWidgetable::CompWidgetable(Utils& u, String&& _tooltip, CursorType _cursorType) :
		Comp(u, std::move(_tooltip), _cursorType),
		bounds0(),
		bounds1(),
		widgetEnvelope(0.f),
		widgetInc(1.f)
	{
	}

	CompWidgetable::CompWidgetable(Utils& u, String&& _tooltip, Notify&& _notify, CursorType _cursorType) :
		Comp(u, std::move(_tooltip), std::move(_notify), _cursorType),
		bounds0(),
		bounds1(),
		widgetEnvelope(0.f),
		widgetInc(1.f)
	{
	}

	void CompWidgetable::defineBounds(const BoundsF& b0, const BoundsF& b1)
	{
		bounds0 = b0;
		bounds1 = b1;
	}

	void CompWidgetable::initWidget(float lengthInSecs, bool _widgetEnv)
	{
		widgetEnvelope = _widgetEnv ? 1.f : 0.f;
		widgetInc = 1.f / (30.f * lengthInSecs) * (_widgetEnv ? -1.f : 1.f);
		startTimerHz(30);
	}

	void CompWidgetable::updateBounds()
	{
		const auto x = static_cast<int>(bounds0.getX() + widgetEnvelope * (bounds1.getX() - bounds0.getX()));
		const auto y = static_cast<int>(bounds0.getY() + widgetEnvelope * (bounds1.getY() - bounds0.getY()));
		const auto w = static_cast<int>(bounds0.getWidth() + widgetEnvelope * (bounds1.getWidth() - bounds0.getWidth()));
		const auto h = static_cast<int>(bounds0.getHeight() + widgetEnvelope * (bounds1.getHeight() - bounds0.getHeight()));

		setBounds(x, y, w, h);
	}

	void CompWidgetable::timerCallback()
	{
		widgetEnvelope += widgetInc;
		if (widgetEnvelope < 0.f || widgetEnvelope > 1.f)
		{
			stopTimer();
			widgetEnvelope = std::rint(widgetEnvelope);
		}

		updateBounds();
	}



	CompScrollable::ScrollBar::ScrollBar(Utils& u, CompScrollable& _scrollable, bool _vertical) :
		Comp(u, "Drag / Mousewheel to scroll."),
		scrollable(_scrollable),
		dragXY(0.f),
		vertical(_vertical)
	{
		setBufferedToImage(true);
	}

	bool CompScrollable::ScrollBar::needed() const noexcept
	{
		if (vertical)
			return scrollable.actualHeight > static_cast<float>(getHeight());
		return scrollable.actualHeight > static_cast<float>(getWidth());
	}

	void CompScrollable::ScrollBar::mouseEnter(const Mouse& mouse)
	{
		Comp::mouseEnter(mouse);
		repaint();
	}

	void CompScrollable::ScrollBar::mouseDown(const Mouse& mouse)
	{
		if (!needed())
			return;

		hideCursor();

		const auto speed = 1.f / utils.getDragSpeed();

		if (vertical)
		{
			const auto h = static_cast<float>(scrollable.getHeight());
			dragXY = mouse.position.y * speed * h;
		}
		else
		{
			const auto w = static_cast<float>(scrollable.getWidth());
			dragXY = mouse.position.x * speed * w;
		}
	}

	void CompScrollable::ScrollBar::mouseDrag(const Mouse& mouse)
	{
		if (!needed())
			return;

		const auto speed = 1.f / utils.getDragSpeed();

		if (vertical)
		{
			const auto h = static_cast<float>(scrollable.getHeight());
			const auto nDragXY = mouse.position.y * speed * h;
			auto dragDif = nDragXY - dragXY;
			if (mouse.mods.isShiftDown())
				dragDif *= SensitiveDrag;
			updateHandlePosY(scrollable.yScrollOffset + dragDif);
			dragXY = nDragXY;
		}
		else
		{
			const auto w = static_cast<float>(scrollable.getWidth());
			const auto nDragXY = mouse.position.x * speed * w;
			auto dragDif = nDragXY - dragXY;
			if (mouse.mods.isShiftDown())
				dragDif *= SensitiveDrag;
			updateHandlePosX(scrollable.xScrollOffset + dragDif);
			dragXY = nDragXY;
		}
	}

	void CompScrollable::ScrollBar::mouseUp(const Mouse& mouse)
	{
		if (!needed())
			return;

		const auto w = static_cast<float>(scrollable.getWidth());
		const auto h = static_cast<float>(scrollable.getHeight());

		if (mouse.mouseWasDraggedSinceMouseDown())
		{
			const auto speed = 1.f / utils.getDragSpeed();

			if (vertical)
			{
				const auto nDragXY = mouse.position.y * speed * h;
				const auto dragDif = nDragXY - dragXY;
				updateHandlePosY(scrollable.yScrollOffset + dragDif);
			}
			else
			{
				const auto nDragXY = mouse.position.x * speed * w;
				const auto dragDif = nDragXY - dragXY;
				updateHandlePosX(scrollable.xScrollOffset + dragDif);
			}
			showCursor(*this);
		}
		else
		{
			if (vertical)
			{
				const auto relPos = mouse.y / h;
				updateHandlePosY(relPos * scrollable.actualHeight);
			}
			else
			{
				const auto relPos = mouse.x / w;
				updateHandlePosX(relPos * scrollable.actualHeight);
			}
			const auto pos = mouse.position.toInt();
			showCursor(*this, &pos);
		}
	}

	void CompScrollable::ScrollBar::mouseExit(const Mouse&)
	{
		repaint();
	}

	void CompScrollable::ScrollBar::mouseWheelMove(const Mouse& mouse, const MouseWheel& wheel)
	{
		const auto reversed = wheel.isReversed ? -1.f : 1.f;
		const bool isTrackPad = wheel.deltaY * wheel.deltaY < .0549316f;
		dragXY = 0.f;
		if (isTrackPad)
		{
			dragXY = reversed * wheel.deltaY;
		}
		else
		{
			const auto deltaYPos = wheel.deltaY > 0.f ? 1.f : -1.f;
			dragXY = reversed * deltaYPos;
		}
		if (mouse.mods.isShiftDown())
			dragXY *= SensitiveDrag;
		dragXY *= utils.thicc * WheelDefaultSpeed;

		if (vertical)
			updateHandlePosY(scrollable.yScrollOffset - dragXY);
		else
			updateHandlePosX(scrollable.xScrollOffset + dragXY);
	}

	void CompScrollable::ScrollBar::paint(Graphics& g)
	{
		if (!needed())
			return;

		const auto w = static_cast<float>(scrollable.getWidth());
		const auto h = static_cast<float>(scrollable.getHeight());

		const auto thicc = utils.thicc;

		BoundsF bounds;

		if (vertical)
		{
			auto handleHeight = h / scrollable.actualHeight * h;

			if (handleHeight < thicc)
				handleHeight = thicc;

			const auto handleY = scrollable.yScrollOffset / scrollable.actualHeight * (h - handleHeight);
			bounds = BoundsF(0.f, handleY, w, handleHeight).reduced(thicc);
		}
		else
		{
			auto handleWidth = w / scrollable.actualHeight * w;

			if (handleWidth < thicc)
				handleWidth = thicc;

			const auto handleX = scrollable.xScrollOffset / scrollable.actualHeight * (w - handleWidth);
			bounds = BoundsF(handleX, 0.f, handleWidth, h).reduced(thicc);
		}

		g.setColour(Colours::c(ColourID::Hover));
		if (isMouseOver())
			g.fillRoundedRectangle(bounds, thicc);
		if (isMouseButtonDown())
			g.fillRoundedRectangle(bounds, thicc);

		g.setColour(Colours::c(ColourID::Interact));
		g.drawRoundedRectangle(bounds, thicc, thicc);
	}

	void CompScrollable::ScrollBar::updateHandlePosY(float y)
	{
		const auto h = static_cast<float>(scrollable.getHeight());
		const auto maxHeight = std::max(h, scrollable.actualHeight - h);
		scrollable.yScrollOffset = juce::jlimit(0.f, maxHeight, y);
		getParentComponent()->resized();
		repaint();
	}

	void CompScrollable::ScrollBar::updateHandlePosX(float x)
	{
		const auto w = static_cast<float>(scrollable.getWidth());
		const auto maxWidth = std::max(w, scrollable.actualHeight - w);
		scrollable.xScrollOffset = juce::jlimit(0.f, maxWidth, x);
		getParentComponent()->resized();
		repaint();
	}


	CompScrollable::CompScrollable(Utils& u, bool vertical) :
		Comp(u, "", CursorType::Default),
		scrollBar(u, *this, vertical),
		xScrollOffset(0.f),
		yScrollOffset(0.f),
		actualHeight(1.f)
	{
		addAndMakeVisible(scrollBar);
	}

	void CompScrollable::mouseWheelMove(const Mouse& mouse, const MouseWheel& wheel)
	{
		Comp::mouseWheelMove(mouse, wheel);
		scrollBar.mouseWheelMove(mouse, wheel);
	}



	CompScreenshotable::CompScreenshotable(Utils& u) :
		Comp(u, "", CursorType::Default),
		screenshotImage(),
		onScreenshotFX()
	{
		setOpaque(true);
	}

	void CompScreenshotable::resized()
	{
		if (getWidth() + getHeight() == 0)
			return;

		if (screenshotImage.isNull())
		{
			screenshotImage = Image(Image::RGB, getWidth(), getHeight(), false);
		}
		else
		{
			screenshotImage = screenshotImage.rescaled(
				getWidth(),
				getHeight(),
				Graphics::lowResamplingQuality
			);
		}
	}

	void CompScreenshotable::paint(Graphics& g)
	{
		g.drawImageAt(screenshotImage, 0, 0, false);
	}

	void CompScreenshotable::takeScreenshot()
	{
		screenshotImage = utils.pluginTop.createComponentSnapshot(
			getBounds(),
			true
		);
		Graphics g{ screenshotImage };
		for (auto& ossfx : onScreenshotFX)
			ossfx(g, screenshotImage);
	}
	

}

