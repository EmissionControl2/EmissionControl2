# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.al_soundfile.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a


PostBuild.al_soundfile.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a


PostBuild.al_soundfile.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a


PostBuild.al_soundfile.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a




# For each target create a dummy ruleso the target does not have to exist
