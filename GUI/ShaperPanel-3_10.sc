/*
 * David Thall
 * ShaperPanel.sc
 * CG2 Project
 */

// ShaperPanel is a panel that exists within a SCWindow.
// It contains a growable List of ShaperMatrices (up to 10), with a
// bypassButton that has both an on/off indicator and a peakMeter/peakLED.
// The ShaperPanel also contains a ShaperNamer that allows the user to input
// the name of a ShaperMatrix.  At the bottom is an adderButton (+/-) for
// expanding the list and view as necessary.

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ShaperPanel {
	// state variables
	var	<parent = nil;			// ref to a parent SCWindow
	var	<name = nil;				// SCStaticText to display in SCTextField
	// SCView's
	var	<bypassButton = nil;		// SCButton reference
	var	<peakMeterRangeSlider = nil;// SCRangeSlider reference
	var	<nameTextField = nil;		// SCTextField reference
	var 	<matrixAdderButton = nil;	// SCButton reference
	// ShaperMatrix Array
	var	<>shaperMatrixArray;		// array of Shaper Matrices

	// create and name a new ShaperPanel
	*new { arg argParent, argName;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		^super.new.initShaperPanel(argParent, argName);
	}

	initShaperPanel { arg argParent, argName;

		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
	}
}