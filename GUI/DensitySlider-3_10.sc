/*
 * David Thall
 * DensitySlider.sc
 * CG2 Project
 */

// Extends MappableSlider (or perhaps ParameterSlider ???), implements other widget views & options

// Fix: The extKeyBox range should be limited to the min & max values of this DensitySlider's spec!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

DensitySlider : ParameterSlider {
	// GUI Widgets
	var	<trigPopUpMenu, <extKeyBoxLabel, <extKeyBox; // // maybe extkeyBox should be sidechainBox ???
	// a list of all .action functions to implement here and append to in subclasses
	var	trigPopUpMenuActionList, extKeyBoxActionList;

	// create and name a new ParameterSlider
	*new { arg argParent,
		argJoystickControlSetArray, argTiltControlSet,
		argName,
		argMin = 0, argMax = 1,
		argLeft = 0, argRight = 1,
		argWarp = \lin, argStep = 0,
		argDefault = 0, argUnits;

		^super.new(argParent,
			argJoystickControlSetArray, argTiltControlSet,
			argName,
			argMin, argMax,
			argLeft, argRight,
			argWarp, argStep,
			argDefault, argUnits)
		.initDensitySlider;

	}

	initDensitySlider {
		var	leftBoxAction, sliderAction, rightBoxAction, currentBoxAction; // actions to append to action list
		var	trigPopUpMenuAction, extKeyBoxAction; // actions to append to action list

		/* Declarations */

		// trigPopUpMenu
		trigPopUpMenu = PopUpMenu.new(parent);
		trigPopUpMenu.canFocus = false;
		trigPopUpMenu.font = Font.new("Helvetica", 10);
		trigPopUpMenu.background = HiliteGradient.new(Color.gray, Color.black, \h, 256, 0);
		trigPopUpMenu.stringColor = Color.white;

		// extKeyBoxLabel
		extKeyBoxLabel = StaticText.new(parent);
		extKeyBoxLabel.string = units.asString;
		extKeyBoxLabel.font = Font.new("Helvetica", 12);
		extKeyBoxLabel.align = \right;
		extKeyBoxLabel.stringColor = Color.white;
		// extKeyBox
		extKeyBox = NumberBox.new(parent);
		extKeyBox.canFocus = true;
		extKeyBox.font = Font.new("Helvetica", 12);
		extKeyBox.align = \left;
		extKeyBox.background = Color.black;
		extKeyBox.normalColor = Color.white;
		extKeyBox.typingColor = Color.green;

		/* Initializations */

		trigPopUpMenu.items = [
			"synchronous", "asynchronous", "external key", "key follow", "audio input"
		];
		extKeyBox.enabled = false; // NOTE: This is only true if the initial state of the popUpMenu is != 3!!!
		extKeyBox.value = 1; // ??? super.default; ???
		extKeyBox.step = 1; // ??? super.step; ???

		/* Define Actions */

		// Combine all of these actions bodies into a single method call!!! (They are all the same!)

		// implements extKeyBox value following
		leftBoxAction = { arg leftBox;
			if(trigPopUpMenu.value == 3, { // key follow
				extKeyBox.value = currentBox.value;
				(name ++ " " ++ "extKeyBox.value -> " ++ extKeyBox.value).postln;
			});
		};

		// implements extKeyBox value following
		sliderAction = { arg slider;
			if(trigPopUpMenu.value == 3, { // key follow
				extKeyBox.value = currentBox.value;
				(name ++ " " ++ "extKeyBox.value -> " ++ extKeyBox.value).postln;
			});
		};

		// implements extKeyBox value following
		rightBoxAction = { arg rightBox;
			if(trigPopUpMenu.value == 3, { // key follow
				extKeyBox.value = currentBox.value;
				(name ++ " " ++ "extKeyBox.value -> " ++ extKeyBox.value).postln;
			});
		};

		// implements extKeyBox value following
		currentBoxAction = { arg currentBox;
			if(trigPopUpMenu.value == 3, { // key follow
				extKeyBox.value = currentBox.value;
				(name ++ " " ++ "extKeyBox.value -> " ++ extKeyBox.value).postln;
			});
		};

		trigPopUpMenuAction = { arg trigPopUpMenu;
			(name ++ " " ++ "trigPopUpMenu -> " ++ [trigPopUpMenu.value, trigPopUpMenu.items[trigPopUpMenu.value].value]).postln;
			if((trigPopUpMenu.value == 2) || (trigPopUpMenu.value == 3), { // external key or key follow
				extKeyBox.enabled = true;
			}, {
				extKeyBox.enabled = false;
			});
			("extKeyBox.enabled -> " ++ extKeyBox.enabled).postln;
		};

		extKeyBoxAction = { arg extKeyBox;
			// first, check to make sure the numberbox value is in range
			if(extKeyBox.value < min, { extKeyBox.value = min; });
			if(extKeyBox.value > max, { extKeyBox.value = max; });
			(name ++ " " ++ "extKeyBox.value -> " ++ extKeyBox.value).postln;
		};

		/* Add Actions to ActionList */

		leftBoxActionList.add(leftBoxAction);
		sliderActionList.add(sliderAction);
		rightBoxActionList.add(rightBoxAction);
		currentBoxActionList.add(currentBoxAction);

		trigPopUpMenuActionList = List.new;
		extKeyBoxActionList = List.new;

		trigPopUpMenuActionList.add(trigPopUpMenuAction);
		extKeyBoxActionList.add(extKeyBoxAction);

		/* Actions */

		leftBox.action = { arg leftBox;
			//("BEG ********** " ++ name ++ " " ++ "leftBox.action **********").postln;
			leftBoxActionList.do({ arg action;
				action.value(leftBox); // evaluate
			});
			//("END ********** " ++ name ++ " " ++ "leftBox.action **********\n").postln;
		};

		slider.action = { arg slider;
			//("BEG ********** " ++ name ++ " " ++ "slider.action **********").postln;
			sliderActionList.do({ arg action;
				action.value(slider); // evaluate
			});
			//("END ********** " ++ name ++ " " ++ "slider.action **********\n").postln;
		};

		rightBox.action = { arg rightBox;
			//("BEG ********** " ++ name ++ " " ++ "rightBox.action **********").postln;
			rightBoxActionList.do({ arg action;
				action.value(rightBox); // evaluate
			});
			//("END ********** " ++ name ++ " " ++ "rightBox.action **********\n").postln;
		};

		currentBox.action = { arg currentBox;
			//("BEG ********** " ++ name ++ " " ++ "currentBox.action **********").postln;
			currentBoxActionList.do({ arg action;
				action.value(currentBox); // evaluate
			});
			//("END ********** " ++ name ++ " " ++ "currentBox.action **********\n").postln;
		};

		trigPopUpMenu.action = { arg trigPopUpMenu;
			("BEG ********** " ++ name ++ " " ++ "trigPopUpMenu.action **********").postln;
			trigPopUpMenuActionList.do({ arg action;
				action.value(trigPopUpMenu); // evaluate
			});
			("END ********** " ++ name ++ " " ++ "trigPopUpMenu.action **********\n").postln;
		};

		extKeyBox.action = { arg extKeyBox;
			("BEG ********** " ++ name ++ " " ++ "extKeyBox.action **********").postln;
			extKeyBoxActionList.do({ arg action;
				action.value(extKeyBox); // evaluate
			});
			("END ********** " ++ name ++ " " ++ "extKeyBox.action **********\n").postln;
		};
	}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
		height = 20, gap = 5,
		sliderLength = 200, boxLength = 40, buttonLength = 20,
		optionPopUpMenuLength = 85, optionBoxLength = 40;
		var	offsets;

		slider.orientation_(\horizontal);

		// an array of default widget offsets (in relation to neighboring widgets)
		offsets = [0, boxLength, boxLength * 2, (boxLength * 2) + sliderLength,
			(boxLength * 3) + sliderLength,
			(boxLength * 3) + sliderLength + buttonLength,
			(boxLength * 3) + sliderLength + (buttonLength * 2),
			(boxLength * 3) + sliderLength + (buttonLength * 3),
			(boxLength * 3) + sliderLength + (buttonLength * 3) + optionPopUpMenuLength];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i; each = each + (i * gap); });

		currentBoxLabel.bounds = Rect.new(xOff + offsets[0], yOff, boxLength, height);
		currentBox.bounds = Rect.new(xOff + offsets[0], yOff, boxLength, height);
		leftBoxLabel.bounds = Rect.new(xOff + offsets[1], yOff, boxLength, height);
		leftBox.bounds = Rect.new(xOff + offsets[1], yOff, boxLength, height);
		sliderLabel.bounds = Rect.new(xOff + offsets[2], yOff, sliderLength, height);
		slider.bounds = Rect.new(xOff + offsets[2], yOff, sliderLength, height);
		rightBoxLabel.bounds = Rect.new(xOff + offsets[3], yOff, boxLength, height);
		rightBox.bounds = Rect.new(xOff + offsets[3], yOff, boxLength, height);
		axisButton.bounds = Rect.new(xOff + offsets[4], yOff, buttonLength, height);
		lockButton.bounds = Rect.new(xOff + offsets[5], yOff, buttonLength, height);
		rangeClipButton.bounds = Rect.new(xOff + offsets[6], yOff, buttonLength, height);
		trigPopUpMenu.bounds = Rect.new(xOff + offsets[7], yOff, optionPopUpMenuLength, height);
		extKeyBoxLabel.bounds = Rect.new(xOff + offsets[8], yOff, optionBoxLength, height);
		extKeyBox.bounds = Rect.new(xOff + offsets[8], yOff, optionBoxLength, height);
		(name ++ " " ++ "slider was drawn at [" ++ xOff ++ ", " ++ yOff ++ "]").postln;
	}
}