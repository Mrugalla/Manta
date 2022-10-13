#pragma once
#include "../arch/FormularParser.h"
#include "TextEditor.h"

namespace gui
{
	template<size_t Size>
	struct FormularParser :
		public TextEditor
	{
		using Parser = parser::Parser;

		FormularParser(Utils& u, String&& _tooltip, std::vector<float*>& tables) :
			TextEditor(u, _tooltip, "enter some math"),
			parser(Size)
		{
			onReturn = [&, tables]()
			{
				auto successful = parser(txt);
				if (!successful)
					return false;
				
				for (auto i = 0; i < Size; ++i)
					tables[0][i] = parser[i];

				for (auto i = 1; i < tables.size(); ++i)
					SIMD::copy(tables[i], tables[0], tables.size());

				return true;
			};
		}
		
	protected:
		Parser parser;
	};
}