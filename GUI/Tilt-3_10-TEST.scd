// Tilt Test

///////////////////////////////////////////////////////////////////////////////

// Array of MappableSliders
(
var	window, joystickControlSetArray, tiltControlSet, sliders;

window = Window.new("< Test Area >", Rect.new(0, 1024, 455, 260), true, true);
joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
sliders = Array.fill(10, {
	MappableSlider.new(window, joystickControlSetArray, tiltControlSet); });
sliders.do({ arg each, i;
	each.setZoomSpec("soundfile", 1, 128, 1, 128, \lin, 1, 1, "# ");
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20, \right, \left); });
window.front;
~tilt = TiltControl.new(tiltControlSet, \center, 0.1, 0.1, 20);
)

~tilt.setParameters(\right, 1, 1, 1);
~tilt.start;
~tilt.isPlaying;
~tilt.stopAndReset;
~tilt.stop;
~tilt.reset;
~tilt.pause;
~tilt.resume;


