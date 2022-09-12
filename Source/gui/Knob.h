#pragma once
#include "GUIParams.h"
#include "ContextMenu.h"
#include <functional>

namespace gui
{
    struct Knob :
        public Comp,
        public Timer
    {
        using Func = std::function<void(Knob&)>;
        using OnDrag = std::function<void(Knob&, PointF&, bool)>;
        using OnUp = std::function<void(Knob&, const Mouse&)>;
        using OnTimer = std::function<bool(Knob&)>;
        using OnPaint = std::function<void(Knob&, Graphics&)>;
        using GetInfo = std::function<String(int)>;

        Knob(Utils&, const String & /*name*/ = "", const String & /*tooltip*/ = "", CursorType = CursorType::Interact);

        ~Knob();

        void init(std::vector<int>&&, std::vector<int>&&);

        void timerCallback() override;

        void paint(juce::Graphics&) override;

        void resized() override;

        void mouseEnter(const Mouse&) override;

        void mouseExit(const Mouse&) override;

        void mouseDown(const Mouse&) override;

        void mouseDrag(const Mouse&) override;

        void mouseUp(const Mouse&) override;

        void mouseWheelMove(const Mouse&, const MouseWheel&) override;

        void mouseDoubleClick(const Mouse&) override;

        void setLocked(bool);

        Func onEnter, onExit, onDown, onWheel, onResize, onDoubleClick;
        OnDrag onDrag;
        OnUp onUp;
        OnTimer onTimer;
        OnPaint onPaint;
        GetInfo getInfo;
        Label label;
        PointF dragXY;
        BoundsF knobBounds;
        std::vector<float> values;
        std::vector<std::unique_ptr<Comp>> comps;
        std::vector<int> states;
        bool hidesCursor, locked;
        CursorType activeCursor;
    };

	/* knob, pID, name, modulatable, meter */
    void makeParameter(Knob&, PID, const String&, bool = true, const std::atomic<float>* = nullptr);

    /* knob, pID, name, onPaint modulatable, meter */
    void makeParameter(Knob&, PID, const String&, const Knob::OnPaint&, bool = true, const std::atomic<float>* = nullptr);

	struct ContextMenuKnobs :
		public ContextMenu
	{
		Notify makeNotify2(ContextMenuKnobs&);

		ContextMenuKnobs(Utils&);
	};



    struct TextEditorKnobs :
        public TextEditor
    {
        Notify makeNotify(TextEditorKnobs&);

        TextEditorKnobs(Utils&);

        void paint(Graphics&) override;
    };

}