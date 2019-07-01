/*
 * David Thall
 * PresetButton.sc
 * CG2 Project
 */

// A PresetButton contains a single instance of Preset

// Update: no way to directly load/store using the \defaultDictionary
// (will be handled externally for the time being)
// One way to fix this is to have a settable variable to choose if the .action
// calls load & store on the \currentDictionary or \defaultDictionary!!!

// When clicked, it load's the current config stored in its instance of Preset
// When clicked (w/ Command Key), it store's the current state of the gui widgets
// (sliders & buttons) to its instance of Preset
// When clicked (w/ Option Key), it will reload the default config
// in its instance of Preset

// 79 chars
///////////////////////////////////////////////////////////////////////////////

PresetButton {
	// state variables
	var	<parent = nil;			// ref to a parent SCWindow
	var	<name = nil;				// SCStaticText to display on SCButton
	// GUI Widgets
	var	<button = nil;			// SCButton reference
	// Preset (reference)
	var	<>preset = nil;			// Preset reference
	// .action function lists
	var	buttonActionList = nil;		// button List of actions

	// create and name a new PresetButton
	*new { arg argParent, argName, argPreset;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		if(argPreset.isNil, {
			(this.asString ++ ": " ++
					"A Preset must be provided!").postln;
			^nil;
		});
		^super.new.initPresetButton(argParent, argName, argPreset);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the PresetButton instance into a consistent state
	initPresetButton { arg argParent, argName, argPreset;
		this.initPresetButtonStateVariables(argParent, argName, argPreset);
		this.createPresetButtonGUIViews;
		this.addPresetButtonActions;
	}

	// initialize all the internal state variables
	initPresetButtonStateVariables { arg argParent, argName, argPreset;
		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });
		preset = argPreset;

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("preset -> " ++ preset).postln;
	}

	// create and initialize the internal gui views
	createPresetButtonGUIViews {
		// defaults
		var	buttonCanFocus = nil;
		var	buttonFont = nil;

		var	buttonDataList = nil;

		var	buttonStates = nil;

		buttonCanFocus = false;
		buttonFont = Font.new("Helvetica", 10);

		buttonDataList = [name.asString,
				[Color.black, Color.blue], Color.clear];

		buttonStates = Array.fill(2, { arg i;
				[buttonDataList[0],
				buttonDataList[1][i],
				buttonDataList[2]]; });

		// button
		button = SCButton.new(parent);
		button.canFocus_(buttonCanFocus);
		button.font_(buttonFont);
		button.states_(buttonStates);
		button.value_(0);
		button.action_({ arg button;
			buttonActionList.do({ arg action;
				action.value(button); // evaluate
			});
		});

		("button -> " ++ button).postln;
		("button.canFocus -> " ++ button.canFocus).postln;
		("button.font -> " ++ button.font).postln;
		("button.states -> " ++ button.states).postln;
		("button.value -> " ++ button.value).postln;
		("button.action -> " ++ button.action).postln;
	}

	// define & register the action event functions...add them to actionList's
	addPresetButtonActions {
		// action functions to append to action list
		var	buttonAction = nil;

		// define actions
		buttonAction = { arg button;
			if(button.value == 0, { // load (load current)
				preset.load(\currentDictionary);
			});
			if(button.value == 1, { // store (store current)
				preset.store(\currentDictionary);
			});
		};

		// create new actionLists
		buttonActionList = List.new;

		// add actions to actionLists
		buttonActionList.add(buttonAction);

		("buttonActionList -> " ++ buttonActionList).postln;
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5, width = 20, height = 20;
		button.bounds_(Rect.new(xOff, yOff, width, height));
	}
}