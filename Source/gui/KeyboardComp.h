#pragma once
#include "Button.h"
#include "../arch/Conversion.h"

namespace gui
{
	class KeyboardComp :
		public Comp
	{
		struct Key :
			public Comp
		{
			Key(Utils& u, int _noteVal) :
				Comp(u, "", CursorType::Default),
				noteVal(_noteVal),
				bgCol(),
				label(u, noteVal % 12 == 0 ? "C" + String(noteVal / 12 - 1) : String(""))
			{
				const auto whiteKey = audio::isWhiteKey(noteVal % 12);
				bgCol = (whiteKey ? Colour(0xffffffff) : Colour(0xff000000)).withAlpha(.3f);

				setInterceptsMouseClicks(false, false);
				
				if (label.getText().isNotEmpty())
				{
					addAndMakeVisible(label);
					label.textCID = ColourID::Interact;
				}
					
			}
			
			void paint(Graphics& g) override
			{
				const auto thicc = getUtils().thicc;
				const auto bounds = getLocalBounds().toFloat().reduced(thicc);
				
				g.setColour(bgCol);
				g.fillRoundedRectangle(bounds, thicc);
			}

			void resized() override
			{
				const auto thicc = getUtils().thicc;
				const auto bounds = getLocalBounds().toFloat().reduced(thicc);
				label.setBounds(bounds.toNearestInt());
			}
			
			int noteVal;
		protected:
			Colour bgCol;
			Label label;
		};

	public:
		KeyboardComp(Utils& u, String&& _tooltip) :
			Comp(u, _tooltip, CursorType::Default),
			keys(),
			octDown(u, "Press this button to go down an octave."),
			octUp(u, "Press this button to go up an octave."),
			hoverIdx(-1),
			octIdx(3)
		{
			layout.init
			(
				{ 1, 21, 1 },
				{ 1 }
			);

			makeTextButton(octDown, "<<", false);
			makeTextButton(octUp, ">>", false);

			octDown.onClick.push_back([&](Button&)
				{
					if (octIdx > 0)
					{
						--octIdx;
						resized();
					}
				});
			octUp.onClick.push_back([&](Button&)
				{
					if (octIdx < 8)
					{
						++octIdx;
						resized();
					}
				});
			
			addAndMakeVisible(octDown);
			addAndMakeVisible(octUp);

			for (auto i = 0; i < keys.size(); ++i)
			{
				auto& key = keys[i];
				key = std::make_unique<Key>(u, i);
				addAndMakeVisible(*key);
			}
		}

		void resized() override
		{
			for (auto& key : keys)
				key->setVisible(false);

			layout.resized();
			
			layout.place(octDown, 0, 0, 1, 1, false);
			layout.place(octUp, 2, 0, 1, 1, false);

			const auto bounds = layout(1, 0, 1, 1);
			const auto numKeys = 24;
			const auto numKeysInv = 1.f / static_cast<float>(numKeys);
			const auto y = bounds.getY();
			const auto w = bounds.getWidth() * numKeysInv;
			const auto h = bounds.getHeight();
			const auto iOff = octIdx * 12;
			for (auto i = 0; i < numKeys; ++i)
			{
				const auto iF = static_cast<float>(i);
				const auto x = bounds.getX() + iF * w;

				const BoundsF keyBounds(x, y, w, h);
				auto& key = keys[i + iOff];
				key->setBounds(keyBounds.toNearestInt());
				key->setVisible(true);
			}
		}

		void paint(Graphics& g) override
		{
			if (hoverIdx != -1)
			{
				const auto thicc = getUtils().thicc;
				const auto bounds = keys[hoverIdx]->getBounds().toFloat().reduced(thicc);
				g.setColour(Colours::c(ColourID::Hover).withMultipliedAlpha(2.f));
				g.fillRoundedRectangle(bounds, thicc);
			}
		}

		void mouseMove(const Mouse& evt) override
		{
			Comp::mouseMove(evt);

			const auto nHoverIdx = getHoverIdx(evt.getPosition());
			if (hoverIdx != nHoverIdx)
			{
				hoverIdx = nHoverIdx;
				repaint();
			}
			
		}

		void mouseDrag(const Mouse& evt) override
		{
			Comp::mouseDrag(evt);

			const auto nHoverIdx = getHoverIdx(evt.getPosition());
			if (hoverIdx != nHoverIdx)
			{
				hoverIdx = nHoverIdx;
				repaint();
			}
		}

		void mouseExit(const Mouse&) override
		{
			hoverIdx = -1;
			repaint();
		}

	protected:
		std::array<std::unique_ptr<Key>, 128> keys;
		Button octDown, octUp;
		int hoverIdx, octIdx;
		
		int getHoverIdx(Point pt) noexcept
		{
			for (auto i = 0; i < keys.size(); ++i)
			{
				auto& key = keys[i];
				const auto keyX = key->getX();
				const auto keyRight = key->getRight();
				if (pt.x >= keyX && pt.x <= keyRight)
					return i;
			}
			return -1;
		}
		
	};
}