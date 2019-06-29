/*
 * David Thall
 * Joystick.sc
 * CG2 Project
 */

// Behaves like a 2D slider,
// but can be mapped to the X/Y Axis coordinates of any MappableSlider.
// Resolution Zooming is accomplished through resizing (h/v elasticity)
// Also contains an optional gate function when the mouse is clicked down and up!

// FIX: SCRangeSlider's action function was broken (would always call the action,
// even when not physically-manipulated).  Fixed by James Harkin (20040807).
// The same issue exists for SC2DSlider, so I'd better redo the action code for
// this as well!  I should fix the action here, making it update the other views
// value's. Then I should fix it in all other parts of my code!
// (note: this fix could require me to make forced calls...
// (e.g., in sliderX.action or sliderY.action, I may need to call the do
// functions on the joystickControlSetX and joystickControlSetY
// ControlSet's explicitly)

// Add: button at the edge (e.g., a lockButton to turn on/off parent window resizing)
// Add: argColor1, argColor2 to the constructor! (basically, a HiliteGradient)

// 79 chars
///////////////////////////////////////////////////////////////////////////////

Joystick {
	// state variables
	var	<parent = nil;			// ref to a parent SCWindow
	var	<name = nil;				// SCStaticText to display in Joystick
	var	<color = nil;				// blended background color
	// SCView's
	var	<slider2DLabel = nil;		// SCStaticText reference
	var	<slider2D = nil;			// SC2DSlider/SC2DTabletSlider reference
	var	<sliderX = nil;			// SCSlider reference
	var	<sliderY = nil;			// SCSlider reference
	var	<rangeSlider = nil;		// SCRangeSlider reference
	// Joystick ControlSets X & Y (external references)
	var	<joystickControlSetX = nil;	// Joystick ControlSet (x-axis)
	var	<joystickControlSetY = nil;	// Joystick ControlSet (y-axis)
	// .action function lists
	var	slider2DActionList = nil;			// slider2D List of actions
	var	slider2DMouseDownActionList = nil;	// slider2D List of actions
	var	slider2DMouseUpActionList = nil;		// slider2D List of actions
	var	sliderXActionList = nil;			// sliderX List of actions
	var	sliderYActionList = nil;			// sliderY List of actions
	var	rangeSliderActionList = nil;		// rangeSlider List of actions

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new Joystick
	*new { arg argParent, argName, argColor,
			argJoystickControlSetX, argJoystickControlSetY,
			hasGate = true;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		^super.new.initJoystick(argParent, argName, argColor,
				argJoystickControlSetX, argJoystickControlSetY,
				hasGate);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the Joystick instance into a consistent state
	initJoystick { arg argParent, argName, argColor,
			argJoystickControlSetX, argJoystickControlSetY,
			hasGate;
		this.initJoystickStateVariables(argParent, argName, argColor,
				argJoystickControlSetX, argJoystickControlSetY);
		this.createJoystickGUIViews(hasGate);
		this.addJoystickActions(hasGate);
	}

	// initialize all the internal state variables
	initJoystickStateVariables { arg argParent, argName, argColor,
			argJoystickControlSetX, argJoystickControlSetY;

		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });
		color = if(argColor.isNil, { Color.new; }, { argColor; });
		joystickControlSetX = argJoystickControlSetX;
		joystickControlSetY = argJoystickControlSetY;

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("color -> " ++ color).postln;
		("joystickControlSetX -> " ++ joystickControlSetX).postln;
		("joystickControlSetY -> " ++ joystickControlSetY).postln;
	}

	// create and initialize the internal gui views
	createJoystickGUIViews { arg hasGate;
		// defaults
		var	slider2DLabelFont = nil;
		var	slider2DLabelAlign = nil;
		var	slider2DLabelStringColor = nil;
		var	slider2DCanFocus = nil;
		var	slider2DBackground = nil;
		var	slider2DKnobColor = nil;
		var	sliderCanFocus = nil;
		var	sliderBackground = nil;
		var	sliderKnobColor = nil;
		var	rangeSliderCanFocus = nil;
		var	rangeSliderBackground = nil;
		var	rangeSliderKnobColor = nil;

		slider2DLabelFont = Font.new("Helvetica", 12);
		slider2DLabelAlign = \center;
		slider2DLabelStringColor = Color.black.alpha_(0.7);
		slider2DCanFocus = false;
		// resolution = 854 horizontal lines across, top-down
		slider2DBackground = HiliteGradient.new(Color.black,
				color, \v, Window.screenBounds.height, 1);
		slider2DKnobColor = Color.white;
		sliderCanFocus = false;
		sliderBackground = Color.clear;
		sliderKnobColor = color;
		rangeSliderCanFocus = false;
		rangeSliderBackground = Color.clear;
		rangeSliderKnobColor = color;

		// slider2DLabel
		slider2DLabel = SCStaticText.new(parent);
		slider2DLabel.string_(name.asString);
		slider2DLabel.font_(slider2DLabelFont);
		slider2DLabel.align_(slider2DLabelAlign);
		slider2DLabel.stringColor_(slider2DLabelStringColor);

		("slider2DLabel -> " ++ slider2DLabel).postln;
		("slider2DLabel.string -> " ++ slider2DLabel.string).postln;
		("slider2DLabel.font -> " ++ slider2DLabel.font).postln;
		//("slider2DLabel.align -> " ++ slider2DLabel.align).postln;
		("slider2DLabel.stringColor -> " ++ slider2DLabel.stringColor).postln;

		// slider2D
		if(hasGate, {
			slider2D = SC2DTabletSlider.new(parent);
		}, {
			slider2D = SC2DSlider.new(parent);
		});
		slider2D.canFocus_(slider2DCanFocus);
		slider2D.background_(slider2DBackground);
		slider2D.knobColor_(slider2DKnobColor);
		slider2D.x_(0);
		slider2D.y_(0);
		//slider2D.value_(0); // object does not understand!!!
		slider2D.step_(0); // infinite precision
		slider2D.action_({ arg slider2D;
			slider2DActionList.do({ arg action;
				action.value(slider2D); // evaluate
			});
		});
		if(hasGate, {
			slider2D.mouseDownAction_({ arg slider2D, x, y, pressure;
				slider2DMouseDownActionList.do({ arg action;
					action.value(slider2D, x, y, pressure); // evaluate
				});
			});
			slider2D.mouseUpAction_({ arg slider2D, x, y, pressure;
				slider2DMouseUpActionList.do({ arg action;
					action.value(slider2D, x, y, pressure); // evaluate
				});
			});
		});

		("slider2D -> " ++ slider2D).postln;
		("slider2D.canFocus -> " ++ slider2D.canFocus).postln;
		("slider2D.background -> " ++ slider2D.background).postln;
		("slider2D.knobColor -> " ++ slider2D.knobColor).postln;
		("slider2D.x -> " ++ slider2D.x).postln;
		("slider2D.y -> " ++ slider2D.y).postln;
		("slider2D.step -> " ++ slider2D.step).postln;
		("slider2D.action -> " ++ slider2D.action).postln;
		if(hasGate, {
			("slider2D.mouseDownAction -> " ++ slider2D.mouseDownAction).postln;
			("slider2D.mouseUpAction -> " ++ slider2D.mouseUpAction).postln;
		});

		// sliderX (w/o a ControlSpec,
		// it outputs [0,1] linear w/ infinite precision)
		sliderX = SCSlider.new(parent);
		sliderX.canFocus_(sliderCanFocus);
		sliderX.background_(sliderBackground);
		sliderX.knobColor_(sliderKnobColor);
		sliderX.value_(0); // always the default
		sliderX.step_(0); // infinite precision
		sliderX.action_({ arg sliderX;
			sliderXActionList.do({ arg action;
				action.value(sliderX); // evaluate
			});
		});

		("sliderX -> " ++ sliderX).postln;
		("sliderX.canFocus -> " ++ sliderX.canFocus).postln;
		("sliderX.background -> " ++ sliderX.background).postln;
		("sliderX.knobColor -> " ++ sliderX.knobColor).postln;
		("sliderX.value -> " ++ sliderX.value).postln;
		("sliderX.step -> " ++ sliderX.step).postln;
		("sliderX.action -> " ++ sliderX.action).postln;

		// sliderY (w/o a ControlSpec,
		// it outputs [0,1] linear w/ infinite precision)
		sliderY = SCSlider.new(parent);
		sliderY.canFocus_(sliderCanFocus);
		sliderY.background_(sliderBackground);
		sliderY.knobColor_(sliderKnobColor);
		sliderY.value_(0); // always the default
		sliderY.step_(0); // infinite precision
		sliderY.action_({ arg sliderY;
			sliderYActionList.do({ arg action;
				action.value(sliderY); // evaluate
			});
		});

		("sliderY -> " ++ sliderY).postln;
		("sliderY.canFocus -> " ++ sliderY.canFocus).postln;
		("sliderY.background -> " ++ sliderY.background).postln;
		("sliderY.knobColor -> " ++ sliderY.knobColor).postln;
		("sliderY.value -> " ++ sliderY.value).postln;
		("sliderY.step -> " ++ sliderY.step).postln;
		("sliderY.action -> " ++ sliderY.action).postln;

		// rangeSlider
		rangeSlider = SCRangeSlider.new(parent);
		rangeSlider.canFocus_(rangeSliderCanFocus);
		rangeSlider.background_(rangeSliderBackground);
		rangeSlider.knobColor_(rangeSliderKnobColor);
		rangeSlider.lo_(0);
		rangeSlider.hi_(1);
		rangeSlider.range_(1);
		//rangeSlider.value; // object does not understand!!!
		rangeSlider.step_(0); // infinite precision
		rangeSlider.action_({ arg rangeSlider;
			rangeSliderActionList.do({ arg action;
				action.value(rangeSlider); // evaluate
			});
		});

		("rangeSlider -> " ++ rangeSlider).postln;
		("rangeSlider.canFocus -> " ++ rangeSlider.canFocus).postln;
		("rangeSlider.background -> " ++ rangeSlider.background).postln;
		("rangeSlider.knobColor -> " ++ rangeSlider.knobColor).postln;
		("rangeSlider.lo -> " ++ rangeSlider.lo).postln;
		("rangeSlider.hi -> " ++ rangeSlider.hi).postln;
		("rangeSlider.range -> " ++ rangeSlider.range).postln;
		("rangeSlider.step -> " ++ rangeSlider.step).postln;
		("rangeSlider.action -> " ++ rangeSlider.action).postln;
	}

	// define & register the action event functions...add them to actionList's
	addJoystickActions { arg hasGate;
		// action functions to append to action list
		var	slider2DAction = nil;
		var	slider2DMouseDownAction = nil;
		var	slider2DMouseUpAction = nil;
		var	sliderXAction = nil;
		var	sliderYAction = nil;
		var	rangeSliderAction = nil;

		// define actions
		slider2DAction = { arg slider2D;
			// update the value of any connected MappableSliders
			this.sendUpdatesFromAxisX(slider2D);
			this.sendUpdatesFromAxisY(slider2D);

			// update the value of sliderX and sliderY (follow movement)
			sliderX.value_(slider2D.x);
			sliderY.value_(slider2D.y);

			// update the range of the rangeSlider
			this.updateRangeSlider;

			//("[x, y] -> " ++ "[" ++
			//		slider2D.x ++ ", " ++ slider2D.y ++ "]").postln;
		};

		if(hasGate, {
			slider2DMouseDownAction = { arg slider2D, x, y, pressure;
				slider2D.doAction;
				("[x, y, pressure] -> " ++ "[" ++
						x ++ ", " ++ y ++ ", " ++ pressure ++ "]").postln;
			};

			slider2DMouseUpAction = { arg slider2D, x, y, pressure;
				("[x, y, pressure] -> " ++ "[" ++
						x ++ ", " ++ y ++ ", " ++ pressure ++ "]").postln;
			};
		});

		sliderXAction = { arg sliderX;
			// update the Joysticks's 'x' coordinate (position)
			slider2D.x_(sliderX.value);

			// update the range of the rangeSlider
			this.updateRangeSlider;
		};

		sliderYAction = { arg sliderY;
			// update the Joysticks's 'y' coordinate (position)
			slider2D.y_(sliderY.value);

			// update the range of the rangeSlider
			this.updateRangeSlider;
		};

		rangeSliderAction = { arg rangeSlider;
			//("rangeSlider [lo, hi] -> " ++ "[" ++
			//		rangeSlider.lo ++ ", " ++ rangeSlider.hi ++ "]").postln;

			// update the Joystick's 'x' and 'y' coordinates (positions)
			// according to the rangeSlider's range
			//slider2D.x_(rangeSlider.lo + 0.5 - (slider2D.y * 0.5));
			//slider2D.y_(rangeSlider.hi - rangeSlider.lo - 1);
		};

		// create new actionLists
		slider2DActionList = List.new;
		if(hasGate, {
			slider2DMouseDownActionList = List.new;
			slider2DMouseUpActionList = List.new;
		});
		sliderXActionList = List.new;
		sliderYActionList = List.new;
		rangeSliderActionList = List.new;

		// add actions to actionLists
		slider2DActionList.add(slider2DAction);
		if(hasGate, {
			slider2DMouseDownActionList.add(slider2DMouseDownAction);
			slider2DMouseUpActionList.add(slider2DMouseUpAction);
		});
		sliderXActionList.add(sliderXAction);
		sliderYActionList.add(sliderYAction);
		rangeSliderActionList.add(rangeSliderAction);
	}

	// update all currently registered MappableSlider's
	// connected to the x-axis
	sendUpdatesFromAxisX { arg slider2D;
		joystickControlSetX.do({ arg view;
			view.slider.valueAction_(slider2D.x);
		});
	}

	// update all currently registered MappableSlider's
	// connected to the y-axis
	sendUpdatesFromAxisY { arg slider2D;
		joystickControlSetY.do({ arg view;
			view.slider.valueAction_(slider2D.y);
		});
	}

	// update the width/range of the rangeSlider
	updateRangeSlider {
		// same as: slider2D.x - 0.5 + (slider2D.y / 2)
		rangeSlider.lo_(slider2D.x - 0.5 + (slider2D.y * 0.5));
		// same as: (2 * slider2D.x) - rangeSlider.lo
		rangeSlider.hi_(1 - slider2D.y + rangeSlider.lo);
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// setColor {}

	// draw the widgets on the screen within a parent window
	// NOTE: Joystick SCView's will be restrained to within the bounds
	// of the parent SCWindow! (for resizing)
	draw { arg xOff = 5, yOff = 5,
			sliderWidth = 20, gap = 5,
			sliderXSide = \bottom, sliderYSide = \right, rangeSliderSide = \top,
			elastic = true;
		var	parentWidth = nil;
		var	parentHeight = nil;

		var	slider2DWidth = nil;
		var	slider2DHeight = nil;

		// get the parent SCWindow's width & height
		parentWidth = parent.bounds.width;
		parentHeight = parent.bounds.height;

		("parentWidth -> " ++ parentWidth).postln;
		("parentHeight -> " ++ parentHeight).postln;

		// calculate the width & height for the 2DSlider
		slider2DWidth = parentWidth - (xOff * 2) - sliderWidth - gap;
		slider2DHeight = parentHeight - (yOff * 2) - sliderWidth - gap;

		("slider2DWidth -> " ++ slider2DWidth).postln;
		("slider2DHeight -> " ++ slider2DHeight).postln;

		slider2DLabel.bounds_(Rect.new(xOff,
				yOff, slider2DWidth, slider2DHeight));
		slider2D.bounds_(Rect.new(xOff,
				yOff, slider2DWidth, slider2DHeight));
		sliderX.bounds_(Rect.new(xOff,
				yOff + slider2DHeight + gap, slider2DWidth, sliderWidth));
		sliderY.bounds_(Rect.new(xOff + slider2DWidth + gap,
				yOff, sliderWidth, slider2DHeight));
		rangeSlider.bounds_(Rect.new(xOff,
				0, slider2DWidth, yOff));

		if(elastic, {
			slider2DLabel.resize_(5); 	// h-elastic, v-elastic
			slider2D.resize_(5);		// h-elastic, v-elastic
			sliderX.resize_(8);		// h-elastic, fixed to bottom
			sliderY.resize_(6);		// fixed to right, v-elastic
			rangeSlider.resize_(2);		// h-elastic, fixed to top
		});
	}
}