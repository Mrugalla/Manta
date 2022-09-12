#include "Knob.h"

namespace gui
{
    // KNOB

    Knob::Knob(Utils& u, const String& _name, const String& _tooltip, CursorType _cursorType) :
        Comp(u, _tooltip, _cursorType),
        onEnter([](Knob&) {}),
        onExit([](Knob&) {}),
        onDown([](Knob&) {}),
        onWheel([](Knob&) {}),
        onResize([](Knob&) {}),
        onDoubleClick([](Knob&) {}),
        onDrag([](Knob&, PointF&, bool) {}),
        onUp([](Knob&, const Mouse&) {}),
        onTimer([](Knob&) { return false; }),
        onPaint([](Knob&, Graphics&) {}),
        getInfo([](int) { return ""; }),
        label(u, _name),
        dragXY(),
        knobBounds(),
        values(),
        comps(),
        states(),
        hidesCursor(true),
        locked(false),
        activeCursor(_cursorType)
    {
        setName(_name);

        label.textCID = ColourID::Txt;
        label.just = Just::centred;
        label.mode = Label::Mode::TextToLabelBounds;

        addAndMakeVisible(label);
    }

    Knob::~Knob()
    {
		
    }

    void Knob::init(std::vector<int>&& distX, std::vector<int>&& distY)
    {
        layout.init(
            distX,
            distY
        );
    }

    void Knob::timerCallback()
    {
        if (onTimer(*this))
            repaint();
    }

    void Knob::paint(juce::Graphics& g)
    {
        onPaint(*this, g);
    }

    void Knob::resized()
    {
        layout.resized();
        onResize(*this);
    }

    void Knob::mouseEnter(const Mouse& mouse)
    {
        Comp::mouseEnter(mouse);
        onEnter(*this);
    }

    void Knob::mouseExit(const Mouse&)
    {
        onExit(*this);
    }

    void Knob::mouseDown(const Mouse& mouse)
    {
        if (mouse.mods.isLeftButtonDown())
        {
            if (hidesCursor)
                hideCursor();

            dragXY.setXY(
                mouse.position.x,
                mouse.position.y
            );

            onDown(*this);
        }
    }

    void Knob::mouseDrag(const Mouse& mouse)
    {
        if (mouse.mods.isLeftButtonDown())
        {
            auto dragOffset = mouse.position - dragXY;
            auto shiftDown = mouse.mods.isShiftDown();
            onDrag(*this, dragOffset, shiftDown);
            dragXY = mouse.position;
        }
    }

    void Knob::mouseUp(const Mouse& mouse)
    {
        onUp(*this, mouse);

        if (hidesCursor)
            showCursor(*this);
    }

    void Knob::mouseWheelMove(const Mouse& mouse, const MouseWheel& wheel)
    {
        if (mouse.mods.isAnyMouseButtonDown())
            return;

        const bool reversed = wheel.isReversed ? -1.f : 1.f;
        const bool isTrackPad = wheel.deltaY * wheel.deltaY < .0549316f;
        if (isTrackPad)
            dragXY.setXY(
                reversed * wheel.deltaX,
                reversed * wheel.deltaY
            );
        else
        {
            const auto deltaXPos = wheel.deltaX > 0.f ? 1.f : -1.f;
            const auto deltaYPos = wheel.deltaY > 0.f ? 1.f : -1.f;
            dragXY.setXY(
                reversed * WheelDefaultSpeed * deltaXPos,
                reversed * WheelDefaultSpeed * deltaYPos
            );
        }

        if (mouse.mods.isShiftDown())
            dragXY *= SensitiveDrag;

        onWheel(*this);
    }

    void Knob::mouseDoubleClick(const Mouse&)
    {
        onDoubleClick(*this);
    }

    void Knob::setLocked(bool lckd)
    {
        locked = lckd;
        if (locked)
        {
            setCursorType(CursorType::Inactive);
            setAlpha(.2f);
        }
        else
        {
            setCursorType(activeCursor);
            setAlpha(1.f);
        }
    }


    // FREE FUNC

    void makeParameter(Knob& knob, PID pID, const String& name, const Knob::OnPaint& onPaint, bool modulatable, const std::atomic<float>* meter)
    {
        //const bool hasMeter = meter != nullptr;

        knob.getInfo = [pID](int i)
        {
            if (i == 0)
                return toString(pID);
            return String();
        };

        knob.setName(name);
        knob.label.setText(name);
        knob.setTooltip(param::toTooltip(pID));

        auto& utils = knob.getUtils();
        auto param = utils.getParam(pID);
        const auto angleWidth = PiQuart * 3.f;
        const auto angleRange = angleWidth * 2.f;

        knob.locked = param->isLocked();

        knob.onEnter = [param](Knob& k)
        {
            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();
        };

        knob.onExit = [](Knob& k)
        {
            k.label.setText(k.getName());
            k.label.repaint();
        };

        knob.onDown = [param](Knob& k)
        {
            if (param->isInGesture())
                return;
            param->beginGesture();

            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();
        };

        knob.onDrag = [param](Knob& k, PointF& dragOffset, bool shiftDown)
        {
            if (shiftDown)
                dragOffset *= SensitiveDrag;

            const auto speed = 1.f / k.getUtils().getDragSpeed();

            const auto newValue = juce::jlimit(0.f, 1.f, param->getValue() - dragOffset.y * speed);
            param->setValueNotifyingHost(newValue);

            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();

            k.notify(EvtType::ParametrDragged, &k);
        };

        knob.onUp = [param, angleWidth, angleRange](Knob& k, const Mouse& mouse)
        {
            if (mouse.mods.isLeftButtonDown())
            {
                if (!mouse.mouseWasDraggedSinceMouseDown())
                {
                    if (mouse.mods.isCtrlDown())
                        param->setValueNotifyingHost(param->getDefaultValue());
                    else
                    {
                        PointF centre(
                            static_cast<float>(k.getWidth()) * .5f,
                            static_cast<float>(k.getHeight()) * .5f
                        );
                        const LineF fromCentre(centre, mouse.position);
                        const auto angle = fromCentre.getAngle();

                        const auto newValue = juce::jlimit(0.f, 1.f, (angle + angleWidth) / angleRange);
                        param->setValue(newValue);
                    }
                }
                param->endGesture();
            }
            else if (mouse.mods.isRightButtonDown())
                if (!mouse.mouseWasDraggedSinceMouseDown())
                    if (mouse.mods.isCtrlDown())
                        param->setValueWithGesture(param->getDefaultValue());
                    else
                        k.notify(EvtType::ParametrRightClicked, &k);

            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();
        };

        knob.onWheel = [param](Knob& k)
        {
            const auto newValue = juce::jlimit(0.f, 1.f, param->getValue() + k.dragXY.y);
            param->setValueWithGesture(newValue);

            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();
        };

        knob.onDoubleClick = [param](Knob& k)
        {
            const auto dVal = param->getDefaultValue();

            while (param->isInGesture()) {}
            param->setValueWithGesture(dVal);

            k.label.setText(param->getCurrentValueAsText());
            k.label.repaint();

            k.notify(EvtType::ParametrDragged, &k);
        };

        enum { Value, MaxModDepth, ValMod, ModBias, Meter, NumValues };
        knob.values.reserve(NumValues);
        auto& values = knob.values;

        values.emplace_back(param->getValue());
        values.emplace_back(param->getMaxModDepth());
        values.emplace_back(param->getValMod());
        values.emplace_back(param->getModBias());
        values.emplace_back(0.f);

        if (meter != nullptr)
            knob.onTimer = [param, meter](Knob& k)
        {
            const auto lckd = param->isLocked();
            if (k.locked != lckd)
                k.locked = lckd;

            const auto vn = param->getValue();
            const auto mmd = param->getMaxModDepth();
            const auto vm = param->getValMod();
            const auto mb = param->getModBias();
            const auto metr = std::floor(meter->load() * 128.f) * .0078125f;

            auto& vals = k.values;

            if (vals[Value] != vn || vals[MaxModDepth] != mmd || vals[ValMod] != vm || vals[ModBias] != mb || vals[Meter] != metr)
            {
                vals[Value] = vn;
                vals[MaxModDepth] = mmd;
                vals[ValMod] = vm;
                vals[ModBias] = mb;
                vals[Meter] = metr;
                return true;
            }

            return false;
        };
        else
            knob.onTimer = [param](Knob& k)
        {
            const auto lckd = param->isLocked();
            if (k.locked != lckd)
                k.setLocked(lckd);

            const auto vn = param->getValue();
            const auto mmd = param->getMaxModDepth();
            const auto vm = param->getValMod();
            const auto mb = param->getModBias();

            auto& vals = k.values;

            if (vals[Value] != vn || vals[MaxModDepth] != mmd || vals[ValMod] != vm || vals[ModBias] != mb)
            {
                vals[Value] = vn;
                vals[MaxModDepth] = mmd;
                vals[ValMod] = vm;
                vals[ModBias] = mb;
                return true;
            }

            return false;
        };

        enum { ModDial, NumComps };

        if (modulatable)
        {
            auto modDial = std::make_unique<Knob>(knob.getUtils(), "M", "Drag this to modulate the macro's modulation depth.", CursorType::Mod);
            {
                auto& dial = *modDial;

                auto& label = dial.label;
                label.mode = Label::Mode::TextToLabelBounds;
                label.textCID = ColourID::Bg;

                enum { StateMaxModDepth, StateModBias, NumStates };
                dial.states.push_back(StateMaxModDepth);

                dial.onResize = [](Knob& k)
                {
                    k.knobBounds = k.getLocalBounds().toFloat();
                    const auto thicc = k.getUtils().thicc * .5f;
                    k.label.setBounds(k.knobBounds.reduced(thicc).toNearestInt());
                };

                dial.onPaint = [](Knob& k, Graphics& g)
                {
                    auto state = k.states[0];

                    Colour col;
                    switch (state)
                    {
                    case StateMaxModDepth:
                        col = Colours::c(ColourID::Mod);
                        break;
                    case StateModBias:
                        col = Colours::c(ColourID::Bias);
                        break;
                    }
                    g.setColour(col);
                    g.fillEllipse(k.knobBounds);
                };

                dial.onDrag = [param](Knob& k, PointF& dragOffset, bool shiftDown)
                {
                    if (shiftDown)
                        dragOffset *= SensitiveDrag;

                    auto state = k.states[0];
                    auto& utils = k.getUtils();
                    if (state == StateModBias)
                        dragOffset *= .5f;
                    const auto speed = 1.f / utils.getDragSpeed();
                    dragOffset *= speed;

                    float newValue;
                    switch (state)
                    {
                    case StateMaxModDepth:
                        newValue = param->getMaxModDepth() - dragOffset.y;
                        param->setMaxModDepth(newValue);
                        break;
                    case StateModBias:
                        newValue = param->getModBias() - dragOffset.y;
                        param->setModBias(newValue);
                        break;
                    }
                };

                dial.onUp = [param](Knob& k, const Mouse& mouse)
                {
                    if (!mouse.mouseWasDraggedSinceMouseDown())
                    {
                        if (mouse.mods.isCtrlDown())
                        {
                            param->setMaxModDepth(0.f);
                            param->setModBias(.5f);
                        }
                        else if (mouse.mods.isRightButtonDown())
                        {
                            auto& state = k.states[0];
                            state = (state + 1) % NumStates;

                            switch (state)
                            {
                            case StateMaxModDepth:
                                k.label.setText("M");
                                k.setCursorType(CursorType::Mod);
                                k.activeCursor = CursorType::Mod;
                                break;
                            case StateModBias:
                                k.label.setText("B");
                                k.setCursorType(CursorType::Bias);
                                k.activeCursor = CursorType::Bias;
                                break;
                            }

                            repaintWithChildren(&k);
                        }
                    }
                };

                dial.onDoubleClick = [param](Knob&)
                {
                    param->setMaxModDepth(0.f);
                    param->setModBias(.5f);
                };

                knob.comps.push_back(std::move(modDial));
                knob.addAndMakeVisible(*knob.comps.back());
            }
        }

        if (modulatable)
            knob.onResize = [](Knob& k)
        {
            const auto thicc = k.getUtils().thicc;
            auto& layout = k.getLayout();

            k.knobBounds = layout(0, 0, 3, 2, true).reduced(thicc);
            layout.place(k.label, 0, 2, 3, 1, false);
            layout.place(*k.comps[ModDial], 1, 1, 1, 1, true);
        };
        else
        {
            knob.onResize = [](Knob& k)
            {
                const auto thicc = k.getUtils().thicc;
                auto& layout = k.getLayout();

                k.knobBounds = layout(0, 0, 3, 2, true).reduced(thicc);
                layout.place(k.label, 0, 2, 3, 1, false);
            };
        }

        knob.onPaint = onPaint;

        knob.init
        (
            { 40, 40, 40 },
            { 100, 40, 40 }
        );

        knob.startTimerHz(PPDFPSKnobs);
    }

    void makeParameter(Knob& knob, PID pID, const String& name, bool modulatable, const std::atomic<float>* meter)
    {
        const bool hasMeter = meter != nullptr;
        const auto angleWidth = PiQuart * 3.f;
        const auto angleRange = angleWidth * 2.f;
        enum { Value, MaxModDepth, ValMod, ModBias, Meter, NumValues };
		
        const auto paintLines = [angleWidth](Graphics& g, Colour col, const PointF& centre, float radius, float radiusInner, float radDif, Stroke& strokeType)
        {
            g.setColour(col);
            Path arcOutline;

            arcOutline.addCentredArc(
                centre.x, centre.y,
                radius, radius,
                0.f,
                -angleWidth, angleWidth,
                true
            );
            g.strokePath(arcOutline, strokeType);

            Path arcInline;
            arcInline.addCentredArc(
                centre.x, centre.y,
                radiusInner, radiusInner,
                0.f,
                -angleWidth, angleWidth,
                true
            );
            auto stroke2 = strokeType;
            stroke2.setStrokeThickness(radDif);
            g.strokePath(arcInline, stroke2);
        };

        const auto paintMod = [angleRange, angleWidth](Graphics& g, const PointF& centre, const float* vals,
            float valNormAngle, float valAngle, float thicc, float thicc2, float radius, float radiusInner, float radiusExt,
            float radDif, Stroke strokeType)
        {
            const auto valModAngle = vals[ValMod] * angleRange;
            const auto modAngle = -angleWidth + valModAngle;
            const auto modTick = LineF::fromStartAndAngle(centre, radiusExt, modAngle);

            g.setColour(Colours::c(ColourID::Bg));
            g.drawLine(modTick, thicc * 4.f);

            const auto maxModDepthAngle = juce::jlimit(-angleWidth, angleWidth, valNormAngle + vals[MaxModDepth] * angleRange - angleWidth);
            const auto biasAngle = angleRange * vals[ModBias] - angleWidth;

            g.setColour(Colours::c(ColourID::Bias));
            {
                Path biasPath;
                biasPath.addCentredArc(
                    centre.x, centre.y,
                    radiusInner, radiusInner,
                    0.f,
                    0.f, biasAngle,
                    true
                );
                auto bStroke = strokeType;
                bStroke.setStrokeThickness(radDif);

                g.strokePath(biasPath, bStroke);
            }

            g.setColour(Colours::c(ColourID::Mod));
            g.drawLine(modTick.withShortenedStart(radiusInner), thicc2);
            {
                Path modPath;
                modPath.addCentredArc(
                    centre.x, centre.y,
                    radius, radius,
                    0.f,
                    maxModDepthAngle, valAngle,
                    true
                );
                g.strokePath(modPath, strokeType);
            }
        };

        const auto paintTick = [](Graphics& g, Colour col, const PointF& centre,
            float radius, float radiusInner, float valAngle,
            float thicc, float thicc3, float thicc5)
        {
            const auto tickLine = LineF::fromStartAndAngle(centre, radius, valAngle);
            g.setColour(Colours::c(ColourID::Bg));
            g.drawLine(tickLine, thicc5);
            g.setColour(col);
            g.drawLine(tickLine.withShortenedStart(radiusInner - thicc), thicc3);
        };

        Knob::OnPaint onPaint;
        
        if (hasMeter)
            onPaint = [paintLines, paintMod, paintTick, angleWidth, angleRange, modulatable](Knob& k, Graphics& g)
        {
            const auto& vals = k.values;
            const auto thicc = k.getUtils().thicc;
            const auto thicc2 = thicc * 2.f;
            const auto thicc3 = thicc * 3.f;
            const auto thicc5 = thicc * 5.f;
            Stroke strokeType(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::butt);
            const auto radius = k.knobBounds.getWidth() * .5f;
            const auto radiusInner = radius * .8f;
            const auto radDif = (radius - radiusInner) * .8f;
            const auto radiusBetween = radiusInner + radDif;

            PointF centre
            (
                radius + k.knobBounds.getX(),
                radius + k.knobBounds.getY()
            );

            const auto col = Colours::c(ColourID::Interact);

            // METER
            if (vals[Meter] != 0.f)
            {
                g.setColour(Colours::c(ColourID::Txt));
                Path meterArc;

                const auto metr = vals[Meter] > 1.f ? 1.f : vals[Meter];

                const auto meterAngle = angleRange * metr - angleWidth;

                meterArc.addCentredArc(
                    centre.x, centre.y,
                    radiusBetween, radiusBetween,
                    0.f,
                    -angleWidth, meterAngle,
                    true
                );

                strokeType.setStrokeThickness(radDif);
                g.strokePath(meterArc, strokeType);
                strokeType.setStrokeThickness(thicc);
            }

            paintLines(g, col, centre, radius, radiusInner, radDif, strokeType);

            const auto valNormAngle = vals[Value] * angleRange;
            const auto valAngle = -angleWidth + valNormAngle;
            const auto radiusExt = radius + thicc;

            // draw modulation
            if (modulatable)
            {
                paintMod
                (
                    g,
                    centre,
                    vals.data(), valNormAngle, valAngle,
                    thicc, thicc2,
                    radius, radiusInner, radiusExt, radDif,
                    strokeType
                );
            }
				
            paintTick
            (
                g,
                col,
                centre,
                radius, radiusInner, valAngle,
                thicc, thicc3, thicc5
            );
        };
        else
            onPaint = [paintLines, paintMod, paintTick, angleWidth, angleRange, modulatable](Knob& k, Graphics& g)
        {
            const auto& vals = k.values;
            const auto thicc = k.getUtils().thicc;
            const auto thicc2 = thicc * 2.f;
            const auto thicc3 = thicc * 3.f;
            const auto thicc5 = thicc * 5.f;
            Stroke strokeType(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::butt);
            const auto radius = k.knobBounds.getWidth() * .5f;
            const auto radiusInner = radius * .8f;
            const auto radDif = (radius - radiusInner) * .8f;

            PointF centre
            (
                radius + k.knobBounds.getX(),
                radius + k.knobBounds.getY()
            );

            const auto col = Colours::c(ColourID::Interact);

            paintLines(g, col, centre, radius, radiusInner, radDif, strokeType);

            const auto valNormAngle = vals[Value] * angleRange;
            const auto valAngle = -angleWidth + valNormAngle;
            const auto radiusExt = radius + thicc;

            // draw modulation
            if (modulatable)
            {
                paintMod
                (
                    g,
                    centre,
                    vals.data(), valNormAngle, valAngle,
                    thicc, thicc2,
                    radius, radiusInner, radiusExt, radDif,
                    strokeType
                );
            }
				
            paintTick
            (
                g,
                col,
                centre,
                radius, radiusInner, valAngle,
                thicc, thicc3, thicc5
            );
        };
        
        makeParameter(knob, pID, name, onPaint, modulatable, meter);
    }

    // CONTEXT MENU

    Notify ContextMenuKnobs::makeNotify2(ContextMenuKnobs& popUp)
    {
        return [&pop = popUp](EvtType type, const void* stuff)
        {
            if (type == EvtType::ParametrRightClicked)
            {
                const auto& knob = *static_cast<const Knob*>(stuff);
                auto& utils = pop.getUtils();
                
                const auto pIDStr = knob.getInfo(0);
                const PID pID = param::toPID(pIDStr);
                auto& param = *utils.getParam(pID);

                pop.setButton([&p = param](Button&)
                {
                    Random rand;
                    p.setValueWithGesture(rand.nextFloat());
                }, 0);
                pop.setButton([&p = param](Button&)
                {
                    juce::Random rand;
                    auto val = p.getValue();
                    val += .05f * (rand.nextFloat() - .5f);
                    p.setValueWithGesture(juce::jlimit(0.f, 1.f, val));
                }, 1);
                pop.setButton([&p = param](Button&)
                {
                    const auto val = p.getDefaultValue();
                    p.setValueWithGesture(val);
                }, 2);
                pop.setButton([&p = param](Button&)
                {
                    p.setDefaultValue(p.getValue());
                }, 3);
                pop.setButton([&p = param](Button&)
                {
                    p.switchLock();
                }, 4);
                pop.setButton([&u = utils, pID = pID](Button&)
                {
                    u.assignMIDILearn(pID);
                }, 5);
                pop.setButton([&u = utils, pID = pID](Button&)
                {
                    u.removeMIDILearn(pID);
                }, 6);
                pop.setButton([&u = utils, &k = knob](Button&)
                {
                    u.getEventSystem().notify(EvtType::EnterParametrValue, &k);
                }, 7);

                pop.place(&knob);
            }
        };
    }

    ContextMenuKnobs::ContextMenuKnobs(Utils& u) :
        ContextMenu(u)
    {
        evts.push_back({ utils.getEventSystem(), makeNotify2(*this) });

        buttons.reserve(7);
        addButton("Randomize", "Randomize this parameter value.");
        addButton("Rand Relative", "Randomize this parameter value relative to its current value.");
        addButton("Load Default", "Resets this parameter value to its default value.");
        addButton("Save Default", "Saves this parameter value as its default one.");
        addButton("Lock / Unlock", "Parameter values are locked into place, even when changing presets.");
        addButton("MIDI Learn", "Click here to assign this parameter to a hardware control.");
        addButton("MIDI Unlearn", "Click here to remove this parameter from its hardware control(s).");
        addButton("Enter Value", "Click here to enter a parameter value with your keyboard");

        init();
    }

    // TEXT EDITOR KNOB

    Notify TextEditorKnobs::makeNotify(TextEditorKnobs& tek)
    {
        return [&editor = tek](EvtType type, const void* stuff)
        {
            if (type == EvtType::ClickedEmpty)
            {
                editor.disable();
                editor.setVisible(false);
            }
            if (type == EvtType::EnterParametrValue)
            {
                editor.disable();
                editor.txt.clear();

                const auto& knob = *static_cast<const Knob*>(stuff);
                const auto pID = param::toPID(knob.getInfo(0));
                auto& utils = editor.getUtils();
                auto& param = *utils.getParam(pID);

                editor.onEscape = [&tek = editor]()
                {
                    tek.disable();
                };

                editor.onReturn = [&tek = editor, &prm = param]()
                {
                    if (tek.txt.isNotEmpty())
                    {
                        const auto val = juce::jlimit(0.f, 1.f, prm.getValueForText(tek.txt));
                        prm.setValueWithGesture(val);
                    }
                    tek.disable();
                };

                const auto mouse = juce::Desktop::getInstance().getMainMouseSource();
                const auto screenPos = utils.getScreenPosition();
                const auto parametrScreenPos = knob.getScreenPosition();
                const auto parametrPos = parametrScreenPos - screenPos;
                const Point parametrCentre(
                    knob.getWidth() / 2,
                    knob.getHeight() / 2
                );
                editor.setCentrePosition(parametrPos + parametrCentre);
                editor.enable();
            }
        };
    }

    TextEditorKnobs::TextEditorKnobs(Utils& u) :
        TextEditor(u, "Enter a value for this parameter.", makeNotify(*this))
    {

    }

    void TextEditorKnobs::paint(Graphics& g)
    {
        const auto thicc = utils.thicc;
        g.setColour(Colours::c(ColourID::Darken));
        g.fillRoundedRectangle(getLocalBounds().toFloat(), thicc);

        TextEditor::paint(g);
    }
	
}

