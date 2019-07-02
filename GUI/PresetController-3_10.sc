/*
 * David Thall
 * PresetsController.sc
 * CG2 Project
 */

// A PresetsController contains an array of PresetButtons and a PresetInterpolator.
// It maintains a collection of Presets, and allows the user to interpolate between
// them in a specified interpolation time.  It overrides the action function defined
// in PresetButton, in order to create the proper user interface needed for
// interpolation control.  It's gui contains a group of numbered buttons
// (provided externally by a PresetButton array), 3 radio-style ctrl buttons
// (pause/stop/replay), 3 radio-style mode buttons (reset/load/store), 3 radio-style
// curve buttons (lin/exp/log), a status label field, a timeBox for setting the
// interpolation time constant, and a rangeslider showing the current interpolation
// time.  An instance of PresetsController is a control container for its children
// views, independent of their data, but dependent on their states.

// When clicked, it load's the current config stored in its instance of Preset
// When clicked (w/ Command Key), it store's the current state of the gui widgets
// (sliders & buttons) to its instance of Preset
// When clicked (w/ Option Key), it will reload the default config
// in its instance of Preset

// Fix: The timeRangeSlider doesn't seem to show the lowest value during
// interpolation (off by 1 error in defer?)
// Fix: When 'reset' or 'load' is selected and an empty preset is clicked
// with a time > 0, the morph function gets a targetPresetNum that doesn't exist
// Fix: The pauseButtonFlashRoutine should set the button's color's back to
// their defaults when 'stop' is called!
// Update: allow timeZoomSlider.currentBox to dynamically update
// morph function when changed...
// Update: Clean up routine & task functions!
// Add: Methods for the rangeSlider progressBar (fill, clear etc...)
// Add: Instance variable that holds copy of PresetInterpolotor's
// maxStepsPerSec field! (dynamically update this depending on CPU usage?)
// Add: option to draw rangeSlider vertically

// 79 chars
///////////////////////////////////////////////////////////////////////////////

PresetsController {
	// state variables
	var	<parent = nil;			// ref to a parent SCWindow
	var	<name = nil;				// SCStaticText to display in ???
	var	<currentModeButtonSelected = nil;
	var	<currentCurveButtonSelected = nil;
	// SCView's
	var	<presetButtonArray = nil;	// PresetButton Array (refs)
	var	<presetInterpolator = nil;	// PresetInterpolator reference

	var	<pauseButton = nil; 		// presetInterpolator ctrl
	var	<stopButton = nil;
	var	<refreshButton = nil;

	var	<resetButton = nil;		// presetButton mode ('radio'-style)
	var	<loadButton = nil;
	var	<storeButton = nil;

	var	<linButton = nil;			// presetInterpolator curve ('radio'-style)
	var	<expButton = nil;
	var	<logButton = nil;

	var	<statusLabel = nil; 		// SCStaticText reference
	var	<timeZoomSlider = nil; 		// ZoomSlider reference
	var	<timeRangeSliderLabel = nil;// SCStaticText reference
	var	<timeRangeSlider = nil;		// SCRangeSlider reference

	// .action function lists
	var	pauseButtonActionList = nil;// pauseButton List of actions
	var	stopButtonActionList = nil;	// stopButton List of actions
	var	refreshButtonActionList = nil;// refreshButton List of actions
	var	resetButtonActionList = nil;// resetButton List of actions
	var	loadButtonActionList = nil;	// loadButton List of actions
	var	storeButtonActionList = nil;// storeButton List of actions
	var	linButtonActionList = nil;// linButton List of actions
	var	expButtonActionList = nil;	// expButton List of actions
	var	logButtonActionList = nil;// logButton List of actions

	var	timeRangeSliderActionList = nil;// timeRangeSlider List of actions

	// Tasks
	var	<timeRangeSliderTask = nil;
	var	<pauseButtonFlashRoutine = nil;

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new PresetsController
	*new { arg argParent, argName,
			argPresetButtonArray, argPresetInterpolator;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		if(argPresetButtonArray.isNil, {
			(this.asString ++ ": " ++
					"A PresetButton Array must be provided!").postln;
			^nil;
		});
		if(argPresetInterpolator.isNil, {
			(this.asString ++ ": " ++
					"A PresetInterpolator must be provided!").postln;
			^nil;
		});
		^super.new.initPresetsController(argParent, argName,
				argPresetButtonArray, argPresetInterpolator);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the PresetsController instance into a consistent state
	initPresetsController { arg argParent, argName,
			argPresetButtonArray, argPresetInterpolator;

		this.initPresetsControllerStateVariables(argParent, argName,
				argPresetButtonArray, argPresetInterpolator);
		this.createPresetsControllerGUIViews;
				this.createPresetsControllerTimeZoomSlider;
		this.addPresetsControllerActions; // register actions
				this.overridePresetButtonActions;
	}

	// initialize all the internal state variables
	initPresetsControllerStateVariables { arg argParent, argName,
			argPresetButtonArray, argPresetInterpolator;
		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });
		presetButtonArray = argPresetButtonArray;
		presetInterpolator = argPresetInterpolator;

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("presetButtonArray -> " ++ presetButtonArray).postln;
		("presetInterpolator -> " ++ presetInterpolator).postln;
	}

	// create and initialize the internal gui views
	createPresetsControllerGUIViews {
		// defaults
		var	buttonCanFocus = nil;
		var	buttonFont = nil;

		var	pauseButtonDataList = nil;
		var	stopButtonDataList = nil;
		var	refreshButtonDataList = nil;
		var	resetButtonDataList = nil;
		var	loadButtonDataList = nil;
		var	storeButtonDataList = nil;
		var	linButtonDataList = nil;
		var	expButtonDataList = nil;
		var	logButtonDataList = nil;

		var	pauseButtonStates = nil;
		var	stopButtonStates = nil;
		var	refreshButtonStates = nil;
		var	resetButtonStates = nil;
		var	loadButtonStates = nil;
		var	storeButtonStates = nil;
		var	linButtonStates = nil;
		var	expButtonStates = nil;
		var	logButtonStates = nil;

		var	statusLabelFont = nil;
		var	statusLabelAlign = nil;
		var	statusLabelBackground = nil;
		var	statusLabelStringColor = nil;
		var	timeRangeSliderLabelFont = nil;
		var	timeRangeSliderLabelAlign = nil;
		var	timeRangeSliderLabelStringColor = nil;
		var	timeRangeSliderCanFocus = nil;
		var	timeRangeSliderBackground = nil;
		var	timeRangeSliderKnobColor = nil;

		buttonCanFocus = false;
		buttonFont = Font.new("Helvetica", 10);

		pauseButtonDataList = [["||", Color.black, Color.clear]];
		stopButtonDataList = [["S", Color.black, Color.clear]];
		refreshButtonDataList = [["R", Color.black, Color.clear]];
		resetButtonDataList = ["reset",
				[Color.black, Color.blue], Color.clear];
		loadButtonDataList = ["load",
				[Color.black, Color.blue], Color.clear];
		storeButtonDataList = ["store",
				[Color.black, Color.blue], Color.clear];
		linButtonDataList = ["lin",
				[Color.black, Color.blue], Color.clear];
		expButtonDataList = ["exp",
				[Color.black, Color.blue], Color.clear];
		logButtonDataList = ["log",
				[Color.black, Color.blue], Color.clear];

		pauseButtonStates = pauseButtonDataList;
		stopButtonStates = stopButtonDataList;
		refreshButtonStates = refreshButtonDataList;
		resetButtonStates = Array.fill(2, { arg i;
					[resetButtonDataList[0],
					resetButtonDataList[1][i],
					resetButtonDataList[2]]; });
		loadButtonStates = Array.fill(2, { arg i;
					[loadButtonDataList[0],
					loadButtonDataList[1][i],
					loadButtonDataList[2]]; });
		storeButtonStates = Array.fill(2, { arg i;
					[storeButtonDataList[0],
					storeButtonDataList[1][i],
					storeButtonDataList[2]]; });
		linButtonStates = Array.fill(2, { arg i;
					[linButtonDataList[0],
					linButtonDataList[1][i],
					linButtonDataList[2]]; });
		expButtonStates = Array.fill(2, { arg i;
					[expButtonDataList[0],
					expButtonDataList[1][i],
					expButtonDataList[2]]; });
		logButtonStates = Array.fill(2, { arg i;
					[logButtonDataList[0],
					logButtonDataList[1][i],
					logButtonDataList[2]]; });

		statusLabelFont = Font.new("Helvetica", 12);
		statusLabelAlign = \center;
		statusLabelBackground = Color.black;
		statusLabelStringColor = Color.white;
		timeRangeSliderLabelFont = Font.new("Helvetica", 10);
		timeRangeSliderLabelAlign = \center;
		timeRangeSliderLabelStringColor = Color.clear;
		timeRangeSliderCanFocus = false;
		timeRangeSliderBackground = Color.black;
		timeRangeSliderKnobColor = Color.blue;

		// pauseButton
		pauseButton = Button.new(parent);
		pauseButton.canFocus_(buttonCanFocus);
		pauseButton.font_(buttonFont);
		pauseButton.states_(pauseButtonStates);
		pauseButton.value_(0);
		pauseButton.action_({ arg pauseButton;
			pauseButtonActionList.do({ arg action;
				action.value(pauseButton); // evaluate
			});
		});

		("pauseButton -> " ++ pauseButton).postln;
		("pauseButton.canFocus -> " ++ pauseButton.canFocus).postln;
		("pauseButton.font -> " ++ pauseButton.font).postln;
		("pauseButton.states -> " ++ pauseButton.states).postln;
		("pauseButton.value -> " ++ pauseButton.value).postln;
		("pauseButton.action -> " ++ pauseButton.action).postln;

		// stopButton
		stopButton = Button.new(parent);
		stopButton.canFocus_(buttonCanFocus);
		stopButton.font_(buttonFont);
		stopButton.states_(stopButtonStates);
		stopButton.value_(0);
		stopButton.action_({ arg stopButton;
			stopButtonActionList.do({ arg action;
				action.value(stopButton); // evaluate
			});
		});

		("stopButton -> " ++ stopButton).postln;
		("stopButton.canFocus -> " ++ stopButton.canFocus).postln;
		("stopButton.font -> " ++ stopButton.font).postln;
		("stopButton.states -> " ++ stopButton.states).postln;
		("stopButton.value -> " ++ stopButton.value).postln;
		("stopButton.action -> " ++ stopButton.action).postln;

		// refreshButton
		refreshButton = Button.new(parent);
		refreshButton.canFocus_(buttonCanFocus);
		refreshButton.font_(buttonFont);
		refreshButton.states_(refreshButtonStates);
		refreshButton.value_(0);
		refreshButton.action_({ arg refreshButton;
			refreshButtonActionList.do({ arg action;
				action.value(refreshButton); // evaluate
			});
		});

		("refreshButton -> " ++ refreshButton).postln;
		("refreshButton.canFocus -> " ++ refreshButton.canFocus).postln;
		("refreshButton.font -> " ++ refreshButton.font).postln;
		("refreshButton.states -> " ++ refreshButton.states).postln;
		("refreshButton.value -> " ++ refreshButton.value).postln;
		("refreshButton.action -> " ++ refreshButton.action).postln;

		// resetButton
		resetButton = Button.new(parent);
		resetButton.canFocus_(buttonCanFocus);
		resetButton.font_(buttonFont);
		resetButton.states_(resetButtonStates);
		resetButton.value_(1); // selected...
		resetButton.action_({ arg resetButton;
			resetButtonActionList.do({ arg action;
				action.value(resetButton); // evaluate
			});
		});

		("resetButton -> " ++ resetButton).postln;
		("resetButton.canFocus -> " ++ resetButton.canFocus).postln;
		("resetButton.font -> " ++ resetButton.font).postln;
		("resetButton.states -> " ++ resetButton.states).postln;
		("resetButton.value -> " ++ resetButton.value).postln;
		("resetButton.action -> " ++ resetButton.action).postln;

		// loadButton
		loadButton = Button.new(parent);
		loadButton.canFocus_(buttonCanFocus);
		loadButton.font_(buttonFont);
		loadButton.states_(loadButtonStates);
		loadButton.value_(0);
		loadButton.action_({ arg loadButton;
			loadButtonActionList.do({ arg action;
				action.value(loadButton); // evaluate
			});
		});

		("loadButton -> " ++ loadButton).postln;
		("loadButton.canFocus -> " ++ loadButton.canFocus).postln;
		("loadButton.font -> " ++ loadButton.font).postln;
		("loadButton.states -> " ++ loadButton.states).postln;
		("loadButton.value -> " ++ loadButton.value).postln;
		("loadButton.action -> " ++ loadButton.action).postln;

		// storeButton
		storeButton = Button.new(parent);
		storeButton.canFocus_(buttonCanFocus);
		storeButton.font_(buttonFont);
		storeButton.states_(storeButtonStates);
		storeButton.value_(0);
		storeButton.action_({ arg storeButton;
			storeButtonActionList.do({ arg action;
				action.value(storeButton); // evaluate
			});
		});

		("storeButton -> " ++ storeButton).postln;
		("storeButton.canFocus -> " ++ storeButton.canFocus).postln;
		("storeButton.font -> " ++ storeButton.font).postln;
		("storeButton.states -> " ++ storeButton.states).postln;
		("storeButton.value -> " ++ storeButton.value).postln;
		("storeButton.action -> " ++ storeButton.action).postln;

		// linButton
		linButton = Button.new(parent);
		linButton.canFocus_(buttonCanFocus);
		linButton.font_(buttonFont);
		linButton.states_(linButtonStates);
		linButton.value_(1); // selected...
		linButton.action_({ arg linButton;
			linButtonActionList.do({ arg action;
				action.value(linButton); // evaluate
			});
		});

		("linButton -> " ++ linButton).postln;
		("linButton.canFocus -> " ++ linButton.canFocus).postln;
		("linButton.font -> " ++ linButton.font).postln;
		("linButton.states -> " ++ linButton.states).postln;
		("linButton.value -> " ++ linButton.value).postln;
		("linButton.action -> " ++ linButton.action).postln;

		// expButton
		expButton = Button.new(parent);
		expButton.canFocus_(buttonCanFocus);
		expButton.font_(buttonFont);
		expButton.states_(expButtonStates);
		expButton.value_(0);
		expButton.action_({ arg expButton;
			expButtonActionList.do({ arg action;
				action.value(expButton); // evaluate
			});
		});

		("expButton -> " ++ expButton).postln;
		("expButton.canFocus -> " ++ expButton.canFocus).postln;
		("expButton.font -> " ++ expButton.font).postln;
		("expButton.states -> " ++ expButton.states).postln;
		("expButton.value -> " ++ expButton.value).postln;
		("expButton.action -> " ++ expButton.action).postln;

		// logButton
		logButton = Button.new(parent);
		logButton.canFocus_(buttonCanFocus);
		logButton.font_(buttonFont);
		logButton.states_(logButtonStates);
		logButton.value_(0);
		logButton.action_({ arg logButton;
			logButtonActionList.do({ arg action;
				action.value(logButton); // evaluate
			});
		});

		("logButton -> " ++ logButton).postln;
		("logButton.canFocus -> " ++ logButton.canFocus).postln;
		("logButton.font -> " ++ logButton.font).postln;
		("logButton.states -> " ++ logButton.states).postln;
		("logButton.value -> " ++ logButton.value).postln;
		("logButton.action -> " ++ logButton.action).postln;

		// statusLabel
		statusLabel = StaticText.new(parent);
		statusLabel.string_("");
		statusLabel.font_(statusLabelFont);
		statusLabel.align_(statusLabelAlign);
		statusLabel.background_(statusLabelBackground);
		statusLabel.stringColor_(statusLabelStringColor);

		("statusLabel -> " ++ statusLabel).postln;
		("statusLabel.string -> " ++ statusLabel.string).postln;
		("statusLabel.font -> " ++ statusLabel.font).postln;
		//("statusLabel.align -> " ++ statusLabel.align).postln;
		("statusLabel.stringColor -> " ++ statusLabel.stringColor).postln;

		// timeRangeSliderLabel
		timeRangeSliderLabel = StaticText.new(parent);
		timeRangeSliderLabel.string_("");
		timeRangeSliderLabel.font_(timeRangeSliderLabelFont);
		timeRangeSliderLabel.align_(timeRangeSliderLabelAlign);
		timeRangeSliderLabel.stringColor_(timeRangeSliderLabelStringColor);

		("timeRangeSliderLabel -> " ++ timeRangeSliderLabel).postln;
		("timeRangeSliderLabel.string -> " ++ timeRangeSliderLabel.string).postln;
		("timeRangeSliderLabel.font -> " ++ timeRangeSliderLabel.font).postln;
		//("timeRangeSliderLabel.align -> " ++ timeRangeSliderLabel.align).postln;
		("timeRangeSliderLabel.stringColor -> " ++ timeRangeSliderLabel.stringColor).postln;

		// timeRangeSlider
		timeRangeSlider = RangeSlider.new(parent);
		timeRangeSlider.canFocus_(timeRangeSliderCanFocus);
		timeRangeSlider.background_(timeRangeSliderBackground);
		timeRangeSlider.knobColor_(timeRangeSliderKnobColor);
		timeRangeSlider.lo_(0);
		timeRangeSlider.hi_(1);
		timeRangeSlider.range_(1);
		//timeRangeSlider.value; // object does not understand!!!
		timeRangeSlider.step_(0.001); // finite precision (???????)
		timeRangeSlider.action_({ arg timeRangeSlider;
			timeRangeSliderActionList.do({ arg action;
				action.value(timeRangeSlider); // evaluate
			});
		});

		("timeRangeSlider -> " ++ timeRangeSlider).postln;
		("timeRangeSlider.canFocus -> " ++ timeRangeSlider.canFocus).postln;
		("timeRangeSlider.background -> " ++ timeRangeSlider.background).postln;
		("timeRangeSlider.knobColor -> " ++ timeRangeSlider.knobColor).postln;
		("timeRangeSlider.lo -> " ++ timeRangeSlider.lo).postln;
		("timeRangeSlider.hi -> " ++ timeRangeSlider.hi).postln;
		("timeRangeSlider.range -> " ++ timeRangeSlider.range).postln;
		("timeRangeSlider.step -> " ++ timeRangeSlider.step).postln;
		("timeRangeSlider.action -> " ++ timeRangeSlider.action).postln;

		// set any default states internally... (remove if possible!)
		pauseButton.enabled = false;
		stopButton.enabled = true;
		refreshButton.enabled = false;
		resetButton.value = 1;
		loadButton.value = 0;
		storeButton.value = 0;
		linButton.value = 1;
		expButton.value = 0;
		logButton.value = 0;
		timeRangeSlider.lo = 0;
		timeRangeSlider.hi = 1;
		timeRangeSlider.range = 1;
		timeRangeSlider.step = 0.001;

		currentModeButtonSelected = this.resetButton;
		currentCurveButtonSelected = this.linButton;
	}

	// create and initialize the internal ZoomSlider
	// (combine with gui method!...see ECWavetable)
	createPresetsControllerTimeZoomSlider {
		// defaults
		var	name = nil;
		var	min = nil;
		var	max = nil;
		var	left = nil;
		var	right = nil;
		var	warp = nil;
		var	step = nil;
		var	default = nil;
		var	units = nil;

		name = "fade time";
		min = 0;
		max = 7200; // currently, 2 hours max...
		left = 0;
		right = 60;
		warp = \exp;
		step = 0.1;
		default = 4;
		units = "sec ";

		timeZoomSlider = ZoomSlider.new(parent);
		timeZoomSlider.setZoomSpec(name,
				min, max, left, right, warp, step, default, units);
	}

	// define & register the action event functions...add them to actionList's
	addPresetsControllerActions {
	// action functions to append to action list
		var	pauseButtonAction = nil;
		var	stopButtonAction = nil;
		var	refreshButtonAction = nil;
		var	resetButtonAction = nil;
		var	loadButtonAction = nil;
		var	storeButtonAction = nil;
		var	linButtonAction = nil;
		var	expButtonAction = nil;
		var	logButtonAction = nil;

		var	timeRangeSliderAction = nil;

		// define actions
		pauseButtonAction = { arg pauseButton;
			("BEG ********** " ++ name ++ " " ++ "pauseButton.action **********").postln;
			if(presetInterpolator.isPlaying, {
				presetInterpolator.pause;
				timeRangeSliderTask.pause;
				this.runPauseButtonFlashRoutine(1);
			}, {
				presetInterpolator.resume;
				timeRangeSliderTask.resume;
				pauseButtonFlashRoutine.stop;
			});
			("END ********** " ++ name ++ " " ++ "pauseButton.action **********\n").postln;
		};

		stopButtonAction = {
			("BEG ********** " ++ name ++ " " ++ "stopButton.action **********").postln;
			if(presetInterpolator.isPlaying, {
				presetInterpolator.stop;
				timeRangeSliderTask.stop;
			});

			// if the pauseButtonFlashRoutine is currently playing...
			if(pauseButtonFlashRoutine.state == 7, {
				pauseButtonFlashRoutine.stop;
			});

			timeRangeSlider.lo_(0).hi_(1); // fill timeRangeSlider progressBar
			pauseButton.enabled_(false); // disable the pauseButton
			("END ********** " ++ name ++ " " ++ "stopButton.action **********\n").postln;
		};

		refreshButtonAction = {
			("BEG ********** " ++ name ++ " " ++ "refreshButton.action **********").postln;
			("END ********** " ++ name ++ " " ++ "refreshButton.action **********\n").postln;
		};

		resetButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "resetButton.action **********").postln;
			this.changeModeState(view);
			("END ********** " ++ name ++ " " ++ "resetButton.action **********\n").postln;
		};

		loadButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "loadButton.action **********").postln;
			this.changeModeState(view);
			("END ********** " ++ name ++ " " ++ "loadButton.action **********\n").postln;
		};

		storeButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "storeButton.action **********").postln;
			this.changeModeState(view);
			("END ********** " ++ name ++ " " ++ "storeButton.action **********\n").postln;
		};

		linButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "linButton.action **********").postln;
			this.changeCurveState(view);
			("END ********** " ++ name ++ " " ++ "linButton.action **********\n").postln;
		};

		expButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "expButton.action **********").postln;
			this.changeCurveState(view);
			("END ********** " ++ name ++ " " ++ "expButton.action **********\n").postln;
		};

		logButtonAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "logButton.action **********").postln;
			this.changeCurveState(view);
			("END ********** " ++ name ++ " " ++ "logButton.action **********\n").postln;
		};

		timeRangeSliderAction = { arg view;
			("BEG ********** " ++ name ++ " " ++ "timeRangeSlider.action **********").postln;
			("END ********** " ++ name ++ " " ++ "timeRangeSlider.action **********\n").postln;
		};

		// create new actionLists
		pauseButtonActionList = List.new;
		stopButtonActionList = List.new;
		refreshButtonActionList = List.new;
		resetButtonActionList = List.new;
		loadButtonActionList = List.new;
		storeButtonActionList = List.new;
		linButtonActionList = List.new;
		expButtonActionList = List.new;
		logButtonActionList = List.new;

		// add actions to actionLists
		pauseButtonActionList.add(pauseButtonAction);
		stopButtonActionList.add(stopButtonAction);
		refreshButtonActionList.add(refreshButtonAction);
		resetButtonActionList.add(resetButtonAction);
		loadButtonActionList.add(loadButtonAction);
		storeButtonActionList.add(storeButtonAction);
		linButtonActionList.add(linButtonAction);
		expButtonActionList.add(expButtonAction);
		logButtonActionList.add(logButtonAction);

		("pauseButtonActionList -> " ++ pauseButtonActionList).postln;
		("stopButtonActionList -> " ++ stopButtonActionList).postln;
		("refreshButtonActionList -> " ++ refreshButtonActionList).postln;
		("resetButtonActionList -> " ++ resetButtonActionList).postln;
		("loadButtonActionList -> " ++ loadButtonActionList).postln;
		("storeButtonActionList -> " ++ storeButtonActionList).postln;
		("linButtonActionList -> " ++ linButtonActionList).postln;
		("expButtonActionList -> " ++ expButtonActionList).postln;
		("logButtonActionList -> " ++ logButtonActionList).postln;
	}

	// override PresetButton actions
	// (exclusive control needed by this class)
	overridePresetButtonActions {
		presetButtonArray.do({ arg presetButton;
			presetButton.button.states = [
					[presetButton.name.asString, Color.black, Color.clear],
					[presetButton.name.asString, Color.black, Color.white],
					[presetButton.name.asString, Color.black, Color.white]];
			presetButton.button.action = { arg button;
				("BEG ********** " ++ name ++ " " ++ "button.action **********").postln;
				// if reset button is currently selected...
				if(resetButton.value == 1, {
					button.value = 0;
					if(timeZoomSlider.currentBox.value == 0, {
						"don't interpolate...".postln;

						// quickly zero everything out...
						// in order to do on-the-fly preset button sequencing!!!
						// *** Placed inside of the loop to allow for dynamic
						// 'store' while interpolating ***
						if(presetInterpolator.isPlaying, {
							presetInterpolator.stop;
							timeRangeSliderTask.stop;
						});

						// fill timeRangeSlider progressBar
						timeRangeSlider.lo_(0).hi_(1);

						// reset (load default)
						presetButton.preset.load(\defaultDictionary);
					}, {
						"interpolate...".postln;

						// quickly zero everything out...
						// in order to do on-the-fly preset button sequencing!!!
						// *** Placed inside of the loop to allow for dynamic
						// 'store' while interpolating ***
						if(presetInterpolator.isPlaying, {
							presetInterpolator.stop;
							timeRangeSliderTask.stop;
						});

						// set the parameters and start the PresetInterpolator
						presetInterpolator.setParameters(
								timeZoomSlider.currentBox.value, 10);
						presetInterpolator.start(
								presetButton.name.asInteger, \defaultDictionary);

						this.runTimeRangeSliderTask(timeZoomSlider.currentBox.value, 10);
						pauseButton.enabled = true;
						pauseButtonFlashRoutine.stop;
					});
					statusLabel.string = "reset " ++ presetButton.name;
				}, {
					// if load button is currently selected...
					if(loadButton.value == 1, {
						button.value = 1;
						if(timeZoomSlider.currentBox.value == 0, {
							"don't interpolate...".postln;

							// quickly zero everything out...
							// in order to do on-the-fly preset button sequencing!!!
							// *** Placed inside of the loop to allow for dynamic
							// 'store' while interpolating ***
							if(presetInterpolator.isPlaying, {
								presetInterpolator.stop;
								timeRangeSliderTask.stop;
							});

							// fill timeRangeSlider progressBar
							timeRangeSlider.lo_(0).hi_(1);

							// load (load current)
							presetButton.preset.load(\currentDictionary);
						}, {
							"interpolate...".postln;

							// quickly zero everything out...
							// in order to do on-the-fly preset button sequencing!!!
							// *** Placed inside of the loop to allow for dynamic
							// 'store' while interpolating ***
							if(presetInterpolator.isPlaying, {
								presetInterpolator.stop;
								timeRangeSliderTask.stop;
							});

							// set the parameters and start the PresetInterpolator
							presetInterpolator.setParameters(
									timeZoomSlider.currentBox.value, 10);
							presetInterpolator.start(
									presetButton.name.asInteger, \currentDictionary);

							this.runTimeRangeSliderTask(timeZoomSlider.currentBox.value, 10);
							pauseButton.enabled = true;
							pauseButtonFlashRoutine.stop;
						});
						statusLabel.string = "load " ++ presetButton.name;
					}, {
						// if store button is currently selected...
						if(storeButton.value == 1, {
							button.value = 2;
							// store (store current)
							presetButton.preset.store(\currentDictionary);
								// maybe the user pauses, and then decides to
								// store the current settings, and then wants to
								// unpause! (thus, don't do what's on next line!)
								//pauseButtonFlashRoutine.stop;
						});
						statusLabel.string = "store " ++ presetButton.name;
					});
				});
				("END ********** " ++ name ++ " " ++ "button.action **********\n").postln;
			};
		});
	}

	changeModeState { arg view;
		// turn off current button
		currentModeButtonSelected.value = 0;
		// assign new button to currentModButtonSelected
		currentModeButtonSelected = view;
		// turn on 'new' current button
		currentModeButtonSelected.value = 1;
	}

	changeCurveState { arg view;
		// turn off current button
		currentCurveButtonSelected.value = 0;
		// assign new button to currentCurveButtonSelected
		currentCurveButtonSelected = view;
		// turn on 'new' current button
		currentCurveButtonSelected.value = 1;
	}

	runTimeRangeSliderTask { arg argInterpolationTime,
			argMaxStepsPerSec = 10;
		var	maxSteps = nil;
		var	range = nil;
		var	timesToMove = nil;
		var	increment = nil;
		var	delta = nil;
		// maximum number of steps to keep update rate under maxStepsPerSec
		maxSteps = ((argMaxStepsPerSec * argInterpolationTime).asInteger).max(1);
		range = timeRangeSlider.step.reciprocal;
		timesToMove = (if(range < maxSteps, { range; }, { maxSteps; })).max(1);
		increment = range / timesToMove;
		delta = argInterpolationTime / timesToMove;
		timeRangeSliderTask = Task.new({
			var pos = 0;
			timesToMove.do({
				{ timeRangeSlider.lo_(0).hi_(pos.round(timeRangeSlider.step) *
						range.reciprocal); }.defer;
				("pos -> " ++ pos).postln;
				pos = pos + increment;
				delta.wait;
			});
		}, SystemClock);
		timeRangeSliderTask.start;
	}

	runPauseButtonFlashRoutine { arg argRate = 1,
			argMaxRate = 5;
		var	colorA = nil;
		var	colorB = nil;
		var	flashRate = nil;
		var	delta = nil;
		colorA = Color.black;
		colorB = Color.blue;
		flashRate = argRate.min(argMaxRate); // rate in cps (on + off)
		delta = (flashRate * 2).reciprocal; // one cycle = one oscil = rate * 2
		pauseButtonFlashRoutine = Routine.new({
			var i = 0;
			loop({
				{ pauseButton.states_([["||", if(i.even, { colorA; }, { colorB; }), Color.clear]]).refresh; }.defer;
				("i -> " ++ i).postln;
				i = (i + 1) % 2; // 0, 1, 0, 1, 0, 1...
				delta.wait;
			});
		});
		pauseButtonFlashRoutine.play(SystemClock);
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// draw the pauseButton on the screen within a parent window
	drawPauseButton { arg xOff = 5, yOff = 5, width = 20, height = 20;
		pauseButton.bounds_(Rect.new(xOff, yOff, width, height));
	}

	// draw the stopButton widget on the screen within a parent window
	drawStopButton { arg xOff = 5, yOff = 5, width = 20, height = 20;
		stopButton.bounds_(Rect.new(xOff, yOff, width, height));
	}

	// draw the refreshButton widget on the screen within a parent window
	drawRefreshButton { arg xOff = 5, yOff = 5, width = 20, height = 20;
		refreshButton.bounds_(Rect.new(xOff, yOff, width, height));
	}

	// draw the ctrl widgets on the screen within a parent window
	drawCtrlButtons { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			buttonLength = 20;
		var	offsets = nil; // an array of relative widget offsets

		offsets = [0, buttonLength, buttonLength * 2];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i;
			each = each + (i * gap); });

		pauseButton.bounds_(Rect.new(xOff + offsets[0],
				yOff, buttonLength, width));
		stopButton.bounds_(Rect.new(xOff + offsets[1],
				yOff, buttonLength, width));
		refreshButton.bounds_(Rect.new(xOff + offsets[2],
				yOff, buttonLength, width));
	}

	// draw the mode widgets on the screen within a parent window
	drawModeButtons { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			buttonLength = 40;
		var	offsets = nil; // an array of relative widget offsets

		offsets = [0, buttonLength, buttonLength * 2];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i;
			each = each + (i * gap); });

		resetButton.bounds = Rect.new(xOff + offsets[0],
				yOff, buttonLength, width);
		loadButton.bounds = Rect.new(xOff + offsets[1],
				yOff, buttonLength, width);
		storeButton.bounds = Rect.new(xOff + offsets[2],
				yOff, buttonLength, width);
	}

	// draw the curve widgets on the screen within a parent window
	drawCurveButtons { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			buttonLength = 40;
		var	offsets = nil; // an array of relative widget offsets

		offsets = [0, buttonLength, buttonLength * 2];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i;
			each = each + (i * gap); });

		linButton.bounds = Rect.new(xOff + offsets[0],
				yOff, buttonLength, width);
		expButton.bounds = Rect.new(xOff + offsets[1],
				yOff, buttonLength, width);
		logButton.bounds = Rect.new(xOff + offsets[2],
				yOff, buttonLength, width);
	}

	// draw the statusLabel widget on the screen within a parent window
	drawStatusLabel { arg xOff = 5, yOff = 5, width = 40, height = 20;
		statusLabel.bounds = Rect.new(xOff, yOff, width, height);
	}

	// draw the timeZoomSlider widget on the screen within a parent window
	drawTimeZoomSlider { arg xOff = 5, yOff = 5,
			orientation = \horizontal,
			width = 20, gap = 5,
			sliderLength = 200, boxLength = 40,
			currentBoxSide = \right;
		this.timeZoomSlider.draw(xOff, yOff,
				orientation,
				width, gap,
				sliderLength, boxLength,
				currentBoxSide);
	}

	// draw the timeRangeSlider widget on the screen within a parent window
	drawTimeRangeSlider { arg xOff = 5, yOff = 5, width = 200, height = 5;
		timeRangeSliderLabel.bounds_(Rect.new(xOff, yOff, width, height));
		timeRangeSlider.bounds_(Rect.new(xOff, yOff, width, height));
	}
}
	