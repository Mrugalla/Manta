#pragma once
#include <vector>
#include <functional>

namespace evt
{
    enum class Type
    {
        ColourSchemeChanged,
        TooltipUpdated,
        ButtonClicked,
        ButtonRightClicked,
        ParametrRightClicked,
        ParametrDragged,
        ClickedEmpty,
        PatchUpdated,
        ComponentAdded,
        EnterParametrValue,
        BrowserOpened,
        BrowserClosed,
        FormulaUpdated,
        NumTypes
    };

    using Notify = std::function<void(const Type, const void*)>;

	struct System
	{
		struct Evt
		{
            Evt(System&);

            Evt(System&, const Notify&);

            Evt(System&, Notify&&);

            Evt(const Evt&);

            ~Evt();

            void operator()(const Type, const void* = nullptr) const;

            Notify notifier;
        protected:
            System& sys;
		};

        System();

        void notify(const Type, const void* = nullptr);

    protected:
		std::vector<Evt*> evts;

        void add(Evt*);
        
        void remove(const Evt*);
	};
}