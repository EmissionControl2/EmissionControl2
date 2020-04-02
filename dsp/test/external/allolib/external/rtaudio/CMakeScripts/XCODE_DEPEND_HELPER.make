# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.rtaudio.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a


PostBuild.rtaudio.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a


PostBuild.rtaudio.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a


PostBuild.rtaudio.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a




# For each target create a dummy ruleso the target does not have to exist
