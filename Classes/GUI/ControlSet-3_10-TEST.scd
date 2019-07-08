// ControlSet Test

///////////////////////////////////////////////////////////////////////////////

(
var	sliderWindow, sliders;
var	joystickWindow, joystick;

sliderWindow = Window.new("< Test Area >", Rect.new(0, 1024, 455, 260));
~joystickControlSetArray = Array.fill(16, { ControlSet.new; });
~tiltControlSet = ControlSet.new;
sliders = Array.fill(10, {
	MappableSlider.new(sliderWindow, ~joystickControlSetArray, ~tiltControlSet);
});
sliders.do({ arg each, i;
	each.setZoomSpec("soundfile", 1, 128, 1, 128, \lin, 1, 1, "# ");
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20, \right, \left); });
sliderWindow.front;

joystickWindow = Window.new("< Joystick 1 >", Rect.new(460, 1024, 235, 160));
joystick = Joystick.new(joystickWindow, "", Color.new255(255, 192, 203),
		~joystickControlSetArray[0], ~joystickControlSetArray[1],false);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;
)

~array = Array.new(8);
~tiltControlSet.do({ arg each; each.postln; });
~tiltControlSet.add(~array);
~tiltControlSet.do({ arg each; each.postln; });
~tiltControlSet.remove(~array);
~tiltControlSet.do({ arg each; each.postln; });
