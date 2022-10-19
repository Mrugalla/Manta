#pragma once
#include "TextEditor.h"

namespace gui
{
	Just getJust(const String&);

	// not used yet:
	struct ColourShifter :
		public Comp
	{
		enum
		{
			Display,
			Hue,
			Sat,
			Bright,
			NumControls
		};

		ColourShifter(Utils& u) :
			Comp(u, "Click and/or drag on this component to select a colour."),
			bounds(),
			label
			{
				Label(u, "Display"),
				Label(u, "Hue"),
				Label(u, "Saturation"),
				Label(u, "Brightness")
			},
			currentColour(0xffff0000)
		{
			layout.init
			(
				{ 8, 1 },
				{ 1, 1, 1, 1 }
			);
		}

		void setCurrentColour(Colour c)
		{
			currentColour = c;
			repaint();
		}

		void paint(Graphics& g) override
		{
			const auto thicc = utils.thicc;
			g.fillRoundedRectangle(label[Display].getBounds().toFloat(), thicc);

			float hsb[4]
			{
				0.f,
				currentColour.getHue(),
				currentColour.getSaturation(),
				currentColour.getBrightness()
			};

			{
				const auto w = static_cast<float>(bounds[Hue].getWidth());
				const auto h = static_cast<float>(bounds[Hue].getHeight());

				auto hue = 0.f;
				const auto inc = 1.f / w;

				for (auto x = 0.f; x < w; ++x, hue += inc)
				{
					const auto col = Colour::fromHSL(hue, hsb[Sat], hsb[Bright], 1.f);
					g.setColour(col);
					g.drawRect(x, 0.f, 1.f, h);
				}

				g.setColour(Colour(0xff000000));
				const auto x = inc * hsb[Hue];
				g.drawRect(x - 1.f, 0.f, 3.f, h);
			}
			{
				const auto w = static_cast<float>(bounds[Sat].getWidth());
				const auto h = static_cast<float>(bounds[Sat].getHeight());

				auto sat = 0.f;
				const auto inc = 1.f / w;

				for (auto x = 0.f; x < w; ++x, sat += inc)
				{
					const auto col = Colour::fromHSL(hsb[Hue], sat, hsb[Bright], 1.f);
					g.setColour(col);
					g.drawRect(x, 0.f, 1.f, h);
				}

				g.setColour(Colour(0xff000000));
				const auto x = inc * hsb[Sat];
				g.drawRect(x - 1.f, 0.f, 3.f, h);
			}
			{
				const auto w = static_cast<float>(bounds[Bright].getWidth());
				const auto h = static_cast<float>(bounds[Bright].getHeight());

				auto bright = 0.f;
				const auto inc = 1.f / w;

				for (auto x = 0.f; x < w; ++x, bright += inc)
				{
					const auto col = Colour::fromHSL(hsb[Hue], hsb[Sat], bright, 1.f);
					g.setColour(col);
					g.drawRect(x, 0.f, 1.f, h);
				}

				g.setColour(Colour(0xff000000));
				const auto x = inc * hsb[Bright];
				g.drawRect(x - 1.f, 0.f, 3.f, h);
			}
		}

		void resized() override
		{
			layout.resized();

			layout.place(label[Display], 0, 0, 2, 1, false);

			for (auto i = 1; i < NumControls; ++i)
			{
				bounds[i] = layout(0, i, 1, 1, false);
				layout.place(label[i], 1, i, 1, 1, false);
			}
		}

		void mouseDown(const Mouse&) override
		{

		}
		void mouseDrag(const Mouse&) override
		{

		}
		void mouseUp(const Mouse&) override
		{

		}
	protected:
		std::array<BoundsF, NumControls> bounds;
		std::array<Label, NumControls> label;
		Colour currentColour;
	};
	//

	/// Sub Menus:

	struct ColourSelector :
		public Comp,
		public Timer
	{
		using CS = juce::ColourSelector;

		ColourSelector(Utils&);

		void paint(Graphics&) override;

		void resized() override;

		void timerCallback() override;

	protected:
		CS selector;
		Button revert, deflt;
		Colour curSheme;
	};

	struct ErkenntnisseComp :
		public Comp,
		public Timer
	{
		ErkenntnisseComp(Utils&);

		void timerCallback() override;

		void resized() override;

		void paint(Graphics&) override;

		TextEditor editor;
		Label date;
		Button manifest, inspire, reveal, clear, paste;

	private:
		String getFolder();

		void saveToDisk();

		void parse(String&&);
	};

	struct JuxtaComp :
		public Comp
	{
		struct Jux :
			public Comp
		{
			Jux(Utils& u, String&& text, String&& textA, String&& textB) :
				Comp(u),
				labelA(u, text.isEmpty() ? textA : text),
				labelB(u, text.isEmpty() ? textB : "")
			{
				layout.init
				(
					{ 13, 1, 13 },
					{ 1 }
				);

				bool isTitle = text.isNotEmpty();
				labelA.mode = Label::Mode::TextToLabelBounds;
				addAndMakeVisible(labelA);
				if (!isTitle)
				{
					labelA.just = Just::centredRight;
					labelB.just = Just::centredLeft;
					labelB.mode = Label::Mode::TextToLabelBounds;
					addAndMakeVisible(labelB);
				}
				else
					labelA.textCID = ColourID::Interact;
			}

			void paint(Graphics& g) override
			{
				const bool isTitle = labelB.getText() == "";
				const auto cID = isTitle ? ColourID::Interact : ColourID::Hover;
				g.setColour(Colours::c(cID));
				const auto thicc = static_cast<int>(utils.thicc);
				const auto h = getHeight();
				const auto w = static_cast<float>(getWidth());
				for(auto y = h - 1; y > h - thicc; --y)
					g.drawHorizontalLine(y, 0.f, w);
			}

			void resized() override
			{
				bool isTitle = labelB.getText() == "";
				
				if (isTitle)
					labelA.setBounds(getLocalBounds());
				else
				{
					layout.resized();

					layout.place(labelA, 0, 0, 1, 1);
					layout.place(labelB, 2, 0, 1, 1);
				}
			}

		protected:
			Label labelA, labelB;
		};

		JuxtaComp(Utils& u, const ValueTree& vt) :
			Comp(u),
			juxi()
		{
			const auto numJuxi = vt.getNumChildren();
			juxi.reserve(numJuxi);
			for (auto i = 0; i < numJuxi; ++i)
			{
				const auto juxChild = vt.getChild(i);
				juxi.emplace_back(std::make_unique<Jux>
				(
					u,
					juxChild.getProperty("text").toString(),
					juxChild.getProperty("textA").toString(),
					juxChild.getProperty("textB").toString()
				));
			}

			for (auto& j : juxi)
				addAndMakeVisible(*j);
		}

		void paint(Graphics&) override {}

		void resized() override
		{
			const auto w = static_cast<float>(getWidth());
			const auto h = static_cast<float>(getHeight());
			const auto x = 0.f;
			
			const auto numJuxi = juxi.size();
			const auto inc = h / numJuxi;
			
			auto y = 0.f;
			for (auto& j : juxi)
			{
				j->setBounds(BoundsF(x, y, w, inc).toNearestInt());
				y += inc;
			}
		}

		std::vector<std::unique_ptr<Jux>> juxi;
	};

	/// MENU STUFF IN GENERAL:

	struct ComponentWithBounds
	{
		/*comp,bounds,isQuad*/
		template<typename CompType>
		ComponentWithBounds(CompType* = nullptr, BoundsF&& = BoundsF(0.f, 0.f, 1.f, 1.f), bool = false);

		std::unique_ptr<Component> c;
		BoundsF b;
		bool isQuad;
	};

	struct CompModular :
		public Comp
	{
		/*utils,tooltip,cursorType*/
		CompModular(Utils&, String&&, CursorType);

		void init();

		std::vector<ComponentWithBounds> comps;
	protected:
		void paint(Graphics&) override;

		void resized() override;
	};

	class NavBar :
		public Comp
	{
		struct Node
		{
			Node(const ValueTree&, int, int);

			const ValueTree vt;
			const int x, y;
		};

		using Nodes = std::vector<Node>;

		Nodes makeNodes(const ValueTree&, int = 0, int = 0);

		int getDeepestNode() const noexcept;

	public:
		NavBar(Utils&, const ValueTree&);

		/* subMenu, parent */
		void init(std::unique_ptr<CompModular>&, Comp&);

	protected:
		Label label;
		Nodes nodes;
		std::vector<std::unique_ptr<Button>> buttons;
		const int numMenus, deepestNode;

		void paint(Graphics&) override;

		void resized() override;
	};

	struct Menu :
		public CompWidgetable
	{
		Menu(Utils&, const ValueTree&);

	protected:
		Label label;
		NavBar navBar;
		std::unique_ptr<CompModular> subMenu;

		void paint(juce::Graphics&) override;

		void resized() override;
	};
}