/*
 * David Thall
 * Debug.sc
 * CG2 Project
 */

// The Debug Class is used to selectively debug different pieces of code within
// a project.  Currently it prints all debug info to the standard output.
// In the future, it would be good to give each group a name, a new document
// (w/ a title), an erase function to overwrite old debugging text after 'x'
// statements, and a background and stringColor.

// It might be good to have debugging (i.e., printing comments) turned on/off
// per method or .action function!!!

/*
use as follows...
	Debug.new(0, "I do the debugging...");
	Debug.debugOn[0] = true; // posts: Dictionary[ (0 -> true) ]
	Debug.debugOn[0] = false;// posts: Dictionary[ (0 -> false) ]
	Debug.new(0, "I do the debugging...");
*/

// 79 chars
///////////////////////////////////////////////////////////////////////////////

Debug {
	classvar <>debugOn; // turns on/off debugging feature

	// called at library initialization time
	*initClass {
		// key is 'group', value is 'state' (true/false)
		debugOn = Dictionary.new;
	}

	// create a new Debug
	*new { arg group = 0, string;

		// check if the group is already in the dictionary
		if(debugOn.at(group).isNil, {
			// if it isn't (i.e.,isNil),
			// add it to dictionary,
			// set group to 'true'
			debugOn.put(group, true);
		});

		// it the group's value is 'true' (i.e., group is 'on')
		if(debugOn.at(group), {
			// print string
			string.postln;
		});
	}
}