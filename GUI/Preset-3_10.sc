/*
 * David Thall
 * Preset.sc
 * CG2 Project
 */

// A Preset contains all of the values associated with the state of GUI widgets

// I am storing a multi-dimensional Array to a dictionary,
// using simple enumerations as Key Associations
// Notice the strict order when loading the values back to a MappableSlider
// (leftBox & rightBox must be loaded 'before' currentBox due to
// ZoomSlider.restrictRange = true!!!)

// I could actually store the Preset without referencing my actual ZoomSlider
// Classes directly.  This would require accessing my TopViews children!!!

// Methods: store/save, recall/load, reload/restore/reset/factory/defaults

// NOTE: accessing a ControlSet may not be foolproof in cases where the
// internal memory state changes... consider using an Array of ZoomSliders!!!

/*	*** Preset 'Dictionary Formatting' v1.0 ***
 *
 *	ZoomSlider(class, leftBox, rightBox, currentBox)
 *	MappableSlider(class, leftBox, rightBox, currentBox, lockButton, axisButton)
 *	ModulationSlider(class, leftBox, rightBox, currentBox, lockButton, axisButton,
 *		bypassButton, posNegButton, modSrcPopUpMenu, gainViewButton)
 *	ParameterSlider(class, leftBox, rightBox, currentBox, lockButton, axisButton,
 *		rangeClipButton)
 */

// 79 chars
///////////////////////////////////////////////////////////////////////////////

Preset {
	// ZoomSlider ControlSet
	var	<zoomSliderControlSet;	// ZoomSlider ControlSet
	// Dictionaries (configurations)
	var	<currentDictionary;	// currently stored dictionary (always in RAM)
	var	<defaultDictionary; 	// default dictionary (can be written to disk)

	// create a new Preset
	*new { arg argZoomSliderControlSet;
		if(argZoomSliderControlSet.isNil, {
			(this.asString ++ ": " ++
					"A ControlSet must be provided!").postln;
			^nil;
		});
		^super.new.initPreset(argZoomSliderControlSet);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ZoomSlider instance into a consistent state
	initPreset { arg argZoomSliderControlSet;

		zoomSliderControlSet = argZoomSliderControlSet;
		("zoomSliderControlSet -> " ++ zoomSliderControlSet).postln;

		// create 2 new dictionary's,
		// initialize to the current size of the zoomSliderControlSet
		currentDictionary = Dictionary.new(zoomSliderControlSet.size);
		defaultDictionary = Dictionary.new(zoomSliderControlSet.size);

		("currentDictionary -> " ++ currentDictionary).postln;
		("currentDictionary.size -> " ++ currentDictionary.size).postln;
		("defaultDictionary -> " ++ defaultDictionary).postln;
		("defaultDictionary.size -> " ++ defaultDictionary.size).postln;
	}

	// write to a dictionary
	writeTo { arg dictionary;
		zoomSliderControlSet.do({ arg view, i;
			// most common case first...
			if(view.class == ParameterSlider, {
				("The Set at " ++ i ++ " is a " ++
						view.class).postln;
				dictionary.put(i,
						[view.class,
						view.leftBox.value,
						view.rightBox.value,
						view.currentBox.value,
						view.lockButton.value,
						view.axisButton.value,
						view.rangeClipButton.value]);
				("dictionary.at(" ++ i ++ ") -> " ++
						dictionary.at(i)).postln;
			}, {
				if(view.class == ModulationSlider, {
					("The Set at " ++ i ++ " is a " ++
							view.class).postln;
					dictionary.put(i,
							[view.class,
							view.leftBox.value,
							view.rightBox.value,
							view.currentBox.value,
							view.lockButton.value,
							view.axisButton.value,
							view.bypassButton.value,
							view.posNegButton.value,
							view.modSrcPopUpMenu.value,
							view.gainViewButton.value]);
					("dictionary.at(" ++ i ++ ") -> " ++
							dictionary.at(i)).postln;
				}, {
					if(view.class == MappableSlider, {
						("The Set at " ++ i ++ " is a " ++
								view.class).postln;
						dictionary.put(i,
								[view.class,
								view.leftBox.value,
								view.rightBox.value,
								view.currentBox.value,
								view.lockButton.value,
								view.axisButton.value]);
						("dictionary.at(" ++ i ++ ") -> " ++
								dictionary.at(i)).postln;
					}, {
						if(view.class == ZoomSlider, {
							("The Set at " ++ i ++ " is a " ++
									view.class).postln;
							dictionary.put(i,
									[view.class,
									view.leftBox.value,
									view.rightBox.value,
									view.currentBox.value]);
							("dictionary.at(" ++ i ++ ") -> " ++
									dictionary.at(i)).postln;
						});
					});
				});
			});
		});
	}

	// read from a dictionary
	readFrom { arg dictionary;
		zoomSliderControlSet.do({ arg view, i;
			// most common case first...
			if(view.class == ParameterSlider, {
				("The Set at " ++ i ++ " is a " ++
						dictionary.at(i).at(0)).postln;
				view.leftBox.valueAction_(dictionary.at(i).at(1));
				view.rightBox.valueAction_(dictionary.at(i).at(2));
				view.currentBox.valueAction_(dictionary.at(i).at(3));
				view.lockButton.valueAction_(dictionary.at(i).at(4));
				view.axisButton.valueAction_(dictionary.at(i).at(5));
				view.rangeClipButton.valueAction_(dictionary.at(i).at(6));
			}, {
				if(view.class == ModulationSlider, {
					("The Set at " ++ i ++ " is a " ++
							dictionary.at(i).at(0)).postln;
					view.leftBox.valueAction_(dictionary.at(i).at(1));
					view.rightBox.valueAction_(dictionary.at(i).at(2));
					view.currentBox.valueAction_(dictionary.at(i).at(3));
					view.lockButton.valueAction_(dictionary.at(i).at(4));
					view.axisButton.valueAction_(dictionary.at(i).at(5));
					view.bypassButton.valueAction_(dictionary.at(i).at(6));
					view.posNegButton.valueAction_(dictionary.at(i).at(7));
					view.modSrcPopUpMenu.valueAction_(dictionary.at(i).at(8));
					view.gainViewButton.valueAction_(dictionary.at(i).at(9));
				}, {
					if(view.class == MappableSlider, {
						("The Set at " ++ i ++ " is a " ++
								dictionary.at(i).at(0)).postln;
						view.leftBox.valueAction_(dictionary.at(i).at(1));
						view.rightBox.valueAction_(dictionary.at(i).at(2));
						view.currentBox.valueAction_(dictionary.at(i).at(3));
						view.lockButton.valueAction_(dictionary.at(i).at(4));
						view.axisButton.valueAction_(dictionary.at(i).at(5));
					}, {
						if(view.class == ZoomSlider, {
							("The Set at " ++ i ++ " is a " ++
									dictionary.at(i).at(0)).postln;
							view.leftBox.valueAction_(dictionary.at(i).at(1));
							view.rightBox.valueAction_(dictionary.at(i).at(2));
							view.currentBox.valueAction_(dictionary.at(i).at(3));
						});
					});
				});
			});
		});
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// store a configuration of ZoomSlider parameter values into this Preset
	store { arg argWhichDictionary = \currentDictionary;

		if(argWhichDictionary == \currentDictionary, {
			this.writeTo(currentDictionary);
		}, {
			if(argWhichDictionary == \defaultDictionary, {
				this.writeTo(defaultDictionary);
			});
		});

	}

	// load a previously saved configuration of ZoomSlider parameter values
	// from this Preset
	load { arg argWhichDictionary = \currentDictionary;

		if(argWhichDictionary == \currentDictionary, {
			if(currentDictionary.isEmpty, {
				("currentDictionary is empty").postln;
			}, {
				this.readFrom(currentDictionary);
			});
		}, {
			if(argWhichDictionary == \defaultDictionary, {
				if(defaultDictionary.isEmpty, {
					("defaultDictionary is empty").postln;
				}, {
					this.readFrom(defaultDictionary);
				});
			});
		});

	}
}