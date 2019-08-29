/*
 * David Thall
 * SCNumberBox2.sc
 * CG2 Project
 */

// Extends SCNumberBox, implements...
// 1. Customizable colors
// 2. step-size control (Shift Key)

// DEPRECATED by Jack Kilgore
// Supercollider 3.10 Number Box Extends its functionality beyond
// the extension provided by SCNumberBox2
///////////////////////////////////////////////////////////////////////////////

SCNumberBox2 : NumberBox {
	var	<>defaultStringColor = nil;	// normal display color
	var	<>enterStringColor = nil;	// color when entering text
	var	<>shiftKeyScaler = 10; 		// step scaler when shift is pressed
	var <>keyString = nil;

	*viewClass { ^NumberBox }

	shiftIncrement { this.valueAction = this.value + (step * shiftKeyScaler); }
	shiftDecrement { this.valueAction = this.value - (step * shiftKeyScaler); }

	defaultKeyDownAction { arg char, modifiers, unicode;
		// shift key
		if(modifiers & 131072 == 131072, {
			// standard chardown (arrows)
			if (unicode == 16rF700, { this.shiftIncrement; ^this }); // up
			if (unicode == 16rF703, { this.shiftIncrement; ^this }); // right
			if (unicode == 16rF701, { this.shiftDecrement; ^this }); // down
			if (unicode == 16rF702, { this.shiftDecrement; ^this }); // left
		});

		/*// standard chardown (arrows)
		if (unicode == 16rF700, { this.increment; ^this }); // up arrow (63232)
		if (unicode == 16rF703, { this.increment; ^this }); // right arrow (63235)
		if (unicode == 16rF701, { this.decrement; ^this }); // down arrow (63233)
		if (unicode == 16rF702, { this.decrement; ^this }); // left arrow (63234)

		// enter key / return key / newline key
		if ((char == 3.asAscii) || (char == $\r) || (char == $\n), {
			// if there is no error on repeated entering (e.g., error = 0.1.r)
			if (keyString.notNil,{
				// set the value of the keyString (only if it has changed)
				this.valueAction_(keyString.asFloat);
			});
			^this
		});

		// delete key
		if (char == 127.asAscii, {
			keyString = nil;
			this.string = object.asString;
			this.stringColor = defaultStringColor ? Color.black;
			^this
		});

		// 0 - 9 / +,-,e,E
		if (char.isDecDigit || "+-.eE".includes(char), {
			if (keyString.isNil, {
				keyString = String.new;
				this.stringColor = enterStringColor ? Color.red;
			});
			keyString = keyString.add(char);
			//this.string = keyString;
		});*/
	}

	value_ { arg val;
		keyString = nil;
		this.stringColor = defaultStringColor ? Color.black;
		object = val;
		this.string = object.asString;
	}

}