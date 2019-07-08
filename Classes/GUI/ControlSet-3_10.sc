/*
 * David Thall
 * ControlSet.sc
 * CG2 Project
 */

// A ControlSet is a 'transaction' class object.  It handles all communication
// between a MappableSlider, Joystick, Tilt, or Preset.  A ControlSet instance
// is a wrapper on a Set, which is a collection of objects, no 2 of which are
// equal.  The contents of a Set instance are 'unordered'... therefore, you
// must not depend on the order of the items in the Set instance!

// A MappableSlider instance adds or removes itself from a ControlSet instance
// A Joystick will send its normalized x and y outputs to a ControlSet instance,
// which will then forward these control values to anyone currently registered
// The same is true for Tilt and Preset.

// NOTE: This should probably be scrapped for Set!!!
// The only other solution would be to have this subclass Set!

// 79 chars
///////////////////////////////////////////////////////////////////////////////

ControlSet {
	// Set reference
	var	<set;

	// called once at library compilation time
	*initClass { /* set classvar's in here ??? */ }

	// create a new ControlList
	*new { ^super.new.initControlList;
	}

	/* PRIVATE IMPLEMENTATION */

	// get the ZoomSlider instance into a consistent state
	initControlList {
		set = Set.new;
		("set -> " ++ set).postln;
	}

	/* PUBLIC INTERFACE */ // (override or extend these in subclasses...)

	// add anObject to the Set
	// (an object which is equal to an object already
	// in the Set will not be added)
	add { arg view;
		set.add(view);
		("set -> " ++ set).postln;
	}

	// remove anObject from the Set
	remove { arg view;
		set.remove(view);
			("set -> " ++ set).postln;
	}

	// evaluates some function for each item in the Set
	do { arg function;
		set.do(function); // this works, but is it in correct form ???
	}

	// answers the number of objects contained in the Set
	size {
		// this is the slow way. Most collections have a faster way.
		var tally = 0;
		this.do({
			tally = tally + 1;
		});
		^tally
	}
}