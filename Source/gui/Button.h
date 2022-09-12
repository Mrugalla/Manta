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
		using OnMouseWheel = std::function<void(const Mouse&, const MouseWheel&)>;

		void enableLabel(const String&);

		void enableLabel(std::vector<String>&&);

		void enableParameterSwitch(PID);

		void enableParameter(PID, int /*val*/);

		/* utils, tooltip */
		Button(Utils&, String&& = "");

		Label& getLabel() noexcept;

		const String& getText() const noexcept;

		std::vector<OnClick> onClick, onRightClick, onTimer;
		std::vector<OnPaint> onPaint;
		std::vector<OnMouseWheel> onMouseWheel;
		BlinkyBoy blinkyBoy;
		int toggleState;
		PID pID;
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
		NumSymbols
	};

	void paintAbort(Graphics&, BoundsF);

	/* button, symbol, targetToggleState */
	void makeSymbolButton(Button&, ButtonSymbol, int = 1);

	void makeToggleButton(Button&, const String&);

	void makeParameterSwitchButton(Button&, PID, String&& /*text*/);

	void makeParameterSwitchButton(Button&, PID, ButtonSymbol);

	template<size_t NumButtons>
	void makeParameterButtonsGroup(std::array<Button, NumButtons>&, PID, const char* /*txt*/, bool /*onlyText*/);

	void makeButtonsGroup(std::vector<std::unique_ptr<Button>>&, int /*defaultToggleStateIndex*/ = 0);

	void makeURLButton(Button&, String&& /*urlPath*/);
}

/*

toggleState == 1
	has glow

*/