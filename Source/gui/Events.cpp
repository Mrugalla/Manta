#include "Events.h"

namespace evt
{
    //SYSTEM::EVT

    System::Evt::Evt(System& _sys) :
        sys(_sys),
        notifier(nullptr)
    {
    }

    System::Evt::Evt(System& _sys, const Notify& _notifier) :
        sys(_sys),
        notifier(_notifier)
    {
        sys.add(this);
    }

    System::Evt::Evt(System& _sys, Notify&& _notifier) :
        sys(_sys),
        notifier(_notifier)
    {
        sys.add(this);
    }

    System::Evt::Evt(const Evt& other) :
        notifier(other.notifier),
        sys(other.sys)
    {
        sys.add(this);
    }

    System::Evt::~Evt()
    {
        sys.remove(this);
    }

    void System::Evt::operator()(const Type type, const void* stuff) const
    {
        sys.notify(type, stuff);
    }

    //SYSTEM

    System::System() :
        evts()
    {}

    void System::notify(const Type type, const void* stuff)
    {
        for (const auto e : evts)
            e->notifier(type, stuff);
    }

    void System::add(Evt* e)
    {
        evts.push_back(e);
    }

    void System::remove(const Evt* e)
    {
        for (auto i = 0; i < evts.size(); ++i)
            if (e == evts[i])
            {
                evts.erase(evts.begin() + i);
                return;
            }
    }
}

