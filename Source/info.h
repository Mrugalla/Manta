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

Frequency StringToValue
	enter note values like C2, D#1, Eb3
	and more specific ones like C3-edo7-root42-tune432

GainIn GainOut Meters
	crash -nan if too much gain

Colours
	if chosen white modulations not visible anymore
	dark colours make it too dark

highlevel
	"remove dc offset" switch (steep highpass)
		FIR or IIR? which freq? how steep?

when saving a modpatch the preset browser needs to update its list too

Pan Knob
	when value near 0 draw "C" and not "0" or "-0"

Knob
	double-clicks don't always work

SplineEditor
	Sometimes selection range shows wrong range
		and that feels bad
	Add Feature: Non-removable Points
	Snap to Grid
		sometimes makes points on draggable
	make points come from outside architecturally (maybe?)
	wavetable from spline

Macro Dropdown
	Save All MaxModDepths and Bias-Patch
	Load Some MaxModDepths and Bias-Patch (FileChooser?)
	Switch: Control MaxModDepth or Bias
	Switch: Rel/Abs MaxModDepth
	Flip Parameter Value with MaxModDepth
	Remove All MaxModDepth and Bias

Colours
	figure out natural hue steps

ScrollBar
	some things still not handled by scrollbarcomp alone
	enable scrolling not only on scrollbar hover

PatchBrowser
	revisit tag system

FormularParser
	is log and ln working right?
	make more lightweight
		values push_backs in calculate and calculateX needed?
		make callbacks instead of buffers?

TextEditor
	minimize > back > click > tick doesn't move > 2nd click > does move
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
	different midi channnels (16 * 128 CCs instead of 128)
	possible to save and load default state of whole midi learn patch

ContextMenu
	implement context menu(s) for
		ccMonitor

Label
	make free func(s) for grouping labels (n buttons)

Options Menu
	think about how to make automatic tutorials or manuals
	how to make automatic updates possible

All Params
	saving and loading max mod depths as preset

Param
	...

Oversampler
	make less cpu demanding (FFT?)

FirstTimeAction(s)
	thing that only makes lookuptables when the plugin is started the first time
	tutorial

Pre Processor Defines (PPD)
	PPDSidechainable

HighLevel UI Elements
	undo/redo buttons
	sidechain activated?

-------------------------------------------------------------------

FEATURE REQUESTS:

show total time spent with mrugalla plugins

Achievements System?
	discuss relevance, pros and cons

*/