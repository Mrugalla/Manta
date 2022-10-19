#pragma once
#include "Label.h"

namespace gui
{

	struct BlinkyBoy :
		public Timer
	{
		BlinkyBoy();

		void init(Comp* _comp, float timeInSecs) noexcept;

		Colour getInterpolated(Colour c0, Colour c1) const noexcept;

	protected:
		Comp* comp;
		float env, inv;

		void timerCallback() override;
	};

	struct Button :
		public Comp,
		public Timer
	{
		using OnClick = std::function<void(Button&)>;
		using OnPaint = std::function<void(Graphics&, Button&)>;
		using OnMouseWheel = std::function<void(Button&, const Mouse&, const MouseWheel&)>;

		void enableLabel(const String&);

		void enableLabel(const std::vector<String>&);

		void enableParameterSwitch(const std::vector<PID>&);

		/* pIDs */
		void enableParameter(const std::vector<PID>&);

		/* utils, tooltip, notify */
		Button(Utils&, String&& = "", Notify&& = [](EvtType, const void*){});

		Label& getLabel() noexcept;

		const String& getText() const noexcept;

		std::vector<OnClick> onClick, onRightClick, onTimer;
		std::vector<OnPaint> onPaint;
		std::vector<OnMouseWheel> onMouseWheel;
		BlinkyBoy blinkyBoy;
		int toggleState;
		std::vector<PID> pID;
		bool locked;
	protected:
		Label label;
		std::vector<String> toggleTexts;

		void resized() override;

		void paint(Graphics&) override;

		void mouseEnter(const Mouse&) override;

		void mouseExit(const Mouse&) override;

		void mouseUp(const Mouse&) override;

		void mouseWheelMove(const Mouse&, const MouseWheel&) override;

		void timerCallback() override;
	};

	Button::OnPaint buttonOnPaintDefault();

	/* button; text/name; withToggle; targetToggleState */
	void makeTextButton(Button&, const String&, bool = false, int = 1);
	
	/* button; toggletexts; withToggle; targetToggleState */
	void makeTextButton(Button&, const std::vector<String>&, bool = false, int = 1);

	enum class ButtonSymbol
	{
		Empty,
		Polarity,
		StereoConfig,
		UnityGain,
		Power,
		PatchMode,
		Settings,
		Random,
		Abort,
		SwapParamModDepth,
		ModDepthLock,
		Save,
		Load,
		Remove,
		TuningFork,
		Lookahead,
		NumSymbols
	};

	void paintAbort(Graphics&, BoundsF);

	/* button, symbol, targetToggleState */
	void makeSymbolButton(Button&, ButtonSymbol, int = 1);

	void makeToggleButton(Button&, const String&);

	void makeParameterSwitchButton(Button&, PID, String&& /*text*/);

	void makeParameterSwitchButton(Button&, PID, ButtonSymbol);

	void makeParameterSwitchButton(Button&, const std::vector<PID>&, ButtonSymbol);

	/* button, pIDs, text */
	void makeParameter(Button&, const std::vector<PID>&);

	template<size_t NumButtons>
	void makeParameterButtonsGroup(std::array<Button, NumButtons>&, PID, const char* /*txt*/, bool /*onlyText*/);

	void makeButtonsGroup(std::vector<std::unique_ptr<Button>>&, int /*defaultToggleStateIndex*/ = 0);

	void makeURLButton(Button&, String&& /*urlPath*/);
}

/*

toggleState == 1
	has glow

*/