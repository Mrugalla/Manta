#pragma once
#include "Knob.h"
#include "KeyboardComp.h"

namespace gui
{
	struct TuningEditorButton :
		public Button
	{
		TuningEditorButton(Utils& u, CompWidgetable* tu) :
			Button(u, "This button opens the tuning editor, where you can manipulate the way notes are interpreted.")
		{
			makeSymbolButton(*this, ButtonSymbol::TuningFork);

			toggleState = 0;

			onClick.push_back([tu](Button& btn, const Mouse&)
			{
				if (btn.toggleState == 0)
				{
					// open tuning editor
					tu->initWidget(.2f, false);
					btn.toggleState = 1;
				}
				else
				{
					// close tuning editor
					tu->initWidget(.1f, true);
					btn.toggleState = 0;
				}
			});
		}
	};

	struct XenWheel :
		public Knob
	{
		XenWheel(Utils& u) :
			Knob(u),
			xen(*u.getParam(PID::Xen)),
			xenModLabel(u, String(std::round(xen.getValModDenorm())) + " Xen")
		{
			enum { Value, MaxModDepth, ValMod, ModBias, Meter, NumValues };
			enum { ModDial, Lock, NumComps };

			addAndMakeVisible(xenModLabel);
			xenModLabel.textCID = ColourID::Mod;
			xenModLabel.font = getFontLobster();
			xenModLabel.mode = Label::Mode::TextToLabelBounds;
			xenModLabel.just = Just::centred;

			makeParameter(*this, PID::Xen, "Xen");
			
			onPaint = [&xenP = xen](Knob& knob, Graphics& g)
			{
				const auto& utils = knob.getUtils();
				const auto thicc = utils.thicc;
				Stroke stroke(thicc, Stroke::JointStyle::curved, Stroke::EndCapStyle::butt);

				const auto valNorm = knob.values[Value];
				const auto valDenorm = std::round(xenP.range.convertFrom0to1(valNorm));
				const auto valMod = knob.values[ValMod];
				const auto valModDenorm = std::round(xenP.range.convertFrom0to1(valMod));

				const auto maxModDepth = knob.values[MaxModDepth];

				const auto bounds = knob.knobBounds.reduced(thicc);
				const auto width = bounds.getWidth();
				const auto rad = width * .5f;
				const auto radHalf = rad * .5f;
				const auto boundsInner = bounds.reduced(radHalf);
				PointF centre
				(
					bounds.getX() + rad,
					bounds.getY() + rad
				);

				g.setColour(Colours::c(ColourID::Interact));
				g.drawEllipse(bounds, thicc);
				g.drawEllipse(boundsInner, thicc);

				const auto angleStart = Pi + PiHalf;
				const auto mag0 = radHalf;
				const auto mag1 = radHalf + radHalf * .5f;
				const auto mag2 = rad;

				g.setColour(Colours::c(ColourID::Mod));
				{
					Path arc;
					arc.addCentredArc
					(
						centre.x,
						centre.y,
						mag1,
						mag1,
						0.f,
						0.f,
						maxModDepth * Pi,
						true
					);
					g.strokePath(arc, stroke);
				}

				const auto valModDenormInv = 1.f / valModDenorm;

				for (auto i = 0.f; i < valModDenorm; ++i)
				{
					const auto r = i * valModDenormInv;
					const auto angle = angleStart + r * Tau;
					const auto cos = std::cos(angle);
					const auto sin = std::sin(angle);
					const auto x1 = cos * mag1;
					const auto y1 = sin * mag1;
					const auto x2 = cos * mag2;
					const auto y2 = sin * mag2;
					g.drawLine(centre.x + x1, centre.y + y1,
						centre.x + x2, centre.y + y2, thicc);
				}

				{
					//String str(valModDenorm);
					//g.setFont(getFontDosisExtraBold());
					//g.drawFittedText(str + " edo", boundsInner.toNearestInt(), Just::centred, 1);
				}


				const auto valDenormInv = 1.f / valDenorm;
				g.setColour(Colours::c(ColourID::Interact));

				for (auto i = 0.f; i < valDenorm; ++i)
				{
					const auto r = i * valDenormInv;
					const auto angle = angleStart + r * Tau;
					const auto cos = std::cos(angle);
					const auto sin = std::sin(angle);
					const auto x1 = cos * mag0;
					const auto y1 = sin * mag0;
					const auto x2 = cos * mag1;
					const auto y2 = sin * mag1;
					g.drawLine(centre.x + x1, centre.y + y1,
						centre.x + x2, centre.y + y2, thicc);
				}
			};

			onResize = [this](Knob& k)
			{
				const auto thicc = k.getUtils().thicc;
				auto& layout = k.getLayout();

				k.knobBounds = layout(0, 0, 3, 2, true).reduced(thicc);
				layout.place(k.label, 0, 2, 3, 1, false);
				layout.place(*k.comps[ModDial], 0, 1, 1, 1, true);
				layout.place(*k.comps[Lock], 2.5f, 1, .5f, 1, true);

				const auto width = knobBounds.getWidth();
				const auto rad = width * .5f;
				xenModLabel.setBounds(k.knobBounds.reduced(rad * .7f).toNearestInt());
			};
			
			const auto ot = onTimer;
			onTimer = [this, ot](Knob& k)
			{
				bool shouldRepaint = ot(k);

				if (shouldRepaint)
					xenModLabel.setText(String(std::round(xen.getValModDenorm())) + " Xen");

				return shouldRepaint;
			};
		}
		
	protected:
		const Param& xen;
		Label xenModLabel;
	};

	struct TuningEditor :
		public CompWidgetable
	{
		TuningEditor(Utils& u) :
			CompWidgetable(u, "", CursorType::Default),
			title(u, "Tuning Editor"),
			master(u, "Master"),
			selection(u, "Selection"),
			xen(u),
			baseNote(u),
			masterTune(u),
#if PPDMIDINumVoices != 0
			pitchbend(u),
#endif
			keys(u, "Scrub the keys to test your tuning settings."),
			gain(u)
		{
			setInterceptsMouseClicks(true, true);
			
			addAndMakeVisible(title);
			title.textCID = ColourID::Txt;
			title.font = getFontDosisRegular();
			title.mode = Label::Mode::TextToLabelBounds;

			addAndMakeVisible(master);
			master.textCID = title.textCID;
			master.font = title.font;
			master.mode = title.mode;

			//addAndMakeVisible(selection);
			selection.textCID = title.textCID;
			selection.font = title.font;
			selection.mode = title.mode;

			addAndMakeVisible(xen);
			//utils.getParam(PID::Xen)->setLocked(true);

			addAndMakeVisible(baseNote);
			makeParameter(baseNote, PID::BaseNote, "Base Note");
			//utils.getParam(PID::BaseNote)->setLocked(true);

			addAndMakeVisible(masterTune);
			makeParameter(masterTune, PID::MasterTune, "Master Tune");
			//utils.getParam(PID::MasterTune)->setLocked(true);

#if PPDMIDINumVoices != 0
			addAndMakeVisible(pitchbend);
			makeParameter(pitchbend, PID::PitchbendRange, "Pitchbend");
			utils.getParam(param::toPID(pitchbend.getInfo(0)))->setLocked(true);
#endif
			
			addAndMakeVisible(keys);
			
			keys.onDown = [&synth = utils.audioProcessor.tuningEditorSynth](int pitch)
			{
				synth.pitch.store(static_cast<float>(pitch));
				synth.noteOn.store(true);
			};

			keys.onDrag = [&synth = utils.audioProcessor.tuningEditorSynth](int pitch)
			{
				synth.pitch.store(static_cast<float>(pitch));
			};

			keys.onUp = [&synth = utils.audioProcessor.tuningEditorSynth](int)
			{
				synth.noteOn.store(false);
			};

			addAndMakeVisible(gain);
			makePseudoParameter
			(
				gain,
				"Gain",
				"Adjust the tuning editor's output level",
				&utils.audioProcessor.tuningEditorSynth.gain
			);

			layout.init
			(
				{ 1, 13, 5, 5, 5, 3, 1 },
				{ 1, 3, 1, 3, 5, 5, 1, 3, 5, 1, 3, 1 }
			);
		}

		void paint(Graphics& g)
		{
			g.fillAll(Colours::c(ColourID::Bg).withAlpha(.98f));

			//g.setColour(Colours::c(ColourID::Hover));
			//layout.label(g, "temperament", 2, 8, 1, 1, false);
			//layout.label(g, "0.14", 3, 8, 1, 1, false);
		}

		void resized()
		{
			layout.resized();

			layout.place(title, 1, 1, 5, 1, false);
			layout.place(master, 2, 3, 3, 1, false);
			layout.place(selection, 2, 7, 4, 1, false);

			layout.place(xen, 1, 3, 1, 6, true);

			layout.place(baseNote, 2, 4, 1, 2, false);
			layout.place(masterTune, 3, 4, 1, 2, false);
#if PPDMIDINumVoices != 0
			layout.place(pitchbend, 4, 4, 1, 2, false);
#endif
			
			layout.place(keys, 1, 10, 4, 1, false);
			layout.place(gain, 5, 10, 1, 1, false);
		}

	protected:
		Label title, master, selection;
		XenWheel xen;

		Knob baseNote, masterTune;
#if PPDMIDINumVoices != 0
		Knob pitchbend;
#endif

		KeyboardComp keys;
		Knob gain;
	};
}