/*
 * David Thall
 * SliderOptions.sc
 * CG2 Project
 */

// An instance of SliderOptions has a reference to a select group of ZoomSliders.
// It has a group of gui views (e.g., buttons) that can be created and linked to
// ZoomSliders and their .actions.

// 79 chars
///////////////////////////////////////////////////////////////////////////////

SliderOptions {
	// Name & Location
	var	<parent, <name;
	// GUI Widgets
	var	<densityTriggerPopUpMenu, <densityBufferBox,
		<pitchSpecButton, <pitchDirectionButton, <pitchFormantButton,
		<timeSpecButton, <timeModePopUpMenu,
		<phaseWindowRangeSliderLabel, <phaseWindowRangeSlider,
		<amplitudeEnvelopePopUpMenu, amplitudeGainCompPopUpMenu;
	// ZoomSliderArray to update
	var	<zoomSliderArray;
	// store the current densityTriggerButton.value
	var	<currentAxisButtonValue = 0; // default == off

	// create and name a new SliderOptions
	*new { arg argParent, argName, argZoomSliderArray;
		^super.new.initSliderOptions(argParent, argName, argZoomSliderArray);
	}

	initSliderOptions { arg argParent, argName, argZoomSliderArray;

		parent = argParent;
		name = argName;
		zoomSliderArray = argZoomSliderArray;

		("parent -> " ++ parent).postln;
		("name -> " ++ name).postln;
		("zoomSliderArray -> " ++ zoomSliderArray).postln;

		/* Declarations */

		// densityTriggerPopUpMenu
		densityTriggerPopUpMenu = SCPopUpMenu.new(parent);
		densityTriggerPopUpMenu.canFocus = false;
		densityTriggerPopUpMenu.font = Font.new("Helvetica", 10);
		densityTriggerPopUpMenu.background = HiliteGradient.new(Color.gray, Color.black, \h, 256, 0);
		densityTriggerPopUpMenu.stringColor = Color.white;

		// densityBufferBox
		densityBufferBox = SCNumberBox2.new(parent);
		densityBufferBox.canFocus = true;
		densityBufferBox.font = Font.new("Helvetica", 12);
		densityBufferBox.boxColor = Color.white;
		densityBufferBox.defaultStringColor = Color.black;
		densityBufferBox.enterStringColor = Color.red;

		// pitchSpecButton
		pitchSpecButton = SCButton.new(parent);
		pitchSpecButton.canFocus = false;
		pitchSpecButton.font = Font.new("Helvetica", 10);

		// pitchDirectionButton
		pitchDirectionButton = SCButton.new(parent);
		pitchDirectionButton.canFocus = false;
		pitchDirectionButton.font = Font.new("Helvetica", 10);

		// pitchFormantButton
		pitchFormantButton = SCButton.new(parent);
		pitchFormantButton.canFocus = false;
		pitchFormantButton.font = Font.new("Helvetica", 10);

		// timeSpecButton
		timeSpecButton = SCButton.new(parent);
		timeSpecButton.canFocus = false;
		timeSpecButton.font = Font.new("Helvetica", 10);

		// timeModePopUpMenu
		timeModePopUpMenu = SCPopUpMenu.new(parent);
		timeModePopUpMenu.canFocus = false;
		timeModePopUpMenu.font = Font.new("Helvetica", 10);
		timeModePopUpMenu.background = HiliteGradient.new(Color.gray, Color.black, \h, 256, 0);
		timeModePopUpMenu.stringColor = Color.white;

		// phaseWindowRangeSliderLabel
		phaseWindowRangeSliderLabel = SCStaticText.new(parent);
		phaseWindowRangeSliderLabel.string = "window";
		phaseWindowRangeSliderLabel.font = Font.new("Helvetica", 10);
		phaseWindowRangeSliderLabel.align = \center;
		phaseWindowRangeSliderLabel.stringColor = Color.white;
		// phaseWindowRangeSlider
		phaseWindowRangeSlider = SCRangeSlider.new(parent);
		phaseWindowRangeSlider.canFocus = false;
		phaseWindowRangeSlider.background = Color.black;
		phaseWindowRangeSlider.knobColor = Color.green.alpha_(0.3);

		// amplitudeEnvelopePopUpMenu
		amplitudeEnvelopePopUpMenu = SCPopUpMenu.new(parent);
		amplitudeEnvelopePopUpMenu.canFocus = false;
		amplitudeEnvelopePopUpMenu.font = Font.new("Helvetica", 10);
		amplitudeEnvelopePopUpMenu.background = HiliteGradient.new(Color.gray, Color.black, \h, 256, 0);
		amplitudeEnvelopePopUpMenu.stringColor = Color.white;

		// amplitudeGainCompPopUpMenu
		amplitudeGainCompPopUpMenu = SCPopUpMenu.new(parent);
		amplitudeGainCompPopUpMenu.canFocus = false;
		amplitudeGainCompPopUpMenu.font = Font.new("Helvetica", 10);
		amplitudeGainCompPopUpMenu.background = HiliteGradient.new(Color.gray, Color.black, \h, 256, 0);
		amplitudeGainCompPopUpMenu.stringColor = Color.white;

		/* Initializations */

		densityTriggerPopUpMenu.items = [
			"synchronous", "asynchronous", "external key", "audio input"
		];
		densityBufferBox.value = 1;
		densityBufferBox.step = 1;
		pitchSpecButton.states = [
			["pitch", Color.black, Color.clear],
			["speed", Color.black, Color.clear]
		];
		pitchDirectionButton.states = [
			["forward", Color.black, Color.clear],
			["reverse", Color.black, Color.clear]
		];
		pitchFormantButton.states = [
			["normal", Color.black, Color.clear],
			["formant", Color.black, Color.clear]
		];
		timeSpecButton.states = [
			["time", Color.black, Color.clear],
			["rate", Color.black, Color.clear]
		];
		timeModePopUpMenu.items = [
			"forward", "reverse", "zigzag", "freeze"
		];
		phaseWindowRangeSlider.lo = 0;
		phaseWindowRangeSlider.hi = 1;
		phaseWindowRangeSlider.range = 1;
		phaseWindowRangeSlider.step = 0;
		amplitudeEnvelopePopUpMenu.items = [
			"rectangle", "triangle", "round", "smooth", "expodec", "r-expodec", "rand-env"
		];
		amplitudeGainCompPopUpMenu.items = [
			"eq-power", "eq-gain"
		];
	}

	drawDensityRow { arg xOff = 5, yOff = 5, height = 20, gap = 5, popUpMenuLength = 85, boxLength = 40;
		var	offsets;

		// an array of default widget offsets (in relation to neighboring widgets)
		offsets = [0, popUpMenuLength];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i; each = each + (i * gap); });

		densityTriggerPopUpMenu.bounds = Rect.new(xOff + offsets[0], yOff, popUpMenuLength, height);
		densityBufferBox.bounds = Rect.new(xOff + offsets[1], yOff, boxLength, height);
	}

	drawPitchRow { arg xOff = 5, yOff = 5, height = 20, gap = 5, buttonLength = 40;
		var	offsets;

		// an array of default widget offsets (in relation to neighboring widgets)
		offsets = [0, buttonLength, buttonLength * 2];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i; each = each + (i * gap); });

		pitchSpecButton.bounds = Rect.new(xOff + offsets[0], yOff, buttonLength, height);
		pitchDirectionButton.bounds = Rect.new(xOff + offsets[1], yOff, buttonLength, height);
		pitchFormantButton.bounds = Rect.new(xOff + offsets[2], yOff, buttonLength, height);
	}

	drawTimeRow { arg xOff = 5, yOff = 5, height = 20, gap = 5, buttonLength = 40, popUpMenuLength = 85;
		var	offsets;

		// an array of default widget offsets (in relation to neighboring widgets)
		offsets = [0, buttonLength];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i; each = each + (i * gap); });

		timeSpecButton.bounds = Rect.new(xOff + offsets[0], yOff, buttonLength, height);
		timeModePopUpMenu.bounds = Rect.new(xOff + offsets[1], yOff, popUpMenuLength, height);
	}

	drawPhaseRow { arg xOff = 5, yOff = 5, width = 130, height = 20;
		phaseWindowRangeSliderLabel.bounds = Rect.new(xOff, yOff, width, height);
		phaseWindowRangeSlider.bounds = Rect.new(xOff, yOff, width, height);
	}

	drawAmplitudeRow { arg xOff = 5, yOff = 5, height = 20, gap = 5, popUpMenuLength = 60;
		var	offsets;

		// an array of default widget offsets (in relation to neighboring widgets)
		offsets = [0, popUpMenuLength];
		// shift all offsets according to gap width
		offsets = offsets.collect({ arg each, i; each = each + (i * gap); });

		amplitudeEnvelopePopUpMenu.bounds = Rect.new(xOff + offsets[0], yOff, popUpMenuLength, height);
		amplitudeGainCompPopUpMenu.bounds = Rect.new(xOff + offsets[1], yOff, popUpMenuLength, height);
	}
}