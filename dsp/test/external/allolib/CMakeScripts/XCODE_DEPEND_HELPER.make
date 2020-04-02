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


PostBuild.al.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Debug/libald.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Debug/libald.a


PostBuild.glad.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Debug/libglad.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Debug/libglad.a


PostBuild.glfw.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Debug/libglfw3.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Debug/libglfw3.a


PostBuild.imgui.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libimguid.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libimguid.a


PostBuild.oscpack.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/liboscpackd.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/liboscpackd.a


PostBuild.rtaudio.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a


PostBuild.rtmidi.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a


PostBuild.serial.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a


PostBuild.Gamma.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a


PostBuild.al.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a


PostBuild.glad.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Release/libglad.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Release/libglad.a


PostBuild.glfw.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Release/libglfw3.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Release/libglfw3.a


PostBuild.imgui.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libimgui.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libimgui.a


PostBuild.oscpack.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/liboscpack.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/liboscpack.a


PostBuild.rtaudio.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a


PostBuild.rtmidi.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a


PostBuild.serial.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a


PostBuild.Gamma.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a


PostBuild.al.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a


PostBuild.glad.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/MinSizeRel/libglad.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/MinSizeRel/libglad.a


PostBuild.glfw.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/MinSizeRel/libglfw3.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/MinSizeRel/libglfw3.a


PostBuild.imgui.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libimgui.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libimgui.a


PostBuild.oscpack.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/liboscpack.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/liboscpack.a


PostBuild.rtaudio.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a


PostBuild.rtmidi.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a


PostBuild.serial.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a


PostBuild.Gamma.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a


PostBuild.al.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a


PostBuild.glad.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/RelWithDebInfo/libglad.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/RelWithDebInfo/libglad.a


PostBuild.glfw.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/RelWithDebInfo/libglfw3.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/RelWithDebInfo/libglfw3.a


PostBuild.imgui.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libimgui.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libimgui.a


PostBuild.oscpack.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/liboscpack.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/liboscpack.a


PostBuild.rtaudio.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a


PostBuild.rtmidi.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a


PostBuild.serial.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a




# For each target create a dummy ruleso the target does not have to exist
