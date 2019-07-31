/*
 * David Thall
 * ParameterSlider.sc
 * CG2 Project
 */

// Extends MappableSlider, implements other widget views

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ParameterSlider : MappableSlider {
	// SCView's
	var	<rangeClipButton = nil; 		// SCButton reference (passRejBut???)
	// .action function lists
	var	rangeClipButtonActionList = nil;	// rangeClipButton List of actions

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new ParameterSlider
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
					.initParameterSlider;
		}, {
			^super.new(argParent,
					argJoystickControlSetArray, argTiltControlSet,
					argName,
					argMin, argMax,
					argLeft, argRight,
					argWarp, argStep,
					argDefault, argUnits)
					.initParameterSlider;
		});
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ParameterSlider instance into a consistent state
	initParameterSlider {
		this.createParameterSliderGUIViews;
		this.addParameterSliderActions;
	}

	// create and initialize the internal gui views
	createParameterSliderGUIViews {
		// defaults
		var	buttonCanFocus = nil;
		var	buttonFont = nil;

		var	buttonDataList = nil;

		var	buttonStates = nil;

		buttonCanFocus = false;
		buttonFont = Font.new("Helvetica", 10);

		buttonDataList = ["",
				Color.clear, [Color.clear, Color.blue.alpha_(0.2)]];

		buttonStates = Array.fill(2, { arg i;
				[buttonDataList[0],
				buttonDataList[1],
				buttonDataList[2][i]]; });

		// rangeClipButton
		rangeClipButton = Button.new(parent);
		rangeClipButton.canFocus_(buttonCanFocus);
		rangeClipButton.font_(buttonFont);
		rangeClipButton.states_(buttonStates);
		rangeClipButton.value_(0);
		rangeClipButton.action_({ arg rangeClipButton;
			rangeClipButtonActionList.do({ arg action;
				action.value(rangeClipButton); // evaluate
			});
		});

		("rangeClipButton -> " ++ rangeClipButton).postln;
		("rangeClipButton.canFocus -> " ++ rangeClipButton.canFocus).postln;
		("rangeClipButton.font -> " ++ rangeClipButton.font).postln;
		("rangeClipButton.states -> " ++ rangeClipButton.states).postln;
		("rangeClipButton.value -> " ++ rangeClipButton.value).postln;
		("rangeClipButton.action -> " ++ rangeClipButton.action).postln;
	}

	// define & register the action event functions...add them to actionList's
	addParameterSliderActions {
		// action functions to append to action list
		var	rangeClipButtonAction = nil;

		// define actions
		rangeClipButtonAction = { arg rangeClipButton;
			if(rangeClipButton.value == 0, { // unclipped
				(name ++ " " ++ "rangeClipButton -> " ++
						[rangeClipButton.value,
						rangeClipButton.states[rangeClipButton.value][0]]).postln;
			});
			if(rangeClipButton.value == 1, { // clipped
				(name ++ " " ++ "rangeClipButton -> " ++
						[rangeClipButton.value,
						rangeClipButton.states[rangeClipButton.value][0]]).postln;
			});
		};

		// create new actionLists
		rangeClipButtonActionList = List.new;

		// add actions to actionLists
		rangeClipButtonActionList.add(rangeClipButtonAction);
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	//plugIntoSynth {}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
			width = 20, gap = 5,
			sliderLength = 200, boxLength = 40, buttonLength = 20;
		var	offsets = nil; // an array of relative widget offsets

		slider.orientation_(\horizontal);

		offsets = [0, boxLength, boxLength * 2,
				(boxLength * 2) + sliderLength,
				(boxLength * 3) + sliderLength,
				(boxLength * 3) + sliderLength + buttonLength,
				(boxLength * 3) + sliderLength + (buttonLength * 2)];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i;
			each = each + (i * gap); });

		currentBoxLabel.bounds_(Rect.new(xOff + offsets[0],
			yOff, boxLength, width)).front.acceptsMouse_(false);
		currentBox.bounds_(Rect.new(xOff + offsets[0],
				yOff, boxLength, width));
		leftBoxLabel.bounds_(Rect.new(xOff + offsets[1],
				yOff, boxLength, width)).front.acceptsMouse_(false);
		leftBox.bounds_(Rect.new(xOff + offsets[1],
				yOff, boxLength, width));
		sliderLabel.bounds_(Rect.new(xOff + offsets[2],
				yOff, sliderLength, width)).front.acceptsMouse_(false);
		slider.bounds_(Rect.new(xOff + offsets[2],
				yOff, sliderLength, width));
		rightBoxLabel.bounds_(Rect.new(xOff + offsets[3],
				yOff, boxLength, width)).front.acceptsMouse_(false);
		rightBox.bounds_(Rect.new(xOff + offsets[3],
				yOff, boxLength, width));
		axisButton.bounds_(Rect.new(xOff + offsets[4],
				yOff, buttonLength, width));
		lockButton.bounds_(Rect.new(xOff + offsets[5],
				yOff, buttonLength, width));
		rangeClipButton.bounds_(Rect.new(xOff + offsets[6],
				yOff, buttonLength, width));
		(name ++ " " ++ "slider was drawn at [" ++
				xOff ++ ", " ++ yOff ++ "]").postln;
	}
}