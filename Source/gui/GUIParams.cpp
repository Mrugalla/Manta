#include "GUIParams.h"

namespace gui
{
    Parametr::ModDial::ModDial(Utils& u, Parametr& _parametr) :
        Comp(u, "Drag this to modulate the macro's modulation depth.", CursorType::Mod),
        parametr(_parametr),
        label(u, "M"),
        dragY(0.f),
        state(State::MaxModDepth)
    {
        label.mode = Label::Mode::TextToLabelBounds;
        label.textCID = ColourID::Bg;
        addAndMakeVisible(label);
    }

    void Parametr::ModDial::paint(Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        Colour col;
        switch (state)
        {
        case State::MaxModDepth:
            col = Colours::c(ColourID::Mod);
            break;
        case State::Bias:
            col = Colours::c(ColourID::Bias);
            break;
        }
        g.setColour(col);
        g.fillEllipse(bounds);
    }

    Parametr::ModDial::State Parametr::ModDial::getState() const noexcept { return state; }

    void Parametr::ModDial::resized()
    {
        const auto thicc = utils.thicc;
        label.setBounds(getLocalBounds().toFloat().reduced(thicc * .5f).toNearestInt());
    }

    void Parametr::ModDial::mouseDown(const Mouse& mouse)
    {
        switch (state)
        {
        case State::MaxModDepth:
            dragY = mouse.position.y / utils.getDragSpeed();
            break;
        case State::Bias:
            dragY = mouse.position.y / utils.getDragSpeed() * 2.f;
            break;
        }
    }

    void Parametr::ModDial::mouseDrag(const Mouse& mouse)
    {
        if (mouse.mods.isLeftButtonDown())
        {
            float dragYNew;
            switch (state)
            {
            case State::MaxModDepth:

                dragYNew = mouse.position.y / utils.getDragSpeed();
                break;
            case State::Bias:
                dragYNew = mouse.position.y / utils.getDragSpeed() * 2.f;
                break;
            default:
                dragYNew = 0.f;
                break;
            }
            auto dragOffset = dragYNew - dragY;
            if (mouse.mods.isShiftDown())
                dragOffset *= SensitiveDrag;
            float newValue;
            switch (state)
            {
            case State::MaxModDepth:
                newValue = parametr.param.getMaxModDepth() - dragOffset;
                parametr.param.setMaxModDepth(newValue);
                break;
            case State::Bias:
                newValue = parametr.param.getModBias() - dragOffset;
                parametr.param.setModBias(newValue);
                break;
            }

            dragY = dragYNew;
        }
    }

    void Parametr::ModDial::mouseUp(const Mouse& mouse)
    {
        if (!mouse.mouseWasDraggedSinceMouseDown())
            if (mouse.mods.isCtrlDown())
            {
                switch (state)
                {
                case State::MaxModDepth:

                    parametr.param.setMaxModDepth(0.f);
                    break;
                case State::Bias:
                    parametr.param.setModBias(.5f);
                    break;
                }
            }
            else if (mouse.mods.isRightButtonDown())
            {
                state = static_cast<State>((static_cast<int>(state) + 1) % NumStates);
                switch (state)
                {
                case State::MaxModDepth:
                    label.setText("M");
                    setCursorType(CursorType::Mod);
                    break;
                case State::Bias:
                    label.setText("B");
                    setCursorType(CursorType::Bias);
                    break;
                }
                repaintWithChildren(&parametr);
            }
    }

    Parametr::Parametr(Utils& u, PID _pID, bool _modulatable) :
        Comp(u, param::toTooltip(_pID)),
        param(*u.getParam(_pID)),
        modDial(u, *this),
        valNorm(param.getValue()),
        maxModDepth(param.getMaxModDepth()),
        valMod(param.getValMod()),
        modBias(param.getModBias()),
        locked(param.isLocked()),
        modulatable(_modulatable)
    {
        if (modulatable)
            addAndMakeVisible(modDial);

        setLocked(param.isLocked());
    }

    PID Parametr::getPID() const noexcept { return param.id; }

    void Parametr::setLocked(bool lckd)
    {
        locked = lckd;
        if (locked)
        {
            setCursorType(CursorType::Inactive);
            setAlpha(.2f);
        }
        else
        {
            setCursorType(CursorType::Interact);
            setAlpha(1.f);
        }
    }
}

