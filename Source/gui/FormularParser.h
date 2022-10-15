#pragma once
#include "../arch/FormularParser2.h"
#include "TextEditor.h"

namespace gui
{
	template<size_t Size>
	struct FormularParser :
		public TextEditor
	{
		using Parser = fx::Parser;

		FormularParser(Utils& u, String&& _tooltip, std::vector<float*>& tables) :
			TextEditor(u, _tooltip, "enter some math"),
			fx()
		{
			onReturn = [&, tables]()
			{
				if(!fx(txt))
					return false;
				
				auto x = -1.f;
				const auto inc = 2.f / static_cast<float>(Size);
				for (auto i = 0; i < Size; ++i, x += inc)
					tables[0][i] = fx(x);

				auto numTables = tables.size();
				for (auto i = 1; i < numTables; ++i)
					SIMD::copy(tables[i], tables[0], Size);

				return true;
			};
		}
		
	protected:
		Parser fx;
	};
}