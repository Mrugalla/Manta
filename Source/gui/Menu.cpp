#include "Menu.h"

namespace gui
{
	Just getJust(const String& t)
	{
		if (t == "left") return Just::left;
		if (t == "topLeft") return Just::topLeft;
		if (t == "topRight") return Just::topRight;
		if (t == "top") return Just::top;
		if (t == "bottom") return Just::bottom;
		if (t == "right") return Just::right;

		return Just::centred;
	}

	// COLOURSELECTOR

	ColourSelector::ColourSelector(Utils& u) :
		Comp(u, "", CursorType::Default),
		selector(!CS::showAlphaChannel | CS::showColourAtTop | !CS::showSliders | CS::showColourspace, 4, 7),
		revert(u, "Click here to revert to the last state of your coloursheme."),
		deflt(u, "Click here to set the coloursheme back to its default state."),
		curSheme(Colours::c(ColourID::Interact))
	{
		layout.init(
			{ 1, 1 },
			{ 8, 2 }
		);

		selector.setCurrentColour(curSheme, juce::NotificationType::dontSendNotification);

		makeTextButton(revert, "Revert", false, true);
		makeTextButton(deflt, "Default", false, true);

		revert.onClick.push_back([this](Button&)
			{
				Colours::c.set(curSheme);

				selector.setCurrentColour(
					curSheme,
					juce::NotificationType::dontSendNotification
				);

				notify(EvtType::ColourSchemeChanged);
			});
		deflt.onClick.push_back([this](Button&)
			{
				Colours::c.set(Colours::c.defaultColour());

				curSheme = Colours::c(ColourID::Interact);
				selector.setCurrentColour(
					curSheme,
					juce::NotificationType::dontSendNotification
				);

				notify(EvtType::ColourSchemeChanged);
			});

		addAndMakeVisible(selector);
		addAndMakeVisible(revert);
		addAndMakeVisible(deflt);

		startTimerHz(12);
	}

	void ColourSelector::paint(Graphics&){}

	void ColourSelector::resized()
	{
		layout.resized();

		layout.place(selector, 0, 0, 2, 1, false);
		layout.place(revert, 0, 1, 1, 1, false);
		layout.place(deflt, 1, 1, 1, 1, false);
	}

	void ColourSelector::timerCallback()
	{
		const auto curCol = selector.getCurrentColour();
		const auto lastCol = Colours::c(ColourID::Interact);

		if (curCol == lastCol)
			return;

		Colours::c.set(curCol);
		notify(EvtType::ColourSchemeChanged);
	}

	// ErkenntnisseComp

	ErkenntnisseComp::ErkenntnisseComp(Utils& u) :
		Comp(u, "", CursorType::Default),
		Timer(),
		editor(u, "Enter or edit wisdom.", "Enter wisdom..."),
		date(u, ""),
		manifest(u, "Click here to manifest wisdom to the manifest of wisdom!"),
		inspire(u, "Click here to get inspired by past wisdom of the manifest of wisdom!"),
		reveal(u, "Click here to reveal wisdom from the manifest of wisdom!"),
		clear(u, "Click here to clear the wisdom editor to write more wisdom!")
	{
		const File folder(getFolder());
		if (!folder.exists())
			folder.createDirectory();

		layout.init(
			{ 1, 1, 1, 1 },
			{ 8, 1, 1 }
		);

		addAndMakeVisible(editor);
		addAndMakeVisible(date);

		date.mode = Label::Mode::TextToLabelBounds;
		manifest.getLabel().mode = date.mode;
		inspire.getLabel().mode = date.mode;
		reveal.getLabel().mode = date.mode;
		clear.getLabel().mode = date.mode;

		addAndMakeVisible(manifest);
		addAndMakeVisible(inspire);
		addAndMakeVisible(reveal);
		addAndMakeVisible(clear);

		makeTextButton(manifest, "Manifest");
		makeTextButton(inspire, "Inspire");
		makeTextButton(reveal, "Reveal");
		makeTextButton(clear, "Clear");

		editor.onReturn = [&]()
		{
			saveToDisk();
		};

		editor.onClick = [&]()
		{
			editor.enable();
		};

		manifest.onClick.push_back([&](Button&)
			{
				saveToDisk();
			});

		inspire.onClick.push_back([&](Button&)
			{
				const File folder(getFolder());

				const auto fileTypes = File::TypesOfFileToFind::findFiles;
				const String extension(".txt");
				const auto wildCard = "*" + extension;
				const auto numFiles = folder.getNumberOfChildFiles(fileTypes, wildCard);
				if (numFiles == 0)
					return parse("I am deeply sorry. There is no wisdom in the manifest of wisdom yet.");

				Random rand;
				auto idx = rand.nextInt(numFiles);

				const RangedDirectoryIterator files(
					folder,
					false,
					wildCard,
					fileTypes
				);

				for (const auto& it : files)
				{
					if (idx == 0)
					{
						const File file(it.getFile());
						parse(file.getFileName());
						editor.setText(file.loadFileAsString());
						editor.disable();
						return;
					}
					else
						--idx;
				}
			});

		reveal.onClick.push_back([&](Button&)
			{
				const File file(getFolder() + date.getText());
				if (file.exists())
					file.revealToUser();

				const File folder(getFolder());
				folder.revealToUser();
			});

		clear.onClick.push_back([&](Button&)
			{
				editor.clear();
				editor.enable();
				parse("");
			});

		startTimerHz(4);
	}

	void ErkenntnisseComp::timerCallback()
	{
		if (editor.isShowing())
		{
			editor.enable();
			stopTimer();
		}
	}

	void ErkenntnisseComp::resized()
	{
		layout.resized();

		layout.place(editor, 0, 0, 4, 1, false);
		layout.place(date, 0, 1, 4, 1, false);

		layout.place(manifest, 0, 2, 1, 1, false);
		layout.place(inspire, 1, 2, 1, 1, false);
		layout.place(reveal, 2, 2, 1, 1, false);
		layout.place(clear, 3, 2, 1, 1, false);
	}

	void ErkenntnisseComp::paint(Graphics&)
	{}

	String ErkenntnisseComp::getFolder()
	{
		auto specialLoc = File::getSpecialLocation(File::SpecialLocationType::userApplicationDataDirectory);

		return specialLoc.getFullPathName() + "\\Mrugalla\\sharedState\\TheManifestOfWisdom\\";
	}

	void ErkenntnisseComp::saveToDisk()
	{
		if (editor.isEmpty())
			return parse("You have to enter some wisdom in order to manifest it.");

		const auto now = Time::getCurrentTime();
		const auto nowStr = now.toString(true, true, false, true).replaceCharacters(" ", "_").replaceCharacters(":", "_");

		File file(getFolder() + nowStr + ".txt");

		if (!file.existsAsFile())
			file.create();
		else
			return parse("Relax! You can only manifest 1 wisdom per minute.");

		file.appendText(editor.getText());
		editor.disable();

		parse("Manifested: " + nowStr);
	}

	void ErkenntnisseComp::parse(String&& msg)
	{
		date.setText(msg);
		date.repaint();
	}

	// ComponentWithBounds

	template<typename CompType>
	ComponentWithBounds::ComponentWithBounds(CompType* _c, BoundsF&& _b, bool _isQuad) :
		c(_c == nullptr ? nullptr : _c),
		b(_b),
		isQuad(_isQuad)
	{}

	// CompModular

	CompModular::CompModular(Utils& u, String&& _tooltip, CursorType ct) :
		Comp(u, std::move(_tooltip), ct)
	{}

	void CompModular::init()
	{
		for (auto& cmp : comps)
			addAndMakeVisible(*cmp.c);
	}

	void CompModular::paint(Graphics&){}

	void CompModular::resized()
	{
		layout.resized();

		for (auto& cmp : comps)
			if (cmp.c != nullptr)
				layout.place(
					*cmp.c,
					cmp.b.getX(),
					cmp.b.getY(),
					cmp.b.getWidth(),
					cmp.b.getHeight(),
					cmp.isQuad
				);
	}

	// NavBar::Node

	NavBar::Node::Node(const ValueTree& _vt, int _x, int _y) :
		vt(_vt),
		x(_x),
		y(_y)
	{}

	// NavBar

	NavBar::Nodes NavBar::makeNodes(const ValueTree& xml, int x, int y)
	{
		Nodes ndes;

		for (auto i = 0; i < xml.getNumChildren(); ++i)
		{
			const auto child = xml.getChild(i);
			if (child.hasType("menu"))
			{
				ndes.push_back({ child, x, y });

				const auto moarNodes = makeNodes(child, x + 1, y + 1);
				for (const auto& n : moarNodes)
					ndes.push_back(n);
				y = ndes.back().y + 1;
			}
		}

		return ndes;
	}

	int NavBar::getDeepestNode() const noexcept
	{
		int d = 0;
		for (const auto& n : nodes)
			if (n.x > d)
				d = n.x;
		return d;
	}

	NavBar::NavBar(Utils& u, const ValueTree& xml) :
		Comp(u, "", CursorType::Default),
		label(u, "Nav:"),
		nodes(makeNodes(xml)),
		buttons(),
		numMenus(static_cast<int>(nodes.size())),
		deepestNode(getDeepestNode())
	{
		label.setTooltip("Click on a node in order to navigate to its sub menu.");

		std::vector<int> a, b;
		a.resize(numMenus + 1, 1);
		b.resize(deepestNode + 1, 1);

		layout.init(b, a);

		label.textCID = ColourID::Hover;
		addAndMakeVisible(label);

		buttons.reserve(numMenus);
		for (auto i = 0; i < numMenus; ++i)
		{
			const auto& node = nodes[i].vt;

			buttons.emplace_back(std::make_unique<Button>(
				utils, node.getProperty("tooltip").toString()
				));

			auto& btn = *buttons[i];

			makeTextButton(btn, "- " + node.getProperty("id").toString(), true, 1);
			btn.getLabel().just = Just::left;
			btn.getLabel().font = Font();
		}

		makeButtonsGroup(buttons, 0);

		for (auto& btn : buttons)
			addAndMakeVisible(*btn);
	}

	void NavBar::init(std::unique_ptr<CompModular>& subMenu, Comp& parent)
	{
		for (auto i = 0; i < numMenus; ++i)
		{
			auto& btn = *buttons[i];

			// make navigation functionality

			btn.onClick.push_back([&sub = subMenu, &prnt = parent, &node = nodes[i]](Button&)
			{
				auto& utils = prnt.getUtils();

				sub.reset(new CompModular(utils, "", CursorType::Default));

				auto& comps = sub->comps;

				{
					const auto& xLayoutProp = node.vt.getProperty("x");
					const auto& yLayoutProp = node.vt.getProperty("y");
					if (xLayoutProp.isUndefined() || yLayoutProp.isUndefined())
						return;

					sub->initLayout(xLayoutProp.toString(), yLayoutProp.toString());
				}

				enum Type { kTitle, kTxt, kColourScheme, kLink, kErkenntnisse, kNumTypes };
				std::array<Identifier, kNumTypes> ids
				{
					"title",
					"txt",
					"colourscheme",
					"link",
					"erkenntnisse"
				};

				for (auto c = 0; c < node.vt.getNumChildren(); ++c)
				{
					const auto child = node.vt.getChild(c);

					const auto& xProp = child.getProperty("x", 0.f);
					const auto& yProp = child.getProperty("y", 0.f);
					const auto& wProp = child.getProperty("w", 1.f);
					const auto& hProp = child.getProperty("h", 1.f);

					Component* comp{ nullptr };

					if (child.getType() == ids[kTitle])
					{
						auto cmp = new Label(utils, node.vt.getProperty("id").toString());
						cmp->font = getFontLobster();
						cmp->mode = Label::Mode::TextToLabelBounds;

						comp = cmp;
					}
					else if (child.getType() == ids[kTxt])
					{
						auto cmp = new Label(utils, child.getProperty("text").toString());
						cmp->just = getJust(child.getProperty("just").toString());
						cmp->font = getFontDosisRegular();
						cmp->mode = Label::Mode::TextToLabelBounds;
						cmp->setMinFontHeight(12.f);

						comp = cmp;
					}
					else if (child.getType() == ids[kColourScheme])
					{
						auto cmp = new ColourSelector(utils);

						comp = cmp;
					}
					else if (child.getType() == ids[kLink])
					{
						auto cmp = new Button(utils, child.getProperty("tooltip", "").toString());

						makeTextButton(*cmp, child.getProperty("id").toString(), false, true);
						makeURLButton(*cmp, child.getProperty("link"));

						comp = cmp;
					}
					else if (child.getType() == ids[kErkenntnisse])
					{
						auto cmp = new ErkenntnisseComp(utils);

						comp = cmp;
					}

					if (comp != nullptr)
						comps.push_back(ComponentWithBounds({
								comp,
								{
									static_cast<float>(xProp),
									static_cast<float>(yProp),
									static_cast<float>(wProp),
									static_cast<float>(hProp)
								},
								false
							}));
				}

				sub->init();

				prnt.addAndMakeVisible(*sub);
				prnt.getLayout().place(*sub, 1, 2, 2, 1, false);
			});
		}

		for (auto& oc : buttons.front()->onClick)
			oc(*buttons.front().get());
	}

	void NavBar::paint(Graphics&) {}

	void NavBar::resized()
	{
		layout.resized();

		layout.place(label, 0, 0, deepestNode + 1, 1, false);

		if (numMenus > 0)
		{
			for (auto i = 0; i < numMenus; ++i)
			{
				auto& btn = *buttons[i];
				const auto& node = nodes[i];

				layout.place(btn,
					0.f + .5f * node.x,
					1 + node.y,
					1.f + deepestNode - .5f * node.x,
					1,
					false
				);
			}
		}
	}

	// Menu

	Menu::Menu(Utils& u, const ValueTree& xml) :
		CompWidgetable(u, "", CursorType::Default),
		label(u, xml.getProperty("id", "")),
		navBar(u, xml),
		subMenu(nullptr)
	{
		layout.init(
			{ 20, 50, 20 },
			{ 20, 50, 750, 20 }
		);

		label.textCID = ColourID::Hover;
		addAndMakeVisible(label);
		addAndMakeVisible(navBar);

		navBar.init(subMenu, *this);

		setOpaque(true);
	}

	void Menu::paint(juce::Graphics& g)
	{
		g.fillAll(Colours::c(ColourID::Bg));
	}

	void Menu::resized()
	{
		layout.resized();

		layout.place(label, 1, 1, 1, 1, false);
		layout.place(navBar, 0, 2, 1, 1, false);
		if (subMenu != nullptr)
			layout.place(*subMenu, 1, 2, 2, 1, false);

	}


}