#pragma once
#include "Label.h"

namespace gui
{
    static constexpr float SensitiveDrag = .2f;
    static constexpr float WheelDefaultSpeed = .02f;
    static constexpr float WheelInertia = .9f;

    static constexpr float LockAlpha = .4f;

    struct Parametr :
        public Comp
    {
        struct ModDial :
            public Comp
        {
            enum class State
            {
                MaxModDepth,
                Bias,
                NumStates
            };
            
            static constexpr int NumStates = static_cast<int>(State::NumStates);

            ModDial(Utils&, Parametr&);

            void paint(Graphics&) override;

            State getState() const noexcept;

        protected:
            Parametr& parametr;
            Label label;
            float dragY;
        public:
            State state;

            void resized() override;

            void mouseDown(const Mouse&) override;

            void mouseDrag(const Mouse&) override;

            void mouseUp(const Mouse&) override;

        };

        Parametr(Utils&, PID, bool /*_modulatable*/ = true);

        PID getPID() const noexcept;

        void setLocked(bool);

    protected:
        Param& param;
        ModDial modDial;
        float valNorm, maxModDepth, valMod, modBias;
        bool locked;
        const bool modulatable;
    };
}