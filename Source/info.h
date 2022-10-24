/*

-------------------------------------------------------------------

Welcome to DEFAULT PROJECT!

This project contains my vision of a default JUCE VST plugin project. I made this so I can get started trying new ideas
and finishing projects way faster than from a projucer-made template.

-------------------------------------------------------------------

HOW TO USE:

1. Copy the folder "Source" as well as the file "Project.jucer" into a new folder.
2. In Project.jucer
	2.1 Rename plugin name
	2.2 Define unique plugin ID
	2.3 Configurate the pre processor definitions
3. Code DSP in processBlockCustom
4. Code GUI in lowlevel.cpp/h
5. Ship it!

------------------------------------------------------------------- 

TO DO:

5. logic crashes on move knot
7. y-achsis eq pad for gain. mousewheel for q. slope as extra setting
24. tooltips in high level component and new design for less space
32: A/B to flip arrows to make more obvious that it's for macro
33: preset formulas for formula parser for the math noobs
34: dice button looks not dice-y enough
35: when snap-to-pitch is activated should be reflected in gui
37: if enter value manually and exceeds range, textbox should show corrected value
39: rand relative for all parameters at once
40. rightclick on knob when menu already open closes menu

saving patch does not repaint patch browser button
loading patches doesn't work

manta 2
	am/rm between lanes

modulation of the filter
	TPT/SVF filters work better?
	randomizer doesn't repaint transfer curve correctly

randomizer dangerous - make locks more obvious
	not obvious enough yet for buttons
	sometimes hard to click on small knobs
	macro needs to be lockable too

WaveTableDisplay
	spectral mode doesn't make sense yet

Knob Looks
	make free funcs for knob vs slider behaviour
		for being called in makeParameter's Knob::Looks switch

Reduce RAM-Usage
	Knob & Button
		make sort of a lookAndFeel thing, so that onPaints are not copied n times

WaveTable
	implement spline editor

filterResponseGraph
	in decibels
		then with automatic range
			like [-3, 3]db or [-12, 12]db

tuningeditor
	button to play chromatic scale for preview

Colours
	figure out natural hue steps
	if chosen white modulations not visible anymore
	some dark colours make it too dark
	when changing colours
		default cursor needs update

highlevel
	"remove dc offset" switch (steep highpass)
		FIR or IIR? which freq? how steep?

when saving a modpatch the preset browser needs to update its list too

Pan Knob
	when value near 0 draw "C" and not "0" or "-0"

SplineEditor
	Sometimes selection range shows wrong range
		and that feels bad
	Add Feature: Non-removable/Fixed Points
	Snap to Grid
		sometimes makes points on draggable
	make points come from outside architecturally (maybe?)
	wavetable from spline

ScrollBar
	some things still not handled by scrollbarcomp alone
	enable scrolling not only on scrollbar hover

PatchBrowser
	revisit tag system

TextEditor
	multiline text
		where does click put tick?

sta::State
	rewrite with different types of state:
		per instance
		per plugin id
		per developer
	implement undo/redo

MIDI Learn
	1 cc to n parameters?
		(especially if multiple parameters are controlled by one knob/button)
	different midi channnels (16 * 128 CCs instead of 128)
	possible to save and load default state of whole midi learn patch
	ContextMenu
		implement context menu(s) for
			ccMonitor

Label
	make free func(s) for grouping labels (n buttons)

Options Menu
	how to make automatic updates

All Params
	saving and loading max mod depths as preset

Oversampler
	make less cpu demanding (FFT?)

FirstTimeAction(s)
	thing that only makes lookuptables when the plugin is started the first time
	tutorials

HighLevel UI Elements
	undo/redo buttons
	sidechain activated?

-------------------------------------------------------------------

FEATURE REQUESTS:

show total time spent with mrugalla plugins

Achievements System?
	discuss relevance, pros and cons

*/