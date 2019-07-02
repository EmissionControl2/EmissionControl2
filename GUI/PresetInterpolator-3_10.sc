/*
  * David Thall
 * PresetInterpolator.sc
 * CG2 Project
 */

// A PresetInterpolator spawns tasks that update
// ZoomSlider's toward target values (constant time algorithm)

// Fix: Must set ZoomSlider.restrictRange_(false) when doing a x-over
// Update: I will probably need to change the Preset class's ZoomSliderControlSet
// to another type of data structure that can rely on explicit ordering.
// I'm not going to be adding or removing from my preset.zoomSliderControlSet,
// but the garbage collector may come around and reorder things at any time!!!
// Add: Interpolation Curves (lin, exp, log)

// Update: Change targetPresetNum to actual object (i.e., targetPreset).
// *** Try to decrease the number of running tasks!
// NEW IDEA: Updating gui views takes a lot of 'average cpu',
// especially when the interpolation occurs quickly.
// I should add a way to choose how many times to update the visual as a ratio
// of the actual internal .valueAction updates (which don't use much CPU)
// NEW IDEA: Instead of making new tasks every time the method is called,
// store them internally upon construction.  This will help with the 'peak cpu'
// spikes that occur when making .start calls!!!
// GREAT NEW IDEA: Add variables to calculate the synthUpdateRate and
// guiUpdateRate independently (both are in maxStepsPerSec)!!!
// GREAT NEW IDEA: Don't create & run a Task if the value should never update!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

PresetInterpolator {
	// state variables
	var	<interpolationTime = nil;	// interpolation time constant
	var	<maxStepsPerSec = nil;		// max num of steps to get to destination
	// PresetButton Array
	var	<>presetButtonArray = nil;	// array of PresetButtons
	// running Tasks
	var	<runningTasks = nil;		// array of running tasks

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create a new PresetInterpolator
	*new { arg argPresetButtonArray,
			argInterpolationTime = 10,
			argMaxStepsPerSec = 10;
		if(argPresetButtonArray.isNil, {
			(this.asString ++ ": " ++
					"A PresetButton array must be provided!").postln;
			^nil;
		});
		^super.new.initPresetInterpolator(argPresetButtonArray,
				argInterpolationTime,
				argMaxStepsPerSec);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ZoomSlider instance into a consistent state
	initPresetInterpolator { arg argPresetButtonArray,
			argInterpolationTime,
			argMaxStepsPerSec;
		this.initPresetInterpolatorStateVariables(argPresetButtonArray,
				argInterpolationTime,
				argMaxStepsPerSec);
		//this.setupRunningTasksArray;
	}

	// initialize all the internal state variables
	initPresetInterpolatorStateVariables { arg argPresetButtonArray,
			argInterpolationTime,
			argMaxStepsPerSec;

		this.setParameters(argInterpolationTime, argMaxStepsPerSec);

		presetButtonArray = argPresetButtonArray;
		("presetButtonArray -> " ++ presetButtonArray).postln;

		runningTasks = Array.new;
		("runningTasks -> " ++ runningTasks).postln;
	}


	// setup the runningTasks array (do this only at init time!)
	setupRunningTasksArray {
		var	somePreset = nil;
		var	numViewsToUpdate = nil;
		somePreset = presetButtonArray[0].preset;
		numViewsToUpdate = somePreset.zoomSliderControlSet.size * 3;

		("somePreset -> " ++ somePreset).postln;
		("numViewsToUpdate -> " ++ numViewsToUpdate).postln;

		// create a new array to reference all running tasks
		runningTasks = Array.new(numViewsToUpdate);
		("runningTasks -> " ++ runningTasks).postln;
	}


	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// initialize the internal state variables
	setParameters { arg argInterpolationTime, argMaxStepsPerSec;
		interpolationTime = argInterpolationTime.abs;
		maxStepsPerSec = argMaxStepsPerSec.abs;

		("interpolationTime -> " ++ interpolationTime).postln;
		("maxStepsPerSec -> " ++ maxStepsPerSec).postln;
	}

	// constant-time interpolation algorithm
	start { arg argTargetPresetNum = 1,
			argWhichDictionary = \currentDictionary;
		var	somePreset = nil;
		var	numViewsToUpdate = nil;

		var	targetPreset = nil;
		var	maxSteps = nil;

		/**********/ // setupRunningTasksArray
		somePreset = presetButtonArray[0].preset;
		numViewsToUpdate = somePreset.zoomSliderControlSet.size * 3;

		("somePreset -> " ++ somePreset).postln;
		("numViewsToUpdate -> " ++ numViewsToUpdate).postln;

		// create a new array to reference all running tasks (each time)
		runningTasks = Array.new(numViewsToUpdate);
		("runningTasks -> " ++ runningTasks).postln;
		/**********/

		// store a local copy of the target preset to interpolate toward
		targetPreset = presetButtonArray[argTargetPresetNum - 1].preset;
		// maximum number of steps to keep update rate under maxStepsPerSec
		maxSteps = ((maxStepsPerSec * interpolationTime).asInteger).max(1);

		// loop through each of the gui views
		targetPreset.zoomSliderControlSet.do({ arg view, i;
			var	zoomSliderClass = nil;
			// current values
			var	leftBoxValue = nil;
			var	rightBoxValue = nil;
			var	currentBoxValue = nil;
			// target values
			var	targetLeftBoxValue = nil;
			var	targetRightBoxValue = nil;
			var	targetCurrentBoxValue = nil;
			// total range of values between current and target
			// (according to step size)
			var	leftBoxRange = nil;
			var	rightBoxRange = nil;
			var	currentBoxRange = nil;
			// number of actual steps to take
			var	leftBoxTimesToMove = nil;
			var	rightBoxTimesToMove = nil;
			var	currentBoxTimesToMove = nil;
			// actual calculated increment (pos/neg) for each step
			var	leftBoxIncrement = nil;
			var	rightBoxIncrement = nil;
			var	currentBoxIncrement = nil;
			// amount of time to wait between each step
			var	leftBoxDeltaTime = nil;
			var	rightBoxDeltaTime = nil;
			var	currentBoxDeltaTime = nil;
			// a task thread to run for each view
			var	leftBoxMorphTask = nil;
			var	rightBoxMorphTask = nil;
			var	currentBoxMorphTask = nil;

			// depending on which dictionary is selected...
			if(argWhichDictionary == \currentDictionary, {
				// determine which ZoomSlider Class is contained in this gui view
				zoomSliderClass = targetPreset.currentDictionary.at(i).at(0);
				("zoomSliderClass -> " ++ zoomSliderClass).postln;

				// most common case first...
				if(zoomSliderClass == ParameterSlider, {
					view.lockButton.valueAction_(
							targetPreset.currentDictionary.at(i).at(4));
					view.axisButton.valueAction_(
							targetPreset.currentDictionary.at(i).at(5));
					view.rangeClipButton.valueAction_(
							targetPreset.currentDictionary.at(i).at(6));
				}, {
					if(zoomSliderClass == ModulationSlider, {
						view.lockButton.valueAction_(
								targetPreset.currentDictionary.at(i).at(4));
						view.axisButton.valueAction_(
								targetPreset.currentDictionary.at(i).at(5));
						// shouldn't be turned off prior to interpolation!!!
						//view.bypassButton.valueAction_(
						//		targetPreset.currentDictionary.at(i).at(6));
						view.posNegButton.valueAction_(
								targetPreset.currentDictionary.at(i).at(7));
						view.modSrcPopUpMenu.valueAction_(
								targetPreset.currentDictionary.at(i).at(8));
						view.gainViewButton.valueAction_(
								targetPreset.currentDictionary.at(i).at(9));
					}, {
						if(zoomSliderClass == MappableSlider, {
							// should probably happen at the end of interpolation???
							view.lockButton.valueAction_(
									targetPreset.currentDictionary.at(i).at(4));
							view.axisButton.valueAction_(
									targetPreset.currentDictionary.at(i).at(5));
						}, {
							if(zoomSliderClass == ZoomSlider, {
								// nothing to update...
							});
						});
					});
				});
			}, {
				if(argWhichDictionary == \defaultDictionary, {
					// determine which ZoomSlider Class is contained in this gui view
					zoomSliderClass = targetPreset.defaultDictionary.at(i).at(0);
					("zoomSliderClass -> " ++ zoomSliderClass).postln;

					// most common case first...
					if(zoomSliderClass == ParameterSlider, {
						view.lockButton.valueAction_(
								targetPreset.defaultDictionary.at(i).at(4));
						view.axisButton.valueAction_(
								targetPreset.defaultDictionary.at(i).at(5));
						view.rangeClipButton.valueAction_(
								targetPreset.defaultDictionary.at(i).at(6));
					}, {
						if(zoomSliderClass == ModulationSlider, {
							view.lockButton.valueAction_(
									targetPreset.defaultDictionary.at(i).at(4));
							view.axisButton.valueAction_(
									targetPreset.defaultDictionary.at(i).at(5));
							// shouldn't be turned off prior to interpolation!!!
							//view.bypassButton.valueAction_(
							//		targetPreset.defaultDictionary.at(i).at(6));
							view.posNegButton.valueAction_(
									targetPreset.defaultDictionary.at(i).at(7));
							view.modSrcPopUpMenu.valueAction_(
									targetPreset.defaultDictionary.at(i).at(8));
							view.gainViewButton.valueAction_(
									targetPreset.defaultDictionary.at(i).at(9));
						}, {
							if(zoomSliderClass == MappableSlider, {
								// should probably happen at the end of interpolation???
								view.lockButton.valueAction_(
										targetPreset.defaultDictionary.at(i).at(4));
								view.axisButton.valueAction_(
										targetPreset.defaultDictionary.at(i).at(5));
							}, {
								if(zoomSliderClass == ZoomSlider, {
									// nothing to update...
								});
							});
						});
					});
				});
			});

			// get all the numBox values from the ZoomSlider on the screen
			leftBoxValue = view.leftBox.value;
			rightBoxValue = view.rightBox.value;
			currentBoxValue = view.currentBox.value;
			//("leftBoxValue -> " ++ leftBoxValue).postln;
			//("rightBoxValue -> " ++ rightBoxValue).postln;
			//("currentBoxValue -> " ++ currentBoxValue).postln;

			// extract all the stored values from the target preset
			// (from within the correct dictionary)
			if(argWhichDictionary == \currentDictionary, {
				targetLeftBoxValue = targetPreset.currentDictionary.at(i).at(1);
				targetRightBoxValue = targetPreset.currentDictionary.at(i).at(2);
				targetCurrentBoxValue = targetPreset.currentDictionary.at(i).at(3);
			}, {
				if(argWhichDictionary == \defaultDictionary, {
					targetLeftBoxValue = targetPreset.defaultDictionary.at(i).at(1);
					targetRightBoxValue = targetPreset.defaultDictionary.at(i).at(2);
					targetCurrentBoxValue = targetPreset.defaultDictionary.at(i).at(3);
				});
			});
			//("targetLeftBoxValue -> " ++ targetLeftBoxValue).postln;
			//("targetRightBoxValue -> " ++ targetRightBoxValue).postln;
			//("targetCurrentBoxValue -> " ++ targetCurrentBoxValue).postln;

			// here, the range is calculated as the actual number of steps
			// between the current and target values
			leftBoxRange = (targetLeftBoxValue - leftBoxValue).abs *
					view.step.reciprocal;
			rightBoxRange = (targetRightBoxValue - rightBoxValue).abs *
					view.step.reciprocal;
			currentBoxRange = (targetCurrentBoxValue - currentBoxValue).abs *
					view.step.reciprocal;
			//("leftBoxRange -> " ++ leftBoxRange).postln;
			//("rightBoxRange -> " ++ rightBoxRange).postln;
			//("currentBoxRange -> " ++ currentBoxRange).postln;

			// times to move within interpolationTime
			// (can never be > maxSteps or < 1) (>=1 TO AVOID INF & NAN below!)
			leftBoxTimesToMove = (
				if(leftBoxRange < maxSteps, {
					leftBoxRange;
				}, {
					maxSteps;
				});
			).max(1);
			rightBoxTimesToMove = (
				if(rightBoxRange < maxSteps, {
					rightBoxRange;
				}, {
					maxSteps;
				});
			).max(1);
			currentBoxTimesToMove = (
				if(currentBoxRange < maxSteps, {
					currentBoxRange;
				}, {
					maxSteps;
				});
			).max(1);
			//("leftBoxTimesToMove -> " ++ leftBoxTimesToMove).postln;
			//("rightBoxTimesToMove -> " ++ rightBoxTimesToMove).postln;
			//("currentBoxTimesToMove -> " ++ currentBoxTimesToMove).postln;

			// increment = timesToMove.reciprocal * (target - current)
			leftBoxIncrement = (targetLeftBoxValue - leftBoxValue) /
					leftBoxTimesToMove;
			rightBoxIncrement = (targetRightBoxValue - rightBoxValue) /
					rightBoxTimesToMove;
			currentBoxIncrement = (targetCurrentBoxValue - currentBoxValue) /
					currentBoxTimesToMove;
			//("leftBoxIncrement -> " ++ leftBoxIncrement).postln;
			//("rightBoxIncrement -> " ++ rightBoxIncrement).postln;
			//("currentBoxIncrement -> " ++ currentBoxIncrement).postln;

			// deltaTime = leftBoxTimesToMove.reciprocal * interpolationTime;
			leftBoxDeltaTime = interpolationTime / leftBoxTimesToMove;
			rightBoxDeltaTime = interpolationTime / rightBoxTimesToMove;
			currentBoxDeltaTime = interpolationTime / currentBoxTimesToMove;
			//("leftBoxDeltaTime -> " ++ leftBoxDeltaTime).postln;
			//("rightBoxDeltaTime -> " ++ rightBoxDeltaTime).postln;
			//("currentBoxDeltaTime -> " ++ currentBoxDeltaTime).postln;

			leftBoxMorphTask = Task.new({
				//runningTasks.add(leftBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
				leftBoxTimesToMove.do({
					{ view.leftBox.valueAction_(leftBoxValue.round(view.step)); }.defer;
					//("leftBoxValue -> " ++ leftBoxValue).postln;
					leftBoxValue = leftBoxValue + leftBoxIncrement;
					leftBoxDeltaTime.wait;
				});
				//runningTasks.remove(leftBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
			}, SystemClock);

			rightBoxMorphTask = Task.new({
				//runningTasks.add(rightBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
				rightBoxTimesToMove.do({
					{ view.rightBox.valueAction_(rightBoxValue.round(view.step)); }.defer;
					//("rightBoxValue -> " ++ rightBoxValue).postln;
					rightBoxValue = rightBoxValue + rightBoxIncrement;
					rightBoxDeltaTime.wait;
				});
				//runningTasks.remove(rightBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
			}, SystemClock);

			currentBoxMorphTask = Task.new({
				//runningTasks.add(currentBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
				currentBoxTimesToMove.do({
					{ view.currentBox.valueAction_(currentBoxValue.round(view.step)); }.defer;
					//("currentBoxValue -> " ++ currentBoxValue).postln;
					currentBoxValue = currentBoxValue + currentBoxIncrement;
					currentBoxDeltaTime.wait;
				});
				//runningTasks.remove(currentBoxMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
			}, SystemClock);

			leftBoxMorphTask.start;
			rightBoxMorphTask.start;
			currentBoxMorphTask.start;

			runningTasks.add(leftBoxMorphTask);
			runningTasks.add(rightBoxMorphTask);
			runningTasks.add(currentBoxMorphTask);
		});
	}

	isPlaying {
		^runningTasks.any({ arg item; item.isPlaying; });
	}

	stopAndReset {
		runningTasks.do({ arg each; each.stop.reset; });
	}

	stop {
		runningTasks.do({ arg each; each.stop; });
	}

	reset {
		runningTasks.do({ arg each; each.reset; });
	}

	pause {
		runningTasks.do({ arg each; each.pause; });
	}

	resume {
		runningTasks.do({ arg each; each.resume; });
	}
}