# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.rtmidi.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a


PostBuild.rtmidi.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a


PostBuild.rtmidi.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a


PostBuild.rtmidi.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a




# For each target create a dummy ruleso the target does not have to exist
