#pragma once
#include "../arch/FormulaParser2.h"
#include "TextEditor.h"

namespace gui
{
	struct FormulaParser :
		public TextEditor
	{
		using Parser = fx::Parser;

		enum PostFX
		{
			DCOffset,
			Windowing,
			Normalize,
			NumPostFX
		};

		FormulaParser(Utils& u, String&& _tooltip, std::vector<float*>& tables, int size, int overshoot = 0) :
			TextEditor(u, _tooltip, "enter some math"),
			postFX{ false, false, false },
			fx(),
			updateFormula()
		{
			onReturn = [this, tables, size, overshoot]()
			{
				if(!fx(txt))
					return false;
				
				updateFormula();
				return true;
			};

			updateFormula = [this, tables, size, overshoot]()
			{
				{
					auto x = -1.f;
					const auto inc = 2.f / static_cast<float>(size);
					for (auto i = 0; i < size; ++i, x += inc)
						tables[0][i] = fx(x);
				}

				const auto sizeInv = 1.f / static_cast<float>(size);
				const auto fullSize = size + overshoot;

				if (postFX[DCOffset])
				{
					auto sum = 0.f;
					for (auto i = 0; i < size; ++i)
						sum += tables[0][i];

					const auto gain = -sum * sizeInv;
					if (gain != 0.f)
						SIMD::add(tables[0], gain, size);
				}
				if (postFX[Windowing])
				{
					// tukey window
					const auto alpha = .2f;
					const auto alphaInv = 1.f / alpha;

					for (auto i = 0; i < size; ++i)
					{
						const auto x = static_cast<float>(i) * sizeInv;
						const auto w = x < alpha ? .5f * (1.f + std::cos(Pi * (x * alphaInv - 1.f))) : x > 1.f - alpha ? .5f * (1.f + std::cos(Pi * (x * alphaInv - 1.f / alpha + 1.f))) : 1.f;
						tables[0][i] *= w;
					}
				}
				if (postFX[Normalize])
				{
					auto max = 0.f;
					for (auto i = 0; i < size; ++i)
						max = std::max(max, std::abs(tables[0][i]));

					if (max > 0.f)
						SIMD::multiply(tables[0], 1.f / max, size);
				}
				for (auto i = 0; i < size; ++i)
					tables[0][i] = std::clamp(tables[0][i], -1.f, 1.f);

				for (auto i = 0; i < overshoot; ++i)
					tables[0][size + i] = tables[0][i];

				auto numTables = tables.size();
				for (auto i = 1; i < numTables; ++i)
					SIMD::copy(tables[i], tables[0], fullSize);
			};

			setInterceptsMouseClicks(true, true);

			multiLine = false;
		}

		/* samples */
		std::array<bool, NumPostFX> postFX;
		Parser fx;
		std::function<void()> updateFormula;
	};

	struct FormulaParser2 :
		public Comp
	{
		FormulaParser2(Utils& u, String&& _tooltip, std::vector<float*>& tables, int size, int overshoot) :
			Comp(u, "", CursorType::Default),
			parser(u, std::move(_tooltip), tables, size, overshoot),
			dc(u, "De/activate DC Offset."),
			normalize(u, "De/activate Normalize."),
			windowing(u, "De/activate Windowing."),
			random(u, "Generate a random formula."),
			create(u, "Create a wavetable.")
		{
			layout.init
			(
				{ 1, 1, 1, 1, 1 },
				{ 1, 1 }
			);

			addAndMakeVisible(parser);

			addAndMakeVisible(dc);
			addAndMakeVisible(normalize);
			addAndMakeVisible(windowing);
			addAndMakeVisible(random);
			addAndMakeVisible(create);

			makeToggleButton(dc, "DC");
			makeToggleButton(normalize, "N");
			makeToggleButton(windowing, "W");
			makeTextButton(random, "R", false);
			makeTextButton(create, "C", false);

			dc.onClick.push_back([&](Button& btn, const Mouse&)
			{
				parser.postFX[FormulaParser::DCOffset] = dc.toggleState == 1;
				
				auto& props = btn.utils.getProps();
				auto user = props.getUserSettings();
				if (user != nullptr)
				{
					user->setValue("Parser_DCOffset", dc.toggleState);
					user->save();
				}
			});
			normalize.onClick.push_back([&](Button& btn, const Mouse&)
			{
				parser.postFX[FormulaParser::Normalize] = normalize.toggleState == 1;
				auto& props = btn.utils.getProps();
				auto user = props.getUserSettings();
				if (user != nullptr)
				{
					user->setValue("Parser_Normalize", normalize.toggleState);
					user->save();
				}
			});
			windowing.onClick.push_back([&](Button& btn, const Mouse&)
			{
				parser.postFX[FormulaParser::Windowing] = windowing.toggleState == 1;
				auto& props = btn.utils.getProps();
				auto user = props.getUserSettings();
				if (user != nullptr)
				{
					user->setValue("Parser_Windowing", windowing.toggleState);
					user->save();
				}
			});
			random.onClick.push_back([&](Button&, const Mouse&)
			{
				fx::Tokens postfix;
				fx::generateTerm(postfix, 5, .75f, -1.f, 1.f);
				if (parser.fx(postfix))
				{
					parser.updateFormula();
					notify(EvtType::FormulaUpdated);
				}
				
				//DBG("INFIX:");
				//fx::Tokens infix;
				//fx::toInfix(infix, postfix);
				//DBG(fx::toString(infix));
			});
			create.onClick.push_back([&](Button&, const Mouse&)
			{
				parser.onReturn();
			});

			auto& props = u.getProps();
			auto user = props.getUserSettings();
			if (user != nullptr)
			{
				dc.toggleState = user->getIntValue("Parser_DCOffset", 0);
				normalize.toggleState = user->getIntValue("Parser_Normalize", 0);
				windowing.toggleState = user->getIntValue("Parser_Windowing", 0);
			}
			else
			{
				dc.toggleState = 0;
				normalize.toggleState = 0;
				windowing.toggleState = 0;
			}
			
			parser.postFX[FormulaParser::DCOffset] = dc.toggleState == 1;
			parser.postFX[FormulaParser::Normalize] = normalize.toggleState == 1;
			parser.postFX[FormulaParser::Windowing] = windowing.toggleState == 1;
		}

		void paint(Graphics&) {}

		void resized() override
		{
			layout.resized();

			layout.place(parser, 0, 0, 5, 1);

			layout.place(dc, 0, 1, 1, 1);
			layout.place(windowing, 1, 1, 1, 1);
			layout.place(normalize, 2, 1, 1, 1);
			layout.place(random, 3, 1, 1, 1);
			layout.place(create, 4, 1, 1, 1);
		}

		FormulaParser parser;
		Button dc, normalize, windowing, random, create;
	};
}