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


PostBuild.al_soundfile.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a


PostBuild.ecLib.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Debug/libecLib.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/Debug/libecLib.a


PostBuild.emissionControl.Debug:
PostBuild.ecLib.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.al_soundfile.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.al.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.Gamma.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.glfw.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.glad.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.rtaudio.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.rtmidi.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.cpptoml.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.dr_libs.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.nlohmann_json.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.imgui.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.oscpack.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.stb.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.serial.Debug: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
/Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl:\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Debug/libecLib.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Debug/libald.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGammad.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Debug/libglfw3.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Debug/libglad.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libimguid.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/liboscpackd.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a\
	/usr/local/lib/libsndfile.dylib\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/external/libsamplerate/build/libsamplerate.a
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl


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


PostBuild.samplerate.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Debug/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Debug/libsamplerate.a


PostBuild.serial.Debug:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a


PostBuild.Gamma.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a


PostBuild.al.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a


PostBuild.al_soundfile.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a


PostBuild.ecLib.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Release/libecLib.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/Release/libecLib.a


PostBuild.emissionControl.Release:
PostBuild.ecLib.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.al_soundfile.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.al.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.Gamma.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.glfw.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.glad.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.rtaudio.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.rtmidi.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.cpptoml.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.dr_libs.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.nlohmann_json.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.imgui.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.oscpack.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.stb.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
PostBuild.serial.Release: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl
/Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl:\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Release/libecLib.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Release/libglfw3.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Release/libglad.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libimgui.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/liboscpack.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a\
	/usr/local/lib/libsndfile.dylib\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/external/libsamplerate/build/libsamplerate.a
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/emissionControl


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


PostBuild.samplerate.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Release/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/Release/libsamplerate.a


PostBuild.serial.Release:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a


PostBuild.Gamma.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a


PostBuild.al.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a


PostBuild.al_soundfile.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a


PostBuild.ecLib.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/MinSizeRel/libecLib.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/MinSizeRel/libecLib.a


PostBuild.emissionControl.MinSizeRel:
PostBuild.ecLib.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.al_soundfile.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.al.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.Gamma.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.glfw.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.glad.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.rtaudio.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.rtmidi.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.cpptoml.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.dr_libs.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.nlohmann_json.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.imgui.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.oscpack.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.stb.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
PostBuild.serial.MinSizeRel: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl
/Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl:\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/MinSizeRel/libecLib.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/MinSizeRel/libglfw3.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/MinSizeRel/libglad.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libimgui.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/liboscpack.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a\
	/usr/local/lib/libsndfile.dylib\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/external/libsamplerate/build/libsamplerate.a
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/MinSizeRel/emissionControl


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


PostBuild.samplerate.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/MinSizeRel/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/MinSizeRel/libsamplerate.a


PostBuild.serial.MinSizeRel:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a


PostBuild.Gamma.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a


PostBuild.al.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a


PostBuild.al_soundfile.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a


PostBuild.ecLib.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/RelWithDebInfo/libecLib.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/RelWithDebInfo/libecLib.a


PostBuild.emissionControl.RelWithDebInfo:
PostBuild.ecLib.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.al_soundfile.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.al.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.Gamma.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.glfw.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.glad.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.rtaudio.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.rtmidi.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.cpptoml.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.dr_libs.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.nlohmann_json.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.imgui.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.oscpack.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.stb.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
PostBuild.serial.RelWithDebInfo: /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl
/Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl:\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/RelWithDebInfo/libecLib.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/RelWithDebInfo/libglfw3.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/RelWithDebInfo/libglad.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libimgui.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/liboscpack.a\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a\
	/usr/local/lib/libsndfile.dylib\
	/Users/jkilgore/Projects/EmissionControlPort/dsp/external/libsamplerate/build/libsamplerate.a
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/bin/RelWithDebInfo/emissionControl


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


PostBuild.samplerate.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/RelWithDebInfo/libsamplerate.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/libsamplerate/RelWithDebInfo/libsamplerate.a


PostBuild.serial.RelWithDebInfo:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a:
	/bin/rm -f /Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a




# For each target create a dummy ruleso the target does not have to exist
/Users/jkilgore/Projects/EmissionControlPort/dsp/external/libsamplerate/build/libsamplerate.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Debug/libecLib.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/MinSizeRel/libecLib.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/RelWithDebInfo/libecLib.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/Release/libecLib.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Debug/libal_soundfile.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/MinSizeRel/libal_soundfile.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/RelWithDebInfo/libal_soundfile.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/al_ext/soundfile/Release/libal_soundfile.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Debug/libald.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/MinSizeRel/libal.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/RelWithDebInfo/libal.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/Release/libal.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libimguid.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/liboscpackd.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Debug/libserial.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/MinSizeRel/libGamma.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/RelWithDebInfo/libGamma.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGamma.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Gamma/lib/libGammad.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libimgui.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/liboscpack.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/MinSizeRel/libserial.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libimgui.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/liboscpack.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/RelWithDebInfo/libserial.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libimgui.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/liboscpack.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/Release/libserial.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Debug/libglad.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/MinSizeRel/libglad.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/RelWithDebInfo/libglad.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glad/Release/libglad.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Debug/libglfw3.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/MinSizeRel/libglfw3.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/RelWithDebInfo/libglfw3.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/glfw/src/Release/libglfw3.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Debug/librtaudio.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/MinSizeRel/librtaudio.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/RelWithDebInfo/librtaudio.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtaudio/Release/librtaudio.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Debug/librtmidi.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/MinSizeRel/librtmidi.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/RelWithDebInfo/librtmidi.a:
/Users/jkilgore/Projects/EmissionControlPort/dsp/test/external/allolib/external/rtmidi/Release/librtmidi.a:
/usr/local/lib/libsndfile.dylib:
