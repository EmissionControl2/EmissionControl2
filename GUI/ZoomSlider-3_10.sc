/*
 * David Thall
 * ZoomSlider.sc
 * CG2 Project
 */

// Behaves like a normal slider, but has resolution zooming capablities
// Special case: A slider can be created using an \exp Control Spec with '0'

// Fix: Doesn't seem to support a ControlSpec with a CurveWarp
// Fix: Doesn't support a step == 0 (warn if set at init time)
// Update: All subclass draw methods should inherit functionality (super.draw)
// Update: method doZoomSliderSliderAction probably not needed
// Update: set up the state of the object in a single method
// Update: doZoomSliderNumBoxAction can be renamed doZoomNumBoxAction
// Add: Create a classvar verboseMode/debugMode for printing comments
// Add: Check if the slider or a numBox has changed before calling its action
// Add: background color specified in the SCStaticTextField constructors
// Add: single line comments in the constructor

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ZoomSlider {
	classvar	<>restrictRange = true;	// range checking on/off for currentBox
	classvar	offset = 1;			// offset for calc \exp curve from '0'
	classvar	minStep = 1e-14; 		// minimum step size (0.00000000000001)

	// state variables
	var	<parent = nil;			// ref to a parent SCWindow
	var	<name = nil;				// SCStaticText to display in SCSlider
	var	<min = nil;				// lowest value for all numBoxes
	var	<max = nil;				// highest value for all numBoxes
	var	<left = nil;				// left-most value in current zoom range
	var	<right = nil;				// right-most value in current zoom range
	var	<warp = nil;				// curvature function applied to range
	var	<step = nil;				// quantization of values in range
	var	<default = nil;			// starting value
	var	<units = nil;				// SCStaticText to display in SCNumberBox's

	// ControlSpec
	var	<spec;					// ControlSpec reference

	// SCView's
	var	<sliderLabel = nil;		// SCStaticText reference
	var	<slider = nil;			// SCSlider refererence
	var	<leftBoxLabel = nil;		// SCStaticText reference
	var	<leftBox = nil;			// SCNumberBox2 reference
	var	<rightBoxLabel = nil;		// SCStaticText reference
	var	<rightBox = nil;			// SCNumberBox2 reference
	var	<currentBoxLabel = nil;		// SCStaticText reference
	var	<currentBox = nil;			// SCNumberBox2 reference

	// .action function lists
	var	sliderActionList = nil;		// slider List of actions
	var	leftBoxActionList = nil;	// leftBox List of actions
	var	rightBoxActionList = nil;	// rightBox List of actions
	var	currentBoxActionList = nil;	// currentBox List of actions

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create and name a new ZoomSlider
	*new { arg argParent, argName,
			argMin = 0, argMax = 1,
			argLeft = 0, argRight = 1,
			argWarp = \lin, argStep = 0.001,
			argDefault = 0, argUnits;
		if(argParent.isNil, {
			(this.asString ++ ": " ++
					"A parent window must be provided!").postln;
			^nil;
		});
		^super.new.initZoomSlider(argParent, argName,
				argMin, argMax,
				argLeft, argRight,
				argWarp, argStep,
				argDefault, argUnits);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ZoomSlider instance into a consistent state
	initZoomSlider { arg argParent, argName,
			argMin, argMax,
			argLeft, argRight,
			argWarp, argStep,
			argDefault, argUnits;
		this.initZoomSliderStateVariables(argParent, argName,
				argMin, argMax,
				argLeft, argRight,
				argWarp, argStep,
				argDefault, argUnits);
		this.createZoomSliderControlSpec(left + offset, right + offset,
				warp, step, default);
		this.createZoomSliderGUIViews;
		this.addZoomSliderActions; // register actions
	}

	// initialize all the internal state variables
	initZoomSliderStateVariables { arg argParent, argName,
			argMin, argMax,
			argLeft, argRight,
		 	argWarp, argStep,
		 	argDefault, argUnits;
		parent = argParent;
		name = if(argName.isNil, { "" }, { argName.asString; });
		min = argMin.min(argMax).round(argStep);
		max = argMax.max(argMin).round(argStep);
		left = argLeft.clip(min, max).round(argStep);
		right = argRight.clip(min, max).round(argStep);
		warp = argWarp; // check if this is a type of CurveWarp!
		step = argStep.max(minStep); // limit min step size!
		default = if(left <= right, { // if out-of-range, clip w/in range
			argDefault.clip(left, right).round(step);
		}, {
			argDefault.clip(right, left).round(step);
		});
		units = if(argUnits.isNil, { "" }, { argUnits.asString; });

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("min -> " ++ min).postln;
		("max -> " ++ max).postln;
		("left -> " ++ left).postln;
		("right -> " ++ right).postln;
		("warp -> " ++ warp).postln;
		("step -> " ++ step).postln;
		("default -> " ++ default).postln;
		("units -> " ++ units).postln;
	}

	// use the internal state variables to initialize a new ControlSpec
	createZoomSliderControlSpec { arg argMinVal, argMaxVal,
			argWarp, argStep, argDefault;
		spec = ControlSpec.new(left + offset, right + offset,
				warp, step, default); // default = default + offset ???

		("spec -> " ++ spec).postln;
		("spec.minval -> " ++ spec.minval).postln;
		("spec.maxval -> " ++ spec.maxval).postln;
		("spec.warp -> " ++ spec.warp).postln;
		("spec.step -> " ++ spec.step).postln;
		("spec.default -> " ++ spec.default).postln;
	}

	// create and initialize the internal gui views
	createZoomSliderGUIViews {
		// default SCView 'look & feel'
		var	sliderLabelFont = nil;
		var	sliderLabelAlign = nil;
		var	sliderLabelStringColor = nil;
		var	sliderCanFocus = nil;
		var	sliderBackground = nil;
		var	sliderKnobColor = nil;
		var	numBox2LabelFont = nil;
		var	numBox2LabelAlign = nil;
		var	numBox2LabelStringColor = nil;
		var	numBox2CanFocus = nil;
		var	numBox2Font = nil;
		var	numBox2Align = nil;
		var	numBox2BoxColor = nil;
		var	numBox2DefaultStringColor = nil;
		var	numBox2EnterStringColor = nil;
		var	numBox2ShiftKeyScaler = nil;

		sliderLabelFont = Font.new("Helvetica", 12);
		sliderLabelAlign = \center;
		sliderLabelStringColor = Color.black.alpha_(0.7);
		sliderCanFocus = false;
		sliderBackground = HiliteGradient(Color.new255(207, 214, 224),
				Color.new255(158, 168, 180), \v, 64, 0.3); // apple graphite
		sliderKnobColor = HiliteGradient(Color.new255(163, 210, 255),
				Color.new255(109, 163, 220), \v, 64, 0.3); // apple blue
		numBox2LabelFont = Font.new("Helvetica", 12);
		numBox2LabelAlign = \right;
		numBox2LabelStringColor = Color.white;
		numBox2CanFocus = true;
		numBox2Font = Font.new("Helvetica", 12);
		numBox2Align = \left;
		numBox2BoxColor = Color.black;
		numBox2DefaultStringColor = Color.white;
		numBox2EnterStringColor = Color.green;
		numBox2ShiftKeyScaler = 10;

		// sliderLabel
		sliderLabel = StaticText.new(parent);
		sliderLabel.string_(name.asString);
		sliderLabel.font_(sliderLabelFont);
		sliderLabel.align_(sliderLabelAlign);
		sliderLabel.stringColor_(sliderLabelStringColor);

		("sliderLabel -> " ++ sliderLabel).postln;
		("sliderLabel.string -> " ++ sliderLabel.string).postln;
		("sliderLabel.font -> " ++ sliderLabel.font).postln;
		//("sliderLabel.align -> " ++ sliderLabel.align).postln;
		("sliderLabel.stringColor -> " ++ sliderLabel.stringColor).postln;

		// slider
		slider = Slider.new(parent);
		slider.canFocus_(sliderCanFocus);
		slider.background_(sliderBackground);
		slider.knobColor_(sliderKnobColor);
		slider.value_(spec.unmap(default + offset)); // displays correctly
		slider.step_(0); // infinite precision
		slider.action_({ arg slider;
			sliderActionList.do({ arg action;
				action.value(slider); // evaluate
			});
		});

		("slider -> " ++ slider).postln;
		("slider.canFocus -> " ++ slider.canFocus).postln;
		("slider.background -> " ++ slider.background).postln;
		("slider.knobColor -> " ++ slider.knobColor).postln;
		("slider.value -> " ++ slider.value).postln;
		("slider.step -> " ++ slider.step).postln;
		("slider.action -> " ++ slider.action).postln;

		// leftBoxLabel
		leftBoxLabel = StaticText.new(parent);
		leftBoxLabel.string_(units.asString);
		leftBoxLabel.font_(numBox2LabelFont);
		leftBoxLabel.align_(numBox2LabelAlign);
		leftBoxLabel.stringColor_(numBox2LabelStringColor);

		("leftBoxLabel -> " ++ leftBoxLabel).postln;
		("leftBoxLabel.string -> " ++ leftBoxLabel.string).postln;
		("leftBoxLabel.font -> " ++ leftBoxLabel.font).postln;
		//("leftBoxLabel.align -> " ++ leftBoxLabel.align).postln;
		("leftBoxLabel.stringColor -> " ++ leftBoxLabel.stringColor).postln;

		// leftBox
		leftBox = SCNumberBox2.new(parent);
		leftBox.canFocus_(numBox2CanFocus);
		leftBox.font_(numBox2Font);
		leftBox.align_(numBox2Align);
		leftBox.boxColor_(numBox2BoxColor);
		leftBox.defaultStringColor_(numBox2DefaultStringColor);
		leftBox.enterStringColor_(numBox2EnterStringColor);
		leftBox.shiftKeyScaler_(numBox2ShiftKeyScaler);
		leftBox.value_(left);
		leftBox.step_(step);
		leftBox.action_({ arg leftBox;
			leftBoxActionList.do({ arg action;
				action.value(leftBox); // evaluate
			});
		});

		("leftBox -> " ++ leftBox).postln;
		("leftBox.canFocus -> " ++ leftBox.canFocus).postln;
		("leftBox.font -> " ++ leftBox.font).postln;
		//("leftBox.align -> " ++ leftBox.align).postln;
		("leftBox.boxColor -> " ++ leftBox.boxColor).postln;
		("leftBox.defaultStringColor -> " ++ leftBox.defaultStringColor).postln;
		("leftBox.enterStringColor -> " ++ leftBox.enterStringColor).postln;
		("leftBox.shiftKeyScaler -> " ++ leftBox.shiftKeyScaler).postln;
		("leftBox.value -> " ++ leftBox.value).postln;
		("leftBox.step -> " ++ leftBox.step).postln;
		("leftBox.action -> " ++ leftBox.action).postln;

		// rightBoxLabel
		rightBoxLabel = StaticText.new(parent);
		rightBoxLabel.string_(units.asString);
		rightBoxLabel.font_(numBox2LabelFont);
		rightBoxLabel.align_(numBox2LabelAlign);
		rightBoxLabel.stringColor_(numBox2LabelStringColor);

		("rightBoxLabel -> " ++ rightBoxLabel).postln;
		("rightBoxLabel.string -> " ++ rightBoxLabel.string).postln;
		("rightBoxLabel.font -> " ++ rightBoxLabel.font).postln;
		//("rightBoxLabel.align -> " ++ rightBoxLabel.align).postln;
		("rightBoxLabel.stringColor -> " ++ rightBoxLabel.stringColor).postln;

		// rightBox
		rightBox = SCNumberBox2.new(parent);
		rightBox.canFocus_(numBox2CanFocus);
		rightBox.font_(numBox2Font);
		rightBox.align_(numBox2Align);
		rightBox.boxColor_(numBox2BoxColor);
		rightBox.defaultStringColor_(numBox2DefaultStringColor);
		rightBox.enterStringColor_(numBox2EnterStringColor);
		rightBox.shiftKeyScaler_(numBox2ShiftKeyScaler);
		rightBox.value_(right);
		rightBox.step_(step);
		rightBox.action_({ arg rightBox;
			rightBoxActionList.do({ arg action;
				action.value(rightBox); // evaluate
			});
		});

		("rightBox -> " ++ rightBox).postln;
		("rightBox.canFocus -> " ++ rightBox.canFocus).postln;
		("rightBox.font -> " ++ rightBox.font).postln;
		//("rightBox.align -> " ++ rightBox.align).postln;
		("rightBox.boxColor -> " ++ rightBox.boxColor).postln;
		("rightBox.defaultStringColor -> " ++ rightBox.defaultStringColor).postln;
		("rightBox.enterStringColor -> " ++ rightBox.enterStringColor).postln;
		("rightBox.shiftKeyScaler -> " ++ rightBox.shiftKeyScaler).postln;
		("rightBox.value -> " ++ rightBox.value).postln;
		("rightBox.step -> " ++ rightBox.step).postln;
		("rightBox.action -> " ++ rightBox.action).postln;

		// currentBoxLabel
		currentBoxLabel = StaticText.new(parent);
		currentBoxLabel.string_(units.asString);
		currentBoxLabel.font_(numBox2LabelFont);
		currentBoxLabel.align_(numBox2LabelAlign);
		currentBoxLabel.stringColor_(numBox2LabelStringColor);

		("currentBoxLabel -> " ++ currentBoxLabel).postln;
		("currentBoxLabel.string -> " ++ currentBoxLabel.string).postln;
		("currentBoxLabel.font -> " ++ currentBoxLabel.font).postln;
		//("currentBoxLabel.align -> " ++ currentBoxLabel.align).postln;
		("currentBoxLabel.stringColor -> " ++ currentBoxLabel.stringColor).postln;

		// currentBox
		currentBox = SCNumberBox2.new(parent);
		currentBox.canFocus_(numBox2CanFocus);
		currentBox.font_(numBox2Font);
		currentBox.align_(numBox2Align);
		currentBox.boxColor_(numBox2BoxColor);
		currentBox.defaultStringColor_(numBox2DefaultStringColor);
		currentBox.enterStringColor_(numBox2EnterStringColor);
		currentBox.shiftKeyScaler_(numBox2ShiftKeyScaler);
		currentBox.value_(default);
		currentBox.step_(step);
		currentBox.action_({ arg currentBox;
			currentBoxActionList.do({ arg action;
				action.value(currentBox); // evaluate
			});
		});

		("currentBox -> " ++ currentBox).postln;
		("currentBox.canFocus -> " ++ currentBox.canFocus).postln;
		("currentBox.font -> " ++ currentBox.font).postln;
		//("currentBox.align -> " ++ currentBox.align).postln;
		("currentBox.boxColor -> " ++ currentBox.boxColor).postln;
		("currentBox.defaultStringColor -> " ++ currentBox.defaultStringColor).postln;
		("currentBox.enterStringColor -> " ++ currentBox.enterStringColor).postln;
		("currentBox.shiftKeyScaler -> " ++ currentBox.shiftKeyScaler).postln;
		("currentBox.value -> " ++ currentBox.value).postln;
		("currentBox.step -> " ++ currentBox.step).postln;
		("currentBox.action -> " ++ currentBox.action).postln;
	}

	// define & register the action event functions...add them to actionList's
	addZoomSliderActions {
		// action functions to append to action list
		var	sliderAction = nil;
		var	leftBoxAction = nil;
		var	rightBoxAction = nil;
		var	currentBoxAction = nil;

		// define actions
		sliderAction = { arg slider;
			this.doZoomSliderSliderAction(slider);
		};
		leftBoxAction = { arg leftBox;
			this.doZoomSliderNumBoxAction(leftBox);
		};
		rightBoxAction = { arg rightBox;
			this.doZoomSliderNumBoxAction(rightBox);
		};
		currentBoxAction = { arg currentBox;
			this.doZoomSliderNumBoxAction(currentBox);
		};

		// create new actionLists
		sliderActionList = List.new;
		leftBoxActionList = List.new;
		rightBoxActionList = List.new;
		currentBoxActionList = List.new;

		// add actions to actionLists
		sliderActionList.add(sliderAction);
		leftBoxActionList.add(leftBoxAction);
		rightBoxActionList.add(rightBoxAction);
		currentBoxActionList.add(currentBoxAction);

		("sliderActionList -> " ++ sliderActionList).postln;
		("leftBoxActionList -> " ++ leftBoxActionList).postln;
		("rightBoxActionList -> " ++ rightBoxActionList).postln;
		("currentBoxActionList -> " ++ currentBoxActionList).postln;
	}

	// called when the slider is moved
	doZoomSliderSliderAction { arg slider;
		// update currentBox according to the spec range and slider position
		this.rescaleSpecRange; // do I really need to call this here???
		this.mapFromSliderPositionToCurrentBox;
	}

	// called when a numberBox (leftBox, rightBox, currentBox) is updated
	doZoomSliderNumBoxAction { arg numBox;
		// round the numBox value to the step size (switch w/ next line???)
		this.roundToStep(numBox);

		// check to make sure this numBox value is in min/max range
		this.clipToMinMaxRange(numBox);

		// if restrictRange == true, restrict to the left/right range
		if(restrictRange, { this.restrictCurrentBoxToLeftRightRange; });

		// as long as the leftBox and rightBox are not equal,
		// rescale the spec range and update the slider position
		if(leftBox.value != rightBox.value, {
			this.rescaleSpecRange;
			this.unmapFromCurrentBoxToSliderPosition;
		});
	}

	// round the numberBox value to the step size before displaying it
	// warning: may be subject to floating point rounding errors...
	roundToStep { arg view;
		view.value = view.value.round(step);
	}

	// keep the numberBox value within the min/max range
	// note: after the constructor, min is always less than max
	clipToMinMaxRange { arg view;
		if(view.value < min, {
			view.value = min;
		}, {
			if(view.value > max, {
				view.value = max;
			});
		});
	}

	// keep the currentBox within the inner left/right range
	// note: the relationship between left and right can change dynamically!
	restrictCurrentBoxToLeftRightRange {
		// most common case first...
		if(leftBox.value < rightBox.value, {
			if(currentBox.value < leftBox.value, {
				currentBox.value = leftBox.value;
			}, {
				if(currentBox.value > rightBox.value, {
					currentBox.value = rightBox.value;
				});
			});
		}, {
			if(leftBox.value > rightBox.value, {
				if(currentBox.value > leftBox.value, {
					currentBox.value = leftBox.value;
				}, {
					if(currentBox.value < rightBox.value, {
						currentBox.value = rightBox.value;
					});
				});
			}, {
				if(leftBox.value == rightBox.value, {
					if(currentBox.value != leftBox.value, {
						currentBox.value = leftBox.value;
					});
					/*
					if(currentBox.value != rightBox.value, {
						currentBox.value = rightBox.value;
					});
					*/
				});
			});
		});
	}

	// Zoom baby!!!
	// note: minval and maxval have their relation checked & fixed in ControlSpec
	rescaleSpecRange {
		spec.minval_(leftBox.value + offset);
		spec.maxval_(rightBox.value + offset);
		//spec.minval_(leftBox.value + offset).maxval_(rightBox.value + offset);
		//("[spec.minval, spec.maxval] -> " ++
		//		"[" ++ spec.minval ++ ", " ++ spec.maxval ++ "]").postln;
	}

	// update currentBox value in response to changing slider position
	mapFromSliderPositionToCurrentBox {
		currentBox.value_(spec.map(slider.value) - offset);
		//currentBox.value = spec.map(slider.value) - offset;
		//("[unmapped, mapped] -> " ++
		//		"[" ++ slider.value ++ ", " ++ currentBox.value ++ "]").postln;
	}

	// update position of slider in response to changing currentBox value
	unmapFromCurrentBoxToSliderPosition {
		slider.value_(spec.unmap(currentBox.value + offset));
		//slider.value = spec.unmap(currentBox.value + offset);
		//("[unmapped, mapped] -> " ++
		//		"[" ++ slider.value ++ ", " ++ currentBox.value ++ "]").postln;
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// initialize the internal state variables
	// update the gui views as needed
	setZoomSpec { arg argName,
			argMin = 0, argMax = 1,
			argLeft = 0, argRight = 1,
			argWarp = \lin, argStep = 0,
			argDefault = 0, argUnits;
		this.initZoomSliderStateVariables(parent, argName,
				argMin, argMax,
				argLeft, argRight,
				argWarp, argStep,
				argDefault, argUnits);
		this.createZoomSliderControlSpec(left + offset, right + offset,
				warp, step, default);

		sliderLabel.string = name.asString;
		slider.value_(spec.unmap(default + offset)); // displays correctly
		slider.step_(0); // infinite precision

		("sliderLabel.string -> " ++ sliderLabel.string).postln;
		("slider.value -> " ++ slider.value).postln;
		("slider.step -> " ++ slider.step).postln;

		leftBoxLabel.string = units.asString;
		leftBox.value_(left);
		leftBox.step_(step);

		("leftBoxLabel.string -> " ++ leftBoxLabel.string).postln;
		("leftBox.value -> " ++ leftBox.value).postln;
		("leftBox.step -> " ++ leftBox.step).postln;

		rightBoxLabel.string = units.asString;
		rightBox.value_(right);
		rightBox.step_(step);

		("rightBoxLabel.string -> " ++ rightBoxLabel.string).postln;
		("rightBox.value -> " ++ rightBox.value).postln;
		("rightBox.step -> " ++ rightBox.step).postln;

		currentBoxLabel.string = units.asString;
		currentBox.value_(default);
		currentBox.step_(step);

		("currentBoxLabel.string -> " ++ currentBoxLabel.string).postln;
		("currentBox.value -> " ++ currentBox.value).postln;
		("currentBox.step -> " ++ currentBox.step).postln;
	}

	//setZoomColors {}

	// plug this ZoomSlider instance into a synth's parameter input!!!
	// should include a third arg: unitScaler
	// (for preprocessed mapping before being sent to synth)
	plugIntoSynth { arg argSynth, argParameter;
		// actions to append to action list
		var	sliderAction = nil;
		var	leftBoxAction = nil;
		var	rightBoxAction = nil;
		var	currentBoxAction = nil;

		//("argSynth -> " ++ argSynth).postln;
		//("argParameter -> " ++ argParameter).postln;

		// need to test if any input arg points to nil
		sliderAction = { arg slider;
			argSynth.set(argParameter.asSymbol, currentBox.value);
		};

		leftBoxAction = { arg leftBox;
			argSynth.set(argParameter.asSymbol, currentBox.value);
		};

		rightBoxAction = { arg rightBox;
			argSynth.set(argParameter.asSymbol, currentBox.value);
		};

		currentBoxAction = { arg currentBox;
			argSynth.set(argParameter.asSymbol, currentBox.value);
		};


		sliderActionList.add(sliderAction);
		leftBoxActionList.add(leftBoxAction);
		rightBoxActionList.add(rightBoxAction);
		currentBoxActionList.add(currentBoxAction);

		("sliderActionList -> " ++ sliderActionList).postln;
		("leftBoxActionList -> " ++ leftBoxActionList).postln;
		("rightBoxActionList -> " ++ rightBoxActionList).postln;
		("currentBoxActionList -> " ++ currentBoxActionList).postln;
	}

	// draw the widgets on the screen within a parent window
	draw { arg xOff = 5, yOff = 5,
			orientation = \h,
			width = 20, gap = 5,
			sliderLength = 200, boxLength = 40,
			currentBoxSide = \right;
		var	offsets = nil; // an array of relative widget offsets

		// most common case first...
		if(orientation == \h, {
			if(currentBoxSide == \right, {
				offsets = [0, boxLength, boxLength + sliderLength,
						(boxLength * 2) + sliderLength];
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
				(name ++ " " ++ "slider was drawn at [" ++
						xOff ++ ", " ++ yOff ++ "]").postln;
			}, {
				if(currentBoxSide == \left, {
					offsets = [0, boxLength, boxLength * 2,
							(boxLength * 2) + sliderLength];
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
					(name ++ " " ++ "slider was drawn at [" ++
							xOff ++ ", " ++ yOff ++ "]").postln;
				});
			});
		}, {
			if(orientation == \v, {
				// most common case first...
				if(currentBoxSide == \right, {
					offsets = [0, boxLength, boxLength + sliderLength,
							(boxLength * 2) + sliderLength];
					// shift all offsets according to gap width
					offsets = offsets.collect({ arg each, i;
						each = each + (i * gap); });

					// in order to deal with the slider [0,1] value being flipped...
					// (probably an init error in SCSlider's primitive)
					// the leftBox and rightBox indices have been switched:
					// leftBox  (offsets[0] -> offsets[2])
					// rightBox ([offsets[2] -> offsets[0])

					leftBoxLabel.bounds_(Rect.new(xOff,
							yOff + offsets[2], width, boxLength));
					leftBox.bounds_(Rect.new(xOff,
							yOff + offsets[2], width, boxLength));
					sliderLabel.bounds_(Rect.new(xOff,
							yOff + offsets[1], width, sliderLength));
					slider.bounds_(Rect.new(xOff,
							yOff + offsets[1], width, sliderLength));
					rightBoxLabel.bounds_(Rect.new(xOff,
							yOff + offsets[0], width, boxLength));
					rightBox.bounds_(Rect.new(xOff,
							yOff + offsets[0], width, boxLength));
					currentBoxLabel.bounds_(Rect.new(xOff,
							yOff + offsets[3], width, boxLength));
					currentBox.bounds_(Rect.new(xOff,
							yOff + offsets[3], width, boxLength));
					(name ++ " " ++ "slider was drawn at [" ++
							xOff ++ ", " ++ yOff ++ "]").postln;
				}, {
					if(currentBoxSide == \left, {
						offsets = [0, boxLength, boxLength * 2,
								(boxLength * 2) + sliderLength];
						// shift all offsets according to gap width
						offsets = offsets.collect({ arg each, i;
							each = each + (i * gap); });

						// in order to deal with the slider [0,1] value being flipped...
						// (probably an init error in SCSlider's primitive)
						// the leftBox and rightBox indices have been switched:
						// leftBox  (offsets[1] -> offsets[3])
						// rightBox ([offsets[3] -> offsets[1])

						currentBoxLabel.bounds_(Rect.new(xOff,
								yOff + offsets[0], width, boxLength));
						currentBox.bounds_(Rect.new(xOff,
								yOff + offsets[0], width, boxLength));
						leftBoxLabel.bounds_(Rect.new(xOff,
								yOff + offsets[3], width, boxLength));
						leftBox.bounds_(Rect.new(xOff,
								yOff + offsets[3], width, boxLength));
						sliderLabel.bounds_(Rect.new(xOff,
								yOff + offsets[2], width, sliderLength));
						slider.bounds_(Rect.new(xOff,
								yOff + offsets[2], width, sliderLength));
						rightBoxLabel.bounds_(Rect.new(xOff,
								yOff + offsets[1], width, boxLength));
						rightBox.bounds_(Rect.new(xOff,
								yOff + offsets[1], width, boxLength));
						(name ++ " " ++ "slider was drawn at [" ++
								xOff ++ ", " ++ yOff ++ "]").postln;
					});
				});
			});
		});
	}
}