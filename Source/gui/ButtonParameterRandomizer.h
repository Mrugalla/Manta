#pragma once
#include "Button.h"

namespace gui
{
	struct ButtonParameterRandomizer :
		public Button
	{
        using RandFunc = std::function<void(Random&)>;

        ButtonParameterRandomizer(Utils&);

        void add(Param*);

        void add(std::vector<Param*>&&);

        void add(const std::vector<Param*>&);

        void add(const RandFunc&);

        void operator()();

		std::vector<Param*> randomizables;
        std::vector<RandFunc> randFuncs;
    
    protected:
        void mouseUp(const Mouse&) override;

        void mouseExit(const Mouse&) override;

        String makeTooltip();
	};
}