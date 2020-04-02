# DO NOT EDIT
# This makefile makes sure all linkable targets are
# up-to-date with anything they link to
default:
	echo "Do not invoke directly"

# Rules to remove targets that are older than anything to which they
# link.  This forces Xcode to relink the targets from scratch.  It
# does not seem to check these dependencies itself.
PostBuild.Gamma.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGammad.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGammad.a


PostBuild.Gamma.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a


PostBuild.Gamma.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a


PostBuild.Gamma.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a




# For each target create a dummy ruleso the target does not have to exist
