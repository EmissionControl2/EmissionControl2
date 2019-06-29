/*
 * David Thall
 * MappableSlider.sc
 * CG2 Project
 */

// Extends ZoomSlider, implements x/y mapping and lockable features

// Determine whether or not to remove setters to ControlSet references
// Add: if user presses option key while pressing lockButton, disable all views
//	Otherwise, just remove the slider from the TiltControlSet!!!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

MappableSlider : ZoomSlider {

	// ControlSets (external references)
	var	<joystickControlSetArray = nil;		// Joystick ControlSets x/y
	var	<tiltControlSet = nil;				// Tilt ControlSet
	// state variables
	var	<numAxisButtonStates = nil; 	  	// number of axisButton states
	var	<currentAxisButtonValue = nil; 		// current axisButton.value
	// SCView's
	var	<lockButton;						// SCButton reference
	var	<axisButton;						// SCButton reference
	// .action function lists
	var	lockButtonActionList = nil;			// lockButton List of actions
	var	axisButtonActionList = nil; 		// axisButton List of actions

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new MappableSlider
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
			^super.new(argParent)
					.initMappableSlider(argJoystickControlSetArray,
							argTiltControlSet);
		}, {
			^super.new(argParent, argName,
					argMin, argMax,
					argLeft, argRight,
					argWarp, argStep,
					argDefault, argUnits)
					.initMappableSlider(argJoystickControlSetArray,
							argTiltControlSet);
		});
	}

	/* PRIVATE IMPLEMENTATION */

	// get the MappableSlider instance into a consistent state
	initMappableSlider { arg argJoystickControlSetArray, argTiltControlSet;
		this.initMappableSliderControlSets(argJoystickControlSetArray,
				argTiltControlSet);
		this.initMappableSliderStateVariables;
		this.createMappableSliderGUIViews;
		this.addMappableSliderActions;
	}

	// initialize the 'internal references' to the Joystick & Tilt ControlSets
	initMappableSliderControlSets { arg argJoystickControlSetArray,
			argTiltControlSet;
		joystickControlSetArray = argJoystickControlSetArray;
		tiltControlSet = argTiltControlSet;

		("joystickControlSetArray -> " ++ joystickControlSetArray).postln;
		("tiltControlSet -> " ++ tiltControlSet).postln;
	}

	// initialize all the internal state variables
	initMappableSliderStateVariables {
		numAxisButtonStates = (1 + joystickControlSetArray.size);
		currentAxisButtonValue = 0; // just to get it into a useable state

		("numAxisButtonStates -> " ++ numAxisButtonStates).postln;
		("currentAxisButtonValue -> " ++ currentAxisButtonValue).postln;
	}

	// create and initialize the internal gui views
	createMappableSliderGUIViews {
		// defaults
		var	buttonCanFocus = nil;
		var	buttonFont = nil;

		var	pink = nil;
		var	red = nil;
		var	orange = nil;
		var	yellow = nil;
		var	green = nil;
		var	blue = nil;
		var	purple = nil;
		var	gray = nil;
		var	colors = nil;

		var	lockButtonDataList = nil;
		var	axisButtonDataList = nil;

		var	lockButtonStates = nil;
		var	axisButtonStates = nil;

		buttonCanFocus = false;
		buttonFont = Font.new("Helvetica", 10);

		pink = Color.new255(255, 192, 203);
		red = Color.red.alpha_(0.5);
		orange = Color.new255(255, 165, 0);
		yellow = Color.yellow.alpha_(0.7);
		green = Color.green;
		blue = Color.blue.alpha_(0.7);
		purple = Color.new255(160, 32, 240);
		gray = Color.gray;
		//colors = [pink, red, orange, yellow, green, blue, purple, gray];
		colors = [green, blue, purple, gray, pink, red, orange, yellow];

		lockButtonDataList = ["Lk",
				[Color.black, Color.blue], Color.clear];
		axisButtonDataList = [["x", "y"],
				Color.black, colors.stutter(2)];

		lockButtonStates = Array.fill(2, { arg i;
				[lockButtonDataList[0],
				lockButtonDataList[1][i],
				lockButtonDataList[2]]; });
		axisButtonStates = Array.fill(numAxisButtonStates - 1, { arg i;
				[axisButtonDataList[0][i%2],
				axisButtonDataList[1],
				axisButtonDataList[2][i%16]]; }).insert(0,
						["off", Color.black, Color.clear]);
		// lockButton
		lockButton = SCButton.new(parent);
		lockButton.canFocus_(buttonCanFocus);
		lockButton.font_(buttonFont);
		lockButton.states_(lockButtonStates);
		lockButton.value_(0);
		lockButton.action_({ arg lockButton;
			lockButtonActionList.do({ arg action;
				action.value(lockButton); // evaluate
			});
		});

		("lockButton -> " ++ lockButton).postln;
		("lockButton.canFocus -> " ++ lockButton.canFocus).postln;
		("lockButton.font -> " ++ lockButton.font).postln;
		("lockButton.states -> " ++ lockButton.states).postln;
		("lockButton.value -> " ++ lockButton.value).postln;
		("lockButton.action -> " ++ lockButton.action).postln;

		// axisButton
		axisButton = SCButton.new(parent);
		axisButton.canFocus_(buttonCanFocus);
		axisButton.font_(buttonFont);
		axisButton.states_(axisButtonStates);
		axisButton.value_(0);
		axisButton.action_({ arg axisButton;
			axisButtonActionList.do({ arg action;
				action.value(axisButton); // evaluate
			});
		});

		("axisButton -> " ++ axisButton).postln;
		("axisButton.canFocus -> " ++ axisButton.canFocus).postln;
		("axisButton.font -> " ++ axisButton.font).postln;
		("axisButton.states -> " ++ axisButton.states).postln;
		("axisButton.value -> " ++ axisButton.value).postln;
		("axisButton.action -> " ++ axisButton.action).postln;

		// if lockButton.value == 0,
		if(lockButton.value == 0, {
			// add this view to the tiltControlSet for immediate use
			tiltControlSet.add(this);
		});
	}

	// define & register the action event functions...add them to actionList's
	addMappableSliderActions {
		// action functions to append to action list
		var	lockButtonAction = nil;
		var	axisButtonAction = nil;

		// define actions
		lockButtonAction = { arg lockButton;
			if(lockButton.value == 0, { // unlocked
				(name ++ " " ++ "lockButton -> " ++
						[lockButton.value,
						lockButton.states[lockButton.value][0]]).postln;

				// for every other case but state 0 (off)
				if(currentAxisButtonValue != 0, {
					joystickControlSetArray.at(
							(currentAxisButtonValue - 1) % numAxisButtonStates)
							.add(this); // Do I need the modulus operation???
				});

				tiltControlSet.add(this);

				// enable all
				axisButton.enabled_(true);
				leftBox.enabled_(true);
				sliderLabel.stringColor_(Color.black.alpha_(0.7));
				slider.enabled_(true);
				rightBox.enabled_(true);
				currentBox.enabled_(true);
			});
			if(lockButton.value == 1, { // locked
				(name ++ " " ++ "lockButton -> " ++
						[lockButton.value,
						lockButton.states[lockButton.value][0]]).postln;

				// for every other case but state 0 (off)
				if(currentAxisButtonValue != 0, {
					joystickControlSetArray.at(
							(currentAxisButtonValue - 1) % numAxisButtonStates)
							.remove(this); // Do I need the modulus operation???
				});

				tiltControlSet.remove(this);

				// disable all
				axisButton.enabled_(false);
				leftBox.enabled_(false);
				sliderLabel.stringColor_(Color.black.alpha_(0.7 - 0.4)); // 0.3???
				slider.enabled_(false);
				rightBox.enabled_(false);
				currentBox.enabled_(false);
			});

			("axisButton.enabled -> " ++ axisButton.enabled).postln;
			("leftBox.enabled -> " ++ leftBox.enabled).postln;
			("sliderLabel.stringColor -> " ++ sliderLabel.stringColor).postln;
			("slider.enabled -> " ++ slider.enabled).postln;
			("rightBox.enabled -> " ++ rightBox.enabled).postln;
			("currentBox.enabled -> " ++ currentBox.enabled).postln;
		};

		axisButtonAction = { arg axisButton;
			(name ++ " " ++ "axisButton -> " ++
				[axisButton.value,
				axisButton.states[axisButton.value][0]]).postln;
			this.changeAxisButtonState(axisButton);
		};

		// create new actionLists
		lockButtonActionList = List.new;
		axisButtonActionList = List.new;

		// add actions to actionLists
		lockButtonActionList.add(lockButtonAction);
		axisButtonActionList.add(axisButtonAction);

		("lockButtonActionList -> " ++ lockButtonActionList).postln;
		("axisButtonActionList -> " ++ axisButtonActionList).postln;
	}

	// when the axisButton.value is changed,
	// update the joystickControlSetArray accordingly
	// (change can be non-linear... i.e., step vs. hop)
	changeAxisButtonState { arg view;
		if(currentAxisButtonValue != 0, { // for every other case but 0
			joystickControlSetArray.at(currentAxisButtonValue - 1)
					.remove(this); // remove me from the control set
		});
		currentAxisButtonValue = view.value; // set the new currentAxisButtonValue
		if(currentAxisButtonValue != 0, { // for every other case but 0
			joystickControlSetArray.at(currentAxisButtonValue - 1)
					.add(this); // add me to the control set
		});
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	//setAxisButtonColors {}

	//plugIntoSynth {}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			sliderLength = 200, boxLength = 40, buttonLength = 20,
			currentBoxSide = \right, buttonSide = \left;
		var	offsets = nil; // an array of relative widget offsets

		// most common case first...
		if((currentBoxSide == \right) && (buttonSide == \left), {
			offsets = [0, buttonLength, buttonLength * 2,
					(buttonLength * 2) + boxLength,
					((buttonLength * 2) + boxLength) + sliderLength,
					(buttonLength * 2) + (boxLength * 2) + sliderLength];
			// shift all offsets according to gap width
			offsets = offsets.collect({ arg each, i;
				each = each + (i * gap); });

			lockButton.bounds_(Rect.new(xOff + offsets[0],
					yOff, buttonLength, width));
			axisButton.bounds_(Rect.new(xOff + offsets[1],
					yOff, buttonLength, width));
			leftBoxLabel.bounds_(Rect.new(xOff + offsets[2],
					yOff, boxLength, width));
			leftBox.bounds_(Rect.new(xOff + offsets[2],
					yOff, boxLength, width));
			sliderLabel.bounds_(Rect.new(xOff + offsets[3],
					yOff, sliderLength, width));
			slider.bounds_(Rect.new(xOff + offsets[3],
					yOff, sliderLength, width));
			rightBoxLabel.bounds_(Rect.new(xOff + offsets[4],
					yOff, boxLength, width));
			rightBox.bounds_(Rect.new(xOff + offsets[4],
					yOff, boxLength, width));
			currentBoxLabel.bounds_(Rect.new(xOff + offsets[5],
					yOff, boxLength, width));
			currentBox.bounds_(Rect.new(xOff + offsets[5],
					yOff, boxLength, width));
			(name ++ " " ++ "slider was drawn at [" ++
					xOff ++ ", " ++ yOff ++ "]").postln;
		}, {
			if((currentBoxSide == \right) && (buttonSide == \right), {
				offsets = [0, boxLength, boxLength + sliderLength,
						(boxLength * 2) + sliderLength,
						(boxLength * 3) + sliderLength,
						(boxLength * 3) + buttonLength + sliderLength];
				// shift all offsets according to gap width
				offsets = offsets.collect({ arg each, i;
					each = each + (i * gap); });

				leftBoxLabel.bounds_(Rect.new(xOff + offsets[0],
						yOff, boxLength, width));
				leftBox.bounds_(Rect.new(xOff + offsets[0],
						yOff, boxLength, width));
				sliderLabel.bounds_(Rect.new(xOff + offsets[1],
						yOff, sliderLength, width));
				slider.bounds_(Rect.new(xOff + offsets[1],
						yOff, sliderLength, width));
				rightBoxLabel.bounds_(Rect.new(xOff + offsets[2],
						yOff, boxLength, width));
				rightBox.bounds_(Rect.new(xOff + offsets[2],
						yOff, boxLength, width));
				currentBoxLabel.bounds_(Rect.new(xOff + offsets[3],
						yOff, boxLength, width));
				currentBox.bounds_(Rect.new(xOff + offsets[3],
						yOff, boxLength, width));
				axisButton.bounds_(Rect.new(xOff + offsets[4],
						yOff, buttonLength, width));
				lockButton.bounds_(Rect.new(xOff + offsets[5],
						yOff, buttonLength, width));
				(name ++ " " ++ "slider was drawn at [" ++
						xOff ++ ", " ++ yOff ++ "]").postln;
			}, {
				if((currentBoxSide == \left) && (buttonSide == \left), {
					offsets = [0, buttonLength, buttonLength * 2,
							(buttonLength * 2) + boxLength,
							(buttonLength * 2) + (boxLength * 2),
							(buttonLength * 2) + (boxLength * 2) + sliderLength];
					// shift all offsets according to gap width
					offsets = offsets.collect({ arg each, i;
						each = each + (i * gap); });

					lockButton.bounds_(Rect.new(xOff + offsets[0],
							yOff, buttonLength, width));
					axisButton.bounds_(Rect.new(xOff + offsets[1],
							yOff, buttonLength, width));
					currentBoxLabel.bounds_(Rect.new(xOff + offsets[2],
							yOff, boxLength, width));
					currentBox.bounds_(Rect.new(xOff + offsets[2],
							yOff, boxLength, width));
					leftBoxLabel.bounds_(Rect.new(xOff + offsets[3],
							yOff, boxLength, width));
					leftBox.bounds_(Rect.new(xOff + offsets[3],
							yOff, boxLength, width));
					sliderLabel.bounds_(Rect.new(xOff + offsets[4],
							yOff, sliderLength, width));
					slider.bounds_(Rect.new(xOff + offsets[4],
							yOff, sliderLength, width));
					rightBoxLabel.bounds_(Rect.new(xOff + offsets[5],
							yOff, boxLength, width));
					rightBox.bounds_(Rect.new(xOff + offsets[5],
							yOff, boxLength, width));
					(name ++ " " ++ "slider was drawn at [" ++
							xOff ++ ", " ++ yOff ++ "]").postln;
				}, {
					if((currentBoxSide == \left) && (buttonSide == \right), {
						offsets = [0, boxLength, boxLength * 2,
								(boxLength * 2) + sliderLength,
								(boxLength * 3) + sliderLength,
								(boxLength * 3) + buttonLength + sliderLength];
						// shift all offsets according to gap width
						offsets = offsets.collect({ arg each, i;
							each = each + (i * gap); });

						currentBoxLabel.bounds_(Rect.new(xOff + offsets[0],
								yOff, boxLength, width));
						currentBox.bounds_(Rect.new(xOff + offsets[0],
								yOff, boxLength, width));
						leftBoxLabel.bounds_(Rect.new(xOff + offsets[1],
								yOff, boxLength, width));
						leftBox.bounds_(Rect.new(xOff + offsets[1],
								yOff, boxLength, width));
						sliderLabel.bounds_(Rect.new(xOff + offsets[2],
								yOff, sliderLength, width));
						slider.bounds_(Rect.new(xOff + offsets[2],
								yOff, sliderLength, width));
						rightBoxLabel.bounds_(Rect.new(xOff + offsets[3],
								yOff, boxLength, width));
						rightBox.bounds_(Rect.new(xOff + offsets[3],
								yOff, boxLength, width));
						axisButton.bounds_(Rect.new(xOff + offsets[4],
								yOff, buttonLength, width));
						lockButton.bounds_(Rect.new(xOff + offsets[5],
								yOff, buttonLength, width));
						(name ++ " " ++ "slider was drawn at [" ++
								xOff ++ ", " ++ yOff ++ "]").postln;
					});
				});
			});
		});
	}
}