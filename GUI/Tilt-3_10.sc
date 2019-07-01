/*
 * David Thall
 * Tilt.sc (Bump???)
 * CG2 Project
 */

// A Tilt spawns tasks that update
// ZoomSlider's toward target values (constant rate algorithm)

// Tilt models the motion of rigid objects (in this case... sliders) when a jolt
// is applied at one side (similar to the 'Tilt' function created when
// playing Pinball!)

// Is there a way to grab a moving slider and kill the inner routine?
// (App/SysClock.clear)
// When the rate == 0, the tiltDuration == 0...
// Tilt acts as a directional 'Random' button limited in range by maxAmount!!!

// Update: shaking should be a global routine outside inner slider update routine
// Update:  If the Tilt instance is already playing, I shouldn't start again!!!
// Update: I should move type arg to method start!!!
// Add: velocity and acceleration scalings
// Add: weight factor, random motion, edge bouncing factor,
// flashing lights, localized wobble...
// Add: a boolean to turn on/off edge bouncing!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

Tilt {
	// state variables
	var	<type = nil;				// tilt direction (left, center, right)
	var	<maxAmount = nil;			// distance to travel
	var	<rate = nil;				// rate at which to go the distance (Hz)
	var	<maxStepsPerSec = nil;		// max num of steps to get to destination
	// Tilt ControlSet
	var	<tiltControlSet = nil;		// Tilt ControlSet
	// running Tasks
	var	<runningTasks = nil;		// Array of running tasks

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create a new Tilt
	*new { arg argTiltControlSet,
			argType = \center,
			argMaxAmount = 0.1, argRate = 0.1,
			argMaxStepsPerSec = 20;
		if(argTiltControlSet.isNil, {
			(this.asString ++ ": " ++
					"A ControlSet must be provided!").postln;
			^nil;
		});
		^super.new.initTilt(argTiltControlSet,
				argType,
				argRate, argMaxAmount,
				argMaxStepsPerSec);
	}

	/* PRIVATE IMPLEMENTATION */

	// get the Tilt instance into a consistent state
	initTilt { arg argTiltControlSet,
			argType,
			argMaxAmount, argRate,
			argMaxStepsPerSec;
		this.initTiltStateVariables(argTiltControlSet,
				argType,
				argMaxAmount, argRate,
				argMaxStepsPerSec);
		//this.setupRunningTasksArray;
	}

	// initialize all the internal state variables
	initTiltStateVariables { arg argTiltControlSet,
			argType,
			argMaxAmount, argRate,
			argMaxStepsPerSec;

		this.setParameters(argType,
				argMaxAmount, argRate,
				argMaxStepsPerSec);

		tiltControlSet = argTiltControlSet;
		("tiltControlSet -> " ++ tiltControlSet).postln;

		runningTasks = Array.new;
		("runningTasks -> " ++ runningTasks).postln;
	}

	/*
	// setup the runningTasks array (do this only at init time!)
	setupRunningTasksArray {
		var	numViewsToUpdate = nil;
		numViewsToUpdate = tiltControlSet.size;

		("numViewsToUpdate -> " ++ numViewsToUpdate).postln;

		// create a new array to reference all running tasks
		runningTasks = Array.new(numViewsToUpdate);
		("runningTasks -> " ++ runningTasks).postln;
	}
	*/

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// initialize the internal state variables
	setParameters { arg argType, argMaxAmount, argRate, argMaxStepsPerSec;
		type = argType;
		maxAmount = argMaxAmount.abs;
		rate = argRate.abs;
		maxStepsPerSec = argMaxStepsPerSec.abs;

		("type -> " ++ type).postln;
		("maxAmount -> " ++ maxAmount).postln;
		("rate -> " ++ rate).postln;
		("maxStepsPerSec -> " ++ maxStepsPerSec).postln;
	}

	// constant-rate interpolation algorithm
	start {
		/**********/ // setupRunningTasksArray
		var	numViewsToUpdate = nil;
		numViewsToUpdate = tiltControlSet.size;

		("numViewsToUpdate -> " ++ numViewsToUpdate).postln;

		// create a new array to reference all running tasks (each time)
		runningTasks = Array.new(numViewsToUpdate);
		("runningTasks -> " ++ runningTasks).postln;
		/**********/

		tiltControlSet.do({ arg view;
			var	startPos = nil;		// slider starting position
			var	totalDistance = nil;	// total distance to travel
			var	tiltDur = nil;		// interpolation duration
			var	timesToMove = nil;		// number of steps to take
			var	increment = nil;		// size of a step
			var	deltaTime = nil;		// wait time between steps

			var	sliderMorphTask = nil; // Task reference

			// get the current unmapped slider value
			startPos = view.slider.value;
			// distance for slider to travel, in either direction
			totalDistance =
				if(type == \center, { // left or right
					(maxAmount.asFloat).rand2;
				}, {
					if(type == \right, { // right only
						(maxAmount.asFloat).rand;
					}, {
						if(type == \left, { // left only
							(maxAmount.asFloat).rand.neg;
						});
					});
				});
			// total duration in seconds (can be 0)
			tiltDur = if(rate == 0, { 0; }, { (totalDistance.abs / rate); });
			// number of moves (1 <= timesToMove <= maxStepsPerSec)
			timesToMove = ((maxStepsPerSec * tiltDur).asInteger).max(1);
			// distance of a single move (in either the pos or neg direction)
			increment = timesToMove.reciprocal * totalDistance;
			// wait time between moves
			deltaTime = tiltDur/timesToMove;
			//("startPos -> " ++ startPos).postln;
			//("totalDistance -> " ++ totalDistance).postln;
			//("tiltDur -> " ++ tiltDur).postln;
			//("timesToMove -> " ++ timesToMove).postln;
			//("increment -> " ++ increment).postln;
			//("deltaTime -> " ++ deltaTime).postln;
			// define a Task thread...
			sliderMorphTask = Task.new({
				// add this Task to the runningTasks array
				//runningTasks.add(sliderMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
				// loop...
				timesToMove.do({
					// update the position (fold into range)
					{ view.slider.valueAction_(startPos.fold(0, 1)); }.defer;
					//("startPos -> " ++ startPos).postln;
					// calculate next step position (inc can be pos or neg)
					startPos = startPos + increment; 					deltaTime.wait;
				});
				// remove this Task to the runningTasks array
				//runningTasks.remove(sliderMorphTask);
				//("runningTasks -> " ++ runningTasks).postln;
			}, SystemClock);

			// start/spawn the Task...
			sliderMorphTask.start;

			runningTasks.add(sliderMorphTask);
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