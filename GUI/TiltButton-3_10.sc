/*
 * David Thall
 * TiltButton.sc
 * CG2 Project
 */

// A TiltButton contains a single instance of Tilt

// When clicked, it will perform the current instance of Tilt (according to some input parameters passed in from gui widgets)

// 79 chars
///////////////////////////////////////////////////////////////////////////////

TiltButton {
	// Name & Location & Type
	var	<parent, <name;
	// Tilt
	var	<>tilt;
	// GUI Widgets
	var	<button;

	// create and name a new TiltButton
	*new { arg argParent, argName, argTilt;
		^super.new.initTiltButton(argParent, argName, argTilt);
	}

	initTiltButton { arg argParent, argName, argTilt;

		parent = argParent;
		name = argName;
		tilt = argTilt;

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("tilt -> " ++ tilt).postln;

		/* Declarations */

		// button
		button = SCButton.new(parent);
		button.canFocus = false;
		button.font = Font.new("Helvetica", 10);

		/* Initializations */

		button.states = [
			[name.asString, Color.black, Color.white] // pink = Color.new255(255, 192, 203)
		];

		/* Actions */

		button.action = { arg button;
			("BEG ********** " ++ name ++ " " ++ "button.action **********").postln;

			tilt.start;

			("END ********** " ++ name ++ " " ++ "button.action **********\n").postln;
		};
	}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5, width = 40, height = 20;
		button.bounds_(Rect.new(xOff, yOff, width, height));
	}
}