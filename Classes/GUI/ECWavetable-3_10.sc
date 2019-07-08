/*
 * David Thall
 * ECWavetable.sc
 * CG2 Project
 */

// Allows the user to edit a wavetable stored in RAM in real-time

// Update: Need args for ControlSets for MappableSliders! (hardcoded for now)
// Update: make explicit methods for the rangeSliderRoutine and synthTask!!!
// Update: I may need to make actionLists getters in ZoomSlider classes to
// allow for adding actions to the sliders (e.g., updating the freq & phase of
// the table)
// Update: Put synthdef.load into initClass, and then check each time to see
// if it's loaded!!!
// Update: Need to automate conversion between freq & duration specs!
// Add: argPenColor (sets strokeColor & fillColor)
// Add: Direction button (forward/reverse)
// Add: editing/processing buttons (clear, reverse, invert, smooth, sharpen...)
// Add: method test (use a simple SinOsc SynthDef)
// Add: maintain an internal Buffer instance and SynthDef to play it w/ a Task
// Add: vectorPointsPerSegment
// *** Generalize: Add popUpMenu's for Envelope & LFO Shapes!!!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ECWavetable {
	classvar	<maxTableSize = 1024;

	// state variables
	var	<parent = nil;		// ref to a parent SCWindow
	var	<name = nil;			// SCStaticText to display ???
	var	<color = nil;			// blended background color
	var	<server = nil;		// ref to a running Server instance
	var	<tableSize = nil;		// wavetable buffer size (w/in parent window)

	// SCView's
	var	<multiSlider = nil;	// SCMultiSliderView reference
	var	<centerLabel = nil;	// SCStaticText reference (center divider)
	var	<rangeSlider = nil;	// SCRangeSlider reference (playback pointer)
	var	<freqMappableSlider = nil;	// MappableSlider reference (freq/dur)
	var	<phaseMappableSlider = nil;	// MappableSlider reference (phase)
	var	<dutyCycleMappableSlider = nil;	// MappableSlider reference (duty cycle)
	var	<freqButton = nil;		// SCButton reference (freq/dur)

	// .action function lists
	var	multiSliderActionList = nil;	// multiSlider List of actions
	var	freqButtonActionList = nil;		// freqButton List of actions

	// Synth
	var	<buffer = nil;
	var	<synthDef = nil;
	var	<synth = nil;
	var	<hasChanged = nil;

	// Tasks
	var	wavetableUpdateTask = nil;
	var	rangeSliderTask = nil;

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new ECWavetable
	*new { arg argParent, argName,
			argColor, argServer, argTableSize = 512;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		if(argServer.isNil, { // what if it isn't running ???
			(this.asString ++ ": " ++
					"A Server must be provided!").postln;
			^nil;
		});
		^super.new.initECWavetable(argParent, argName,
				argColor, argServer, argTableSize);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ECWavetable instance into a consistent state
	initECWavetable { arg argParent, argName,
			argColor, argServer, argTableSize;
		this.initECWavetableStateVariables(argParent, argName,
				argColor, argServer, argTableSize);
		this.createECWavetableGUIViews;
		this.addECWavetableActions;
		this.initECWavetableSynth;
		this.runWavetableUpdateTask(0.1);
		this.runRangeSliderTask(10);
	}

	// initialize all the internal state variables
	initECWavetableStateVariables { arg argParent, argName,
			argColor, argServer, argTableSize;
		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });
		color = if(argColor.isNil, { Color.new; }, { argColor; });
		server = argServer;
		tableSize = (argTableSize.asInteger.nextPowerOfTwo).min(maxTableSize);

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("color -> " ++ color).postln;
		("server -> " ++ server).postln;
		("tableSize -> " ++ tableSize).postln;
	}

	// create and initialize the internal gui views
	createECWavetableGUIViews {
		// defaults
		var	multiSliderCanFocus = nil;
		var	multiSliderBackground = nil;
		var	multiSliderStrokeColor = nil;	// color of knob sides (edges)
		var	multiSliderFillColor = nil;		// color of knob (inside)
		var	multiSliderIsFilled = nil;		// is the base below the knob filled?
		var	multiSliderThumbSize = nil;		// ???
		var	multiSliderValueThumbSize = nil;	// h/v extent of knob toward base
		var	multiSliderIndexThumbSize = nil;	// h/v width of an indexed knob
		var	multiSliderIndexIsHorizontal = nil; // can flip layout by 90 degrees
		var	multiSliderShowIndex = nil;		// draws a box at current index
		var	multiSliderElasticMode = nil;	// ???
		var	multiSliderGap = nil;			// pixel spacing between knobs
		var	multiSliderDrawLines = nil;		// interpolate lines between points (strokeColor)
		var	multiSliderDrawRects = nil;		// draw rectangles (thumbs) at nodes
		var	multiSliderSelectionSize = nil;	// default selection size (shift-controlled)
		var	multiSliderReadOnly = nil;		// returns the index and value (aka editable_)
		var	centerLabelBackground = nil;	// center divider background color
		var	rangeSliderCanFocus = nil;
		var	rangeSliderBackground = nil;
		var	rangeSliderKnobColor = nil;
		var	buttonCanFocus = nil;
		var	buttonFont = nil;

		var	freqButtonDataList = nil;

		var	freqButtonStates = nil;

		// var multiSliderValue, multiSliderCurrentValue, multiSliderStep, multiSliderIndex

		multiSliderCanFocus = false;
		multiSliderBackground = color;
		multiSliderStrokeColor = Color.black;
		multiSliderFillColor = Color.black;
		multiSliderIsFilled = false;
		multiSliderThumbSize = 1; 		// ???
		multiSliderValueThumbSize = 1;
		multiSliderIndexThumbSize = 1;
		multiSliderIndexIsHorizontal = true;
		multiSliderShowIndex = false;
		multiSliderElasticMode = 2;
		multiSliderGap = 0;
		multiSliderDrawLines = true;
		multiSliderDrawRects = true;	// makes line look thicker if thumbSize == 1
		multiSliderSelectionSize = 1;
		multiSliderReadOnly = false;
		centerLabelBackground = Color.black.alpha_(0.7);
		rangeSliderCanFocus = false;
		rangeSliderBackground = Color.clear;
		rangeSliderKnobColor = color;
		buttonCanFocus = false;
		buttonFont = Font.new("Helvetica", 10);

		freqButtonDataList = [["freq", "dur"],
				[Color.black, Color.blue], Color.clear];

		freqButtonStates = Array.fill(2, { arg i;
				[freqButtonDataList[0][i],
				freqButtonDataList[1][i],
				freqButtonDataList[2]]; });

		// multiSlider
		multiSlider = MultiSliderView.new(parent);
		multiSlider.canFocus_(multiSliderCanFocus);
		multiSlider.background_(multiSliderBackground);
		multiSlider.strokeColor_(multiSliderStrokeColor);
		multiSlider.fillColor_(multiSliderFillColor);
		multiSlider.isFilled_(multiSliderIsFilled);
		multiSlider.thumbSize_(multiSliderThumbSize);
		multiSlider.valueThumbSize_(multiSliderValueThumbSize);
		multiSlider.indexThumbSize_(multiSliderIndexThumbSize);
		multiSlider.indexIsHorizontal_(multiSliderIndexIsHorizontal);
		multiSlider.showIndex_(multiSliderShowIndex);
		multiSlider.elasticMode_(multiSliderElasticMode);
		multiSlider.gap_(multiSliderGap);
		multiSlider.drawLines_(multiSliderDrawLines);
		multiSlider.drawRects_(multiSliderDrawRects);
		multiSlider.selectionSize_(multiSliderSelectionSize);
		multiSlider.readOnly_(multiSliderReadOnly);
		multiSlider.value_(Array.fill(tableSize, 0.5)); // set all to 0!!!
		multiSlider.step_(0); // infinite precision
		multiSlider.action_({ arg multiSlider;
			multiSliderActionList.do({ arg action;
				action.value(multiSlider); // evaluate
			});
		});

		("multiSlider -> " ++ multiSlider).postln;
		("multiSlider.canFocus -> " ++ multiSlider.canFocus).postln;
		("multiSlider.background -> " ++ multiSlider.background).postln;
		//("multiSlider.strokeColor -> " ++ multiSlider.strokeColor).postln;
		//("multiSlider.fillColor -> " ++ multiSlider.fillColor).postln;
		//("multiSlider.isFilled -> " ++ multiSlider.isFilled).postln;
		//("multiSlider.thumbSize -> " ++ multiSlider.thumbSize).postln;
		//("multiSlider.valueThumbSize -> " ++ multiSlider.valueThumbSize).postln;
		//("multiSlider.indexThumbSize -> " ++ multiSlider.indexThumbSize).postln;
		//("multiSlider.indexIsHorizontal -> " ++ multiSlider.indexIsHorizontal).postln;
		//("multiSlider.showIndex -> " ++ multiSlider.showIndex).postln;
		//("multiSlider.elasticMode -> " ++ multiSlider.elasticMode).postln;
		//("multiSlider.gap -> " ++ multiSlider.gap).postln;
		//("multiSlider.drawLines -> " ++ multiSlider.drawLines).postln;
		//("multiSlider.drawRects -> " ++ multiSlider.drawRects).postln;
		//("multiSlider.selectionSize -> " ++ multiSlider.selectionSize).postln;
		//("multiSlider.readOnly -> " ++ multiSlider.readOnly).postln;
		("multiSlider.step -> " ++ multiSlider.step).postln;
		("multiSlider.action -> " ++ multiSlider.action).postln;

		// centerLabel
		centerLabel = StaticText.new(parent);
		centerLabel.background_(centerLabelBackground);

		("centerLabel -> " ++ centerLabel).postln;
		("centerLabel.background -> " ++ centerLabel.background).postln;

		// rangeSlider
		rangeSlider = RangeSlider.new(parent);
		rangeSlider.canFocus_(rangeSliderCanFocus);
		rangeSlider.background_(rangeSliderBackground);
		rangeSlider.knobColor_(rangeSliderKnobColor);
		rangeSlider.lo_(0);
		rangeSlider.hi_(tableSize.reciprocal); // ???
		rangeSlider.range_(tableSize.reciprocal); // ???
		//rangeSlider.value; // object does not understand!!!
		rangeSlider.step_(tableSize.reciprocal); // finite precision (???????)
		/*
		rangeSlider.action_({ arg rangeSlider;
			rangeSliderActionList.do({ arg action;
				action.value(rangeSlider); // evaluate
			});
		});
		*/

		("rangeSlider -> " ++ rangeSlider).postln;
		("rangeSlider.knobColor -> " ++ rangeSlider.knobColor).postln;
		("rangeSlider.lo -> " ++ rangeSlider.lo).postln;
		("rangeSlider.hi -> " ++ rangeSlider.hi).postln;
		("rangeSlider.range -> " ++ rangeSlider.range).postln;
		("rangeSlider.step -> " ++ rangeSlider.step).postln;

		// freqMappableSlider
		freqMappableSlider = MappableSlider.new(parent,
				Array.fill(2, { ControlSet.new }), ControlSet.new);
		freqMappableSlider.setZoomSpec("frequency",
				-2, 2, -2, 2, \lin, 0.001, 0.1, "Hz ");

		// phaseMappableSlider
		phaseMappableSlider = MappableSlider.new(parent,
				Array.fill(2, { ControlSet.new }), ControlSet.new);
		phaseMappableSlider.setZoomSpec("phase",
				0, 100, 0, 100, \lin, 0.01, 0, "% ");

		// dutyCycleMappableSlider
		dutyCycleMappableSlider = MappableSlider.new(parent,
				Array.fill(2, { ControlSet.new }), ControlSet.new);
		dutyCycleMappableSlider.setZoomSpec("duty cycle",
				0, 100, 0, 100, \lin, 0.01, 0, "% ");

		// freqButton
		freqButton = Button.new(parent);
		freqButton.canFocus_(buttonCanFocus);
		freqButton.font_(buttonFont);
		freqButton.states_(freqButtonStates);
		freqButton.value_(0);
		freqButton.action_({ arg freqButton;
			freqButtonActionList.do({ arg action;
				action.value(freqButton); // evaluate
			});
		});

		("freqButton -> " ++ freqButton).postln;
		("freqButton.canFocus -> " ++ freqButton.canFocus).postln;
		("freqButton.font -> " ++ freqButton.font).postln;
		("freqButton.states -> " ++ freqButton.states).postln;
		("freqButton.value -> " ++ freqButton.value).postln;
		("freqButton.action -> " ++ freqButton.action).postln;
	}

	// define & register the action event functions...add them to actionList's
	addECWavetableActions {
		// action functions to append to action list
		var	multiSliderAction = nil;
		var	freqButtonAction = nil;

		// define actions
		multiSliderAction = { arg multiSlider;
			hasChanged = true;
		};

		freqButtonAction = { arg freqButton;
			if(freqButton.value == 0, { // frequency
				freqMappableSlider.setZoomSpec("frequency",
						-2, 2, -2, 2, \lin, 0.001, 0.1, "Hz ");
			});
			if(freqButton.value == 1, { // duration
				freqMappableSlider.setZoomSpec("duration",
						0.1, 100, 0.1, 100, \exp, 0.1, 10, "sec ");
			});
		};

		// create new actionLists
		multiSliderActionList = List.new;
		freqButtonActionList = List.new;

		// add actions to actionLists
		multiSliderActionList.add(multiSliderAction);
		freqButtonActionList.add(freqButtonAction);

		("multiSliderActionList -> " ++ multiSliderActionList).postln;
		("freqButtonActionList -> " ++ freqButtonActionList).postln;
	}

	initECWavetableSynth {
		buffer = Buffer.alloc(server, tableSize, 1); // completion message ???
		//buffer.cheby(1.0, true, true, true); // set wavetable to true if interpolating...

		/*
		synthDef = SynthDef.new("ECWavetable_1", { arg out = 0,
				bufnum = 0, freq = 440;
			var	secsPerCycle;
			var	phasor, bufferOsc;
			secsPerCycle = tableSize * SampleRate.ir.reciprocal;
			phasor = Phasor.ar(0,
					freq * secsPerCycle * BufRateScale.kr(bufnum),
					0,
					BufFrames.kr(bufnum),
					0);
			bufferOsc = BufRd.ar(1, bufnum, phasor, 1, 1);
			Out.ar(out, bufferOsc);
		}).load(server);
		*/

		/*
		synthDef = SynthDef.new("ECWavetable_1", { arg out = 0,
				bufnum = 0, freq = 440;
			var	osc;
			osc = Osc.ar(bufnum, freq, 0);
			Out.ar(out, osc);
		}).load(server);
		*/

		synthDef = SynthDef.new("ECWavetable_1", { arg out = 0,
				bufnum = 0, freq = 0.1;
			var	wavetableOsc, sinOsc;
			wavetableOsc = OscN.kr(bufnum, freq, 0);
			sinOsc = SinOsc.ar(440 * (2 ** wavetableOsc), 0, 0.2);
			Out.ar(out, sinOsc * 0); // SCALED BY 0 FOR DEMO!!!!!!!
		}).load(server);

		synth = Synth.new("ECWavetable_1",
				[\out, 0, \bufnum, buffer.bufnum, \freq, 0.1], nil, \addToHead);

		hasChanged = false;

		("buffer -> " ++ buffer).postln;
		("synthDef -> " ++ synthDef).postln;
		("synth -> " ++ synth).postln;
		("hasChanged -> " ++ hasChanged).postln;
	}

	runWavetableUpdateTask { arg argDelta = 0.1;
		wavetableUpdateTask = Task.new({
			inf.do({
				if(hasChanged, {
					{ buffer.setn(0, (multiSlider.value - 0.5) * 2); }.defer;
					hasChanged = false;
				});
				argDelta.wait;
			});
		}, SystemClock);
		wavetableUpdateTask.start;
	}

	runRangeSliderTask { arg argMaxStepsPerSec = 10;
		var	currentFreq = nil;
		var	duration;
		var	maxStepsPerCycle = nil;
		var	range = nil;
		var	timesToMove = nil;
		var	increment = nil;
		var	delta = nil;
		currentFreq = freqMappableSlider.currentBox.value; // should never == '0'!!!
		duration = currentFreq.reciprocal;
		maxStepsPerCycle = ((argMaxStepsPerSec * duration).asInteger).max(1);
		range = tableSize;
		timesToMove = (
			if(range < maxStepsPerCycle, {
				range;
			}, {
				maxStepsPerCycle;
			});
		).max(1);
		increment = range / timesToMove;
		delta = duration / timesToMove;


		("currentFreq -> " ++ currentFreq).postln;
		("duration -> " ++ duration).postln;
		("maxStepsPerCycle -> " ++ maxStepsPerCycle).postln;
		("range -> " ++ range).postln;
		("timesToMove -> " ++ timesToMove).postln;
		("increment -> " ++ increment).postln;
		("delta -> " ++ delta).postln;

		rangeSliderTask = Task.new({ // JKilg: this is causing runtime error when closing GUI window
			var	pos = 0;
			inf.do({

				timesToMove.do({
					{
						rangeSlider.lo_(rangeSlider.hi - rangeSlider.step);
						rangeSlider.hi_(pos.round(rangeSlider.step) * range.reciprocal);
					}.defer; // JKilg: this seems to cause an error
					//("pos -> " ++ pos).postln;
					pos = (pos + increment) % tableSize;
					delta.wait;

			});
			});
		}, AppClock); //JKilg: changed to AppClock, since calling GUI primitives
		rangeSliderTask.start;
		("rangeSliderTask -> " ++ rangeSliderTask).postln;
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
			sliderWidth = 20, gap = 5,
			buttonLength = 40,
			rangeSliderSide = \top,
			slidersSide = \bottom,
			elastic = true;
		var	parentWidth = nil;
		var	parentHeight = nil;

		var	multiSliderWidth = nil;
		var	multiSliderHeight = nil;

		var	centerLabelWidth = nil;

		// get the parent SCWindow's width & height
		parentWidth = parent.bounds.width;
		parentHeight = parent.bounds.height;

		("parentWidth -> " ++ parentWidth).postln;
		("parentHeight -> " ++ parentHeight).postln;

		// calculate the width & height for the SCMultiSliderView
		multiSliderWidth = parentWidth - (xOff * 2) - gap;
		multiSliderHeight = parentHeight - (yOff * 2) - gap;

		("multiSliderWidth -> " ++ multiSliderWidth).postln;
		("multiSliderHeight -> " ++ multiSliderHeight).postln;

		centerLabelWidth = 1;
		("centerLabelWidth -> " ++ centerLabelWidth).postln;

		multiSlider.bounds_(Rect.new(xOff,
				yOff, tableSize, 160));
		centerLabel.bounds_(Rect.new(xOff,
				yOff + (160/2), tableSize, centerLabelWidth));
		rangeSlider.bounds_(Rect.new(xOff,
				0, tableSize, yOff));

		freqMappableSlider.draw(xOff,
				yOff + 160 + gap,
				sliderWidth, gap,
				265, 60, 20,
				\right, \left);
		phaseMappableSlider.draw(xOff,
				yOff + 160 + (gap * 2) + sliderWidth,
				sliderWidth, gap,
				265, 60, 20,
				\right, \left);
		dutyCycleMappableSlider.draw(xOff,
				yOff + 160 + (gap * 3) + (sliderWidth * 2),
				sliderWidth, gap,
				265, 60, 20,
				\right, \left);

		freqButton.bounds_(Rect.new(xOff + 515,
				yOff + 160 + gap, buttonLength, sliderWidth));

		if(elastic, {
			multiSlider.resize_(5); 	// h-elastic, v-elastic
			centerLabel.resize_(2);		// h-elastic, fixed to top
			rangeSlider.resize_(2);		// h-elastic, fixed to top

			freqMappableSlider.sliderLabel.resize_(8);
			freqMappableSlider.slider.resize_(8);
			freqMappableSlider.leftBoxLabel.resize_(7);
			freqMappableSlider.leftBox.resize_(7);
			freqMappableSlider.rightBoxLabel.resize_(9);
			freqMappableSlider.rightBox.resize_(9);
			freqMappableSlider.currentBoxLabel.resize_(9);
			freqMappableSlider.currentBox.resize_(9);
			freqMappableSlider.lockButton.resize_(7);
			freqMappableSlider.axisButton.resize_(7);
		});
	}
}