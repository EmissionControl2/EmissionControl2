/*
 * David Thall
 * ModulationSlider.sc
 * CG2 Project
 */

// Extends MappableSlider, implements modulation routing

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ModulationSlider : MappableSlider {
	// state variables
	var	<lastCurrentBoxValue = nil; 	// stores last currentBox.value
	// SCView's
	var	<bypassButton = nil;			// SCButton reference
	var	<posNegButton = nil;			// SCButton reference
	var	<modSrcPopUpMenu = nil;			// SCPopUpMenu reference
	var	<gainViewButton = nil;			// SCButton reference
	// .action function lists
	var	bypassButtonActionList = nil;	// bypassButton List of actions
	var	posNegButtonActionList = nil;	// posNegButton List of actions
	var	modSrcPopUpMenuActionList = nil;	// modSrcPopUpMenu List of actions
	var	gainViewButtonActionList = nil;	// gainViewButton List of actions

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new ModulationSlider
	*new { arg argParent,
			argJoystickControlSetArray, argTiltControlSet,
			argName,
			argMin, argMax,
			argLeft, argRight,
			argWarp, argStep,
			argDefault, argUnits;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		if(((argJoystickControlSetArray.isNil) ||
				(argTiltControlSet.isNil)), {
			(this.asString ++ ": " ++
					"ControlSets must be provided!").postln;
			^nil;
		});
		// overloading (Java-style)
		if(argName.isNil, {
			^super.new(argParent,
					argJoystickControlSetArray, argTiltControlSet)
					.initModulationSlider;
		}, {
			^super.new(argParent,
					argJoystickControlSetArray, argTiltControlSet,
					argName,
					argMin, argMax,
					argLeft, argRight,
					argWarp, argStep,
					argDefault, argUnits)
					.initModulationSlider;
		});
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ModulationSlider instance into a consistent state
	initModulationSlider {
		this.createModulationSliderGUIViews;
		this.addModulationSliderActions;
	}

	// create and initialize the internal gui views
	createModulationSliderGUIViews {
		// defaults
		var	buttonCanFocus = nil;
		var	bypassButtonFont = nil;
		var	posNegButtonFont = nil;
		var	popUpMenuCanFocus = nil;
		var	popUpMenuFont = nil;
		var	popUpMenuBackground = nil;
		var	popUpMenuStringColor = nil;
		var	gainViewButtonFont = nil;

		var	bypassButtonDataList = nil;
		var	posNegButtonDataList = nil;
		var	modSrcPopUpMenuDataList = nil;
		var	gainViewButtonDataList = nil;

		var	bypassButtonStates = nil;
		var	posNegButtonStates = nil;
		var	modSrcPopUpMenuItems = nil;
		var	gainViewButtonStates = nil;

		buttonCanFocus = false;
		bypassButtonFont = Font.new("Helvetica", 10);
		posNegButtonFont = Font.new("Helvetica", 12);
		popUpMenuCanFocus = false;
		popUpMenuFont = Font.new("Helvetica", 10);
		popUpMenuBackground = HiliteGradient.new(Color.gray,
				Color.black, \h, 256, 0);
		popUpMenuStringColor = Color.white;
		gainViewButtonFont = Font.new("Helvetica", 10);

		bypassButtonDataList = [["off", "on"],
				[Color.black, Color.blue], Color.clear];
		posNegButtonDataList = [["+", "-"],
				[Color.black, Color.blue], Color.clear];

		modSrcPopUpMenuDataList = ["noise", "sine a", "sine b", "square", "saw"];
		gainViewButtonDataList = ["",
				Color.clear, [Color.clear, Color.blue.alpha_(0.2)]];

		bypassButtonStates = Array.fill(2, { arg i;
				[bypassButtonDataList[0][i],
				bypassButtonDataList[1][i],
				bypassButtonDataList[2]]; });
		posNegButtonStates = Array.fill(2, { arg i;
				[posNegButtonDataList[0][i],
				posNegButtonDataList[1][i],
				posNegButtonDataList[2]]; });
		modSrcPopUpMenuItems = modSrcPopUpMenuDataList;
		gainViewButtonStates = Array.fill(2, { arg i;
				[gainViewButtonDataList[0],
				gainViewButtonDataList[1],
				gainViewButtonDataList[2][i]]; });

		// bypassButton
		bypassButton = Button.new(parent);
		bypassButton.canFocus_(buttonCanFocus);
		bypassButton.font_(bypassButtonFont);
		bypassButton.states_(bypassButtonStates);
		bypassButton.value_(0);
		bypassButton.action_({ arg bypassButton;
			bypassButtonActionList.do({ arg action;
				action.value(bypassButton); // evaluate
			});
		});

		("bypassButton -> " ++ bypassButton).postln;
		("bypassButton.canFocus -> " ++ bypassButton.canFocus).postln;
		("bypassButton.font -> " ++ bypassButton.font).postln;
		("bypassButton.states -> " ++ bypassButton.states).postln;
		("bypassButton.value -> " ++ bypassButton.value).postln;
		("bypassButton.action -> " ++ bypassButton.action).postln;

		// posNegButton
		posNegButton = Button.new(parent);
		posNegButton.canFocus_(buttonCanFocus);
		posNegButton.font_(posNegButtonFont);
		posNegButton.states_(posNegButtonStates);
		posNegButton.value_(0);
		posNegButton.action_({ arg posNegButton;
			posNegButtonActionList.do({ arg action;
				action.value(posNegButton); // evaluate
			});
		});

		("posNegButton -> " ++ posNegButton).postln;
		("posNegButton.canFocus -> " ++ posNegButton.canFocus).postln;
		("posNegButton.font -> " ++ posNegButton.font).postln;
		("posNegButton.states -> " ++ posNegButton.states).postln;
		("posNegButton.value -> " ++ posNegButton.value).postln;
		("posNegButton.action -> " ++ posNegButton.action).postln;

		// modSrcPopUpMenu
		modSrcPopUpMenu = PopUpMenu.new(parent);
		modSrcPopUpMenu.canFocus_(popUpMenuCanFocus);
		modSrcPopUpMenu.font_(popUpMenuFont);
		modSrcPopUpMenu.background_(popUpMenuBackground);
		modSrcPopUpMenu.stringColor_(popUpMenuStringColor);
		modSrcPopUpMenu.items_(modSrcPopUpMenuItems);
		modSrcPopUpMenu.value_(0);
		modSrcPopUpMenu.action_({ arg modSrcPopUpMenu;
			modSrcPopUpMenuActionList.do({ arg action;
				action.value(modSrcPopUpMenu); // evaluate
			});
		});

		("modSrcPopUpMenu -> " ++ modSrcPopUpMenu).postln;
		("modSrcPopUpMenu.canFocus -> " ++ modSrcPopUpMenu.canFocus).postln;
		("modSrcPopUpMenu.font -> " ++ modSrcPopUpMenu.font).postln;
		("modSrcPopUpMenu.background -> " ++ modSrcPopUpMenu.background).postln;
		("modSrcPopUpMenu.stringColor -> " ++ modSrcPopUpMenu.stringColor).postln;
		("modSrcPopUpMenu.items -> " ++ modSrcPopUpMenu.items).postln;
		("modSrcPopUpMenu.value -> " ++ modSrcPopUpMenu.value).postln;
		("modSrcPopUpMenu.action -> " ++ modSrcPopUpMenu.action).postln;

		// gainViewButton
		gainViewButton = Button.new(parent);
		gainViewButton.canFocus_(buttonCanFocus);
		gainViewButton.font_(gainViewButtonFont);
		gainViewButton.states_(gainViewButtonStates);
		gainViewButton.value_(0);
		gainViewButton.action_({ arg gainViewButton;
			gainViewButtonActionList.do({ arg action;
				action.value(gainViewButton); // evaluate
			});
		});

		("gainViewButton -> " ++ gainViewButton).postln;
		("gainViewButton.canFocus -> " ++ gainViewButton.canFocus).postln;
		("gainViewButton.font -> " ++ gainViewButton.font).postln;
		("gainViewButton.states -> " ++ gainViewButton.states).postln;
		("gainViewButton.value -> " ++ gainViewButton.value).postln;
		("gainViewButton.action -> " ++ gainViewButton.action).postln;

		// if default == 0
		if(default == 0, {
			// disable the bypassButton and display 'off'
			bypassButton.enabled = false;
			bypassButton.value = 0;
		}, {
			// enable the bypassButton and display 'on'
			bypassButton.enabled = true;
			bypassButton.value = 1;
		});
	}

	// define & register the action event functions...add them to actionList's
	addModulationSliderActions {
		// action functions to append to action list
		var	sliderAction = nil;
		var	leftBoxAction = nil;
		var	rightBoxAction = nil;
		var	currentBoxAction = nil;

		var	bypassButtonAction = nil;
		var	posNegButtonAction = nil;
		var	modSrcPopUpMenuAction = nil;
		var	gainViewButtonAction = nil;

		// define actions (extended from ZoomSlider)
		sliderAction = { arg slider;
				this.doModulationSliderCurrentBoxCheckAction(slider); };
		leftBoxAction = { arg leftBox;
				this.doModulationSliderCurrentBoxCheckAction(leftBox); };
		rightBoxAction = { arg rightBox;
				this.doModulationSliderCurrentBoxCheckAction(rightBox); };
		currentBoxAction = { arg currentBox;
				this.doModulationSliderCurrentBoxCheckAction(currentBox); };

		// add actions to actionLists (extended from ZoomSlider)
		sliderActionList.add(sliderAction);
		leftBoxActionList.add(leftBoxAction);
		rightBoxActionList.add(rightBoxAction);
		currentBoxActionList.add(currentBoxAction);

		// define actions
		bypassButtonAction = { arg bypassButton;
			if(bypassButton.value == 0, { // off

				(name ++ " " ++ "bypassButton -> " ++
						[bypassButton.value,
						bypassButton.states[bypassButton.value][0]]).postln;

				// store the currentBox.value into 'lastCurrentBoxValue'
				lastCurrentBoxValue = currentBox.value;
				("lastCurrentBoxValue has been stored as " ++
						lastCurrentBoxValue).postln;
				// move slider to '0' position (relative)
				// note: makes more sense visually to move to '0' when ==
				slider.value = if(leftBox.value <= rightBox.value, {
					0;
				}, {
					1;
				});
				(name ++ " " ++
						"slider.value -> " ++ slider.value).postln;
				// set currentBox to '0'
				currentBox.value = 0;
				(name ++ " " ++ "currentBox.value -> " ++
						currentBox.value).postln;
			});
			if(bypassButton.value == 1,  { // on
				(name ++ " " ++ "bypassButton -> " ++
						[bypassButton.value,
						bypassButton.states[bypassButton.value][0]]).postln;

				// move the slider back to 'lastCurrentBoxValue'
				slider.value = if(leftBox.value == rightBox.value, {
					0;
				}, {
					spec.unmap(lastCurrentBoxValue + offset); //JKilg:
				});                                           //lastCurrentBoxValue is nil; resulting in error
				("lastCurrentBoxValue has been set back to " ++
						(spec.map(slider.value) - offset)).postln;
				// set the currentBox to 'lastCurrentBoxValue'

				currentBox.value = lastCurrentBoxValue; //the nil causes an error here as well
				("currentBox.value -> " ++
						currentBox.value).postln;
			});
		};

		posNegButtonAction = { arg posNegButton;
			if(posNegButton.value == 0, { // positive
				(name ++ " " ++ "posNegButton -> " ++
						[posNegButton.value,
						posNegButton.states[posNegButton.value][0]]).postln;
			});
			if(posNegButton.value == 1, { // negative
				(name ++ " " ++ "posNegButton -> " ++
						[posNegButton.value,
						posNegButton.states[posNegButton.value][0]]).postln;
			});
		};

		modSrcPopUpMenuAction = { arg modSrcPopUpMenu;
			(name ++ " " ++ "modSrcPopUpMenu -> " ++
					[modSrcPopUpMenu.value,
					modSrcPopUpMenu.items[modSrcPopUpMenu.value]]).postln;
		};

		gainViewButtonAction = { arg gainViewButton;
			if(gainViewButton.value == 0, { // off
				(name ++ " " ++ "gainViewButton -> " ++
						[gainViewButton.value,
						gainViewButton.states[gainViewButton.value][0]]).postln;
			});
			if(gainViewButton.value == 1, { // on
				(name ++ " " ++ "gainViewButton -> " ++
						[gainViewButton.value,
						gainViewButton.states[gainViewButton.value][0]]).postln;
			});
		};

		// create new actionLists
		bypassButtonActionList = List.new;
		posNegButtonActionList = List.new;
		modSrcPopUpMenuActionList = List.new;
		gainViewButtonActionList = List.new;

		// add actions to actionLists
		bypassButtonActionList.add(bypassButtonAction);
		posNegButtonActionList.add(posNegButtonAction);
		modSrcPopUpMenuActionList.add(modSrcPopUpMenuAction);
		gainViewButtonActionList.add(gainViewButtonAction);
	}

	// do bypassButton state change & enabling/disabling depending on currentBox.value...
	doModulationSliderCurrentBoxCheckAction { arg view;
		if(currentBox.value == 0, {
			bypassButton.value_(0);
			bypassButton.enabled_(false);
		}, {
			bypassButton.value_(1);
			bypassButton.enabled_(true);
		});
		/*(name ++ " " ++ "bypassButton -> " ++
				[bypassButton.value,
				bypassButton.states[bypassButton.value][0]]).postln;
		("bypassButton.enabled -> " ++
				bypassButton.enabled).postln;*/
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// add a source of modulation to the modSrcPopUpMenu
	// this should also do any routing and parent window refreshing
	addModSrc { arg modSrcString; // currently only a string
		var	itemsList;
		itemsList = modSrcPopUpMenu.items;
		("The previous items in the " ++ name ++ " " ++
				"modSrcPopUpMenu were " ++ itemsList).postln;
		itemsList.add(modSrcString);
		modSrcPopUpMenu.items = itemsList;
		("The new items in the " ++ name ++ " " ++
				"modSrcPopUpMenu are " ++ modSrcPopUpMenu.items).postln;
	}

	// plug this ModulationSlider instance into a synth's parameter input!!!
	// should include a third arg: unitScaler
	// (for preprocessed mapping before being sent to synth)
	// NOTE: overrides (extends) plugIntoSynth instance method!
	plugIntoSynth { arg argSynth, argParameter;
		// actions to append to action list
		var	bypassButtonAction = nil;

		// first add actions from the superclass!!!
		super.plugIntoSynth(argSynth, argParameter);

		bypassButtonAction = { arg bypassButton;
			argSynth.set(argParameter.asSymbol, currentBox.value);
		};

		bypassButtonActionList.add(bypassButtonAction);

		("bypassButtonActionList -> " ++ bypassButtonActionList).postln;
	}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			sliderLength = 200, boxLength = 40,
			buttonLength = 20, popUpMenuLength = 80;
		var	offsets = nil; // an array of relative widget offsets

		slider.orientation_(\horizontal);

		offsets = [0, buttonLength, buttonLength * 2,
				(buttonLength * 2) + popUpMenuLength,
				(buttonLength * 3) + popUpMenuLength,
				(buttonLength * 4) + popUpMenuLength,
				(buttonLength * 5) + popUpMenuLength,
				(buttonLength * 5) + popUpMenuLength + boxLength,
				(buttonLength * 5) + popUpMenuLength + boxLength + sliderLength,
				(buttonLength * 5) + popUpMenuLength + (boxLength * 2) + sliderLength];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i;
			each = each + (i * gap); });

		bypassButton.bounds_(Rect.new(xOff + offsets[0],
				yOff, buttonLength, width));
		posNegButton.bounds_(Rect.new(xOff + offsets[1],
				yOff, buttonLength, width));
		modSrcPopUpMenu.bounds_(Rect.new(xOff + offsets[2],
				yOff, popUpMenuLength, width));
		gainViewButton.bounds_(Rect.new(xOff + offsets[3],
				yOff, buttonLength, width));
		lockButton.bounds_(Rect.new(xOff + offsets[4],
				yOff, buttonLength, width));
		axisButton.bounds_(Rect.new(xOff + offsets[5],
				yOff, buttonLength, width));
		leftBoxLabel.bounds_(Rect.new(xOff + offsets[6],
				yOff, boxLength, width)).front.acceptsMouse_(false);
		leftBox.bounds_(Rect.new(xOff + offsets[6],
				yOff, boxLength, width));
		sliderLabel.bounds_(Rect.new(xOff + offsets[7],
				yOff, sliderLength, width)).front.acceptsMouse_(false);
		slider.bounds_(Rect.new(xOff + offsets[7],
				yOff, sliderLength, width));
		rightBoxLabel.bounds_(Rect.new(xOff + offsets[8],
				yOff, boxLength, width)).front.acceptsMouse_(false);
		rightBox.bounds_(Rect.new(xOff + offsets[8],
				yOff, boxLength, width));
		currentBoxLabel.bounds_(Rect.new(xOff + offsets[9],
				yOff, boxLength, width)).front.acceptsMouse_(false);
		currentBox.bounds_(Rect.new(xOff + offsets[9],
				yOff, boxLength, width));
		(name ++ " " ++ "slider was drawn at [" ++
				xOff ++ ", " ++ yOff ++ "]").postln;
	}
}