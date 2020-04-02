# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.samplerate.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Debug/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Debug/libsamplerate.a


PostBuild.samplerate.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Release/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Release/libsamplerate.a


PostBuild.samplerate.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/MinSizeRel/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/MinSizeRel/libsamplerate.a


PostBuild.samplerate.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/RelWithDebInfo/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/RelWithDebInfo/libsamplerate.a




# For each target create a dummy ruleso the target does not have to exist
