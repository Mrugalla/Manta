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

        enum class LooksType
        {
            Default,
            VerticalSlider,
            NumTypes
        };
    };

    /* knob, name, tooltip, pseudo-parameter, looksType */
    void makePseudoParameter(Knob&, const String&, String&&, std::atomic<float>*, Knob::LooksType = Knob::LooksType::Default);

    /* knob, pID, name, modulatable, meter, looksType */
    void makeParameter(Knob&, PID, const String&, bool = true, const std::atomic<float>* = nullptr, Knob::LooksType = Knob::LooksType::Default);

    /* knob, pIDs, name, modulatable, meter, looksType */
	void makeParameter(Knob&, const std::vector<PID>&, const String&, bool = true, const std::atomic<float>* = nullptr, Knob::LooksType = Knob::LooksType::Default);

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