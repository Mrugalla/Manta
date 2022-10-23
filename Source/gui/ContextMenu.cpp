#include "ContextMenu.h"

namespace gui
{
	Notify ContextMenu::makeNotify(ContextMenu& popUp)
	{
		return [&pop = popUp](EvtType type, const void*)
		{
			if (type == EvtType::ClickedEmpty ||
				type == EvtType::ParametrDragged ||
				type == EvtType::EnterParametrValue)
			{
				pop.setVisible(false);
			}
		};
	}

	ContextMenu::ContextMenu(Utils& u) :
		CompWidgetable(u, "", makeNotify(*this)),
		buttons(),
		labelPtr(),
		origin(0.f, 0.f),
		bounds(0.f, 0.f, 0.f, 0.f)
	{
	}

	void ContextMenu::init()
	{
		for (auto& b : buttons)
			addAndMakeVisible(*b);

		labelPtr.reserve(buttons.size());
		for (auto& b : buttons)
			labelPtr.emplace_back(&b->getLabel());
	}

	void ContextMenu::place(const Comp* comp)
	{
		auto& pluginTop = utils.pluginTop;

		const auto screenPos = (comp->getScreenPosition() + Point(comp->getWidth() / 2, comp->getHeight() / 2) - pluginTop.getScreenPosition()).toFloat();
		BoundsF dest(screenPos.x, screenPos.y, 120.f, pluginTop.getHeight() * .4f);
		if (dest.getBottom() > pluginTop.getBottom())
			dest.setY(pluginTop.getBottom() - dest.getHeight() * 1.5f);
		if (dest.getRight() > pluginTop.getWidth())
			dest.setX(pluginTop.getWidth() - dest.getWidth());

		defineBounds
		(
			BoundsF(screenPos.x, screenPos.y, 1.f, 1.f),
			dest
		);
		initWidget(.05f, false);
		setVisible(true);
	}

	void ContextMenu::paint(Graphics& g)
	{
		const auto thicc = utils.thicc;
		g.setColour(Colours::c(ColourID::Darken));
		g.fillRoundedRectangle(getLocalBounds().toFloat().reduced(thicc), thicc);
	}

	void ContextMenu::addButton(String&& _name, String&& _tooltip)
	{
		buttons.emplace_back(std::make_unique<Button>(utils, std::move(_tooltip)));
		auto& btn = *buttons.back();
		makeTextButton(btn, std::move(_name), false);
	}

	void ContextMenu::setButton(const Button::OnClick& _onClick, int idx)
	{
		auto& btn = *buttons[idx];
		btn.onClick.clear();
		btn.onClick.push_back(_onClick);
	}

	void ContextMenu::resized()
	{
		for (auto l : labelPtr)
			l->mode = Label::Mode::TextToLabelBounds;

		distributeVertically(*this, buttons);

		auto minHeight = labelPtr.front()->font.getHeight();
		for (auto i = 1; i < labelPtr.size(); ++i)
		{
			const auto& l = *labelPtr[i];
			const auto nHeight = l.font.getHeight();
			if (minHeight < nHeight)
				minHeight = nHeight;
		}

		for (auto l : labelPtr)
		{
			l->mode = Label::Mode::None;
			l->setMinFontHeight(minHeight);
		}
	}



	Notify ContextMenuButtons::makeNotify2(ContextMenuButtons& popUp)
	{
		return [&pop = popUp](EvtType type, const void* stuff)
		{
			if (type == EvtType::ButtonRightClicked)
			{
				const auto& button = *static_cast<const Button*>(stuff);
				const auto& pIDs = button.pID;
				bool isParameterButton = !pIDs.empty();
				if (!isParameterButton)
					return;

				auto& utils = pop.getUtils();

				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					juce::Random rand;
					for (auto pID : pIDs)
					{
						auto param = u.getParam(pID);
						param->setValueWithGesture(rand.nextFloat());
					}
				}, 0);
				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					for (auto pID : pIDs)
					{
						auto param = u.getParam(pID);
						const auto val = param->getDefaultValue();
						param->setValueWithGesture(val);
					}
				}, 1);
				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					for (auto pID : pIDs)
					{
						auto param = u.getParam(pID);
						param->setDefaultValue(param->getValue());
					}
				}, 2);
				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					for (auto pID : pIDs)
					{
						auto param = u.getParam(pID);
						param->switchLock();
					}
				}, 3);
				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					for (auto pID : pIDs)
					{
						u.assignMIDILearn(pID);
					}
				}, 4);
				pop.setButton([&u = utils, pIDs](Button&, const Mouse&)
				{
					for (auto pID : pIDs)
					{
						u.removeMIDILearn(pID);
					}
				}, 5);
				pop.setButton([&u = utils, &btn = button](Button&, const Mouse&)
				{
					//u.getEventSystem().notify(EvtType::EnterParametrValue, &btn);
					// idk yet if this works
				}, 6);

				pop.place(&button);
			}
		};
	}

	ContextMenuButtons::ContextMenuButtons(Utils& u) :
		ContextMenu(u)
	{
		evts.push_back({ utils.getEventSystem(), makeNotify2(*this) });

		buttons.reserve(7);
		addButton("Randomize", "Randomize this parameter value.");
		addButton("Load Default", "Resets this parameter value to its default value.");
		addButton("Save Default", "Saves this parameter value as its default one.");
		addButton("Lock / Unlock", "Parameter values are locked into place, even when changing presets.");
		addButton("MIDI Learn", "Click here to assign this parameter to a hardware control.");
		addButton("MIDI Unlearn", "Click here to remove this parameter from its hardware control(s).");
		addButton("Enter Value", "Click here to enter a parameter value with your keyboard");

		init();
	}



	Notify ContextMenuMacro::makeNotify2(ContextMenuMacro& popUp)
	{
		return [&pop = popUp](EvtType type, const void* stuff)
		{
			if (type == EvtType::ButtonRightClicked)
			{
				const auto& button = *static_cast<const Button*>(stuff);

				pop.setButton([](Button&, const Mouse&)
				{

				}, 0);
				pop.setButton([](Button&, const Mouse&)
				{
					
				}, 1);
				pop.setButton([](Button&, const Mouse&)
				{

				}, 2);
				pop.setButton([](Button&, const Mouse&)
				{

				}, 3);
				pop.setButton([](Button&, const Mouse&)
				{

				}, 4);
				pop.setButton([](Button&, const Mouse&)
				{

				}, 5);
				pop.setButton([](Button&, const Mouse&)
				{

				}, 6);

				pop.place(&button);
			}
		};
	}

	ContextMenuMacro::ContextMenuMacro(Utils& u) :
		ContextMenu(u)
	{
		evts.push_back({ utils.getEventSystem(), makeNotify2(*this) });

		buttons.reserve(7);
		
		addButton("Load Mods", "Load a patch as modulation destinations.");
		addButton("Save Mods", "Saves the current modulation destinations as a patch.");
		addButton("Mods / Bias", "Switch between controlling modulation depth and bias.");
		addButton("Rel/Abs Mods", "Switch between relative or absolute modulation depth.");
		addButton("Flip Mods/Value", "Flips the current parameter values with their modulation destinations.");
		addButton("MIDI Unlearn", "Click here to remove this parameter from its hardware control(s).");
		addButton("Randomize All Mods", "Randomizes all parameters modulation depths and bias values");

		init();
	}
}