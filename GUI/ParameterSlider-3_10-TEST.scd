// ParameterSlider Test

///////////////////////////////////////////////////////////////////////////////

// array of ParameterSlider's
(
var	window, joystickControlSetArray, tiltControlSet, sliders;
var	joystickWindow, joystick;

window = Window.new("< Test Area >", Rect.new(0, 1024, 480, 260), true, true);
joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
sliders = Array.fill(10, {
	ParameterSlider.new(window, joystickControlSetArray, tiltControlSet,
			"modulation amount", 0, 100, 0, 100, \lin, 0.01, 0, "% "); });
sliders.do({ arg each, i;
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20); });
window.front;

joystickWindow = Window.new("< Joystick 1 >",
		Rect.new(485, 1024, 235, 160), true, true);
joystick = Joystick.new(joystickWindow, "", Color.new255(255, 192, 203),
		joystickControlSetArray[0], joystickControlSetArray[1], false);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;
)

// array of ParameterSlider's (explicitly calls setZoomSpec)
(
var	window, joystickControlSetArray, tiltControlSet, sliders;
var	joystickWindow, joystick;

window = Window.new("< Test Area >", Rect.new(0, 1024, 480, 260), true, true);
joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
sliders = Array.fill(10, {
	ParameterSlider.new(window, joystickControlSetArray, tiltControlSet); });
sliders.do({ arg each, i;
	each.setZoomSpec("modulation amount", 0, 100, 0, 100, \lin, 0.01, 0, "% ");
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20); });
window.front;

joystickWindow = Window.new("< Joystick 1 >",
		Rect.new(485, 1024, 235, 160), true, true);
joystick = Joystick.new(joystickWindow, "", Color.new255(255, 192, 203),
		joystickControlSetArray[0], joystickControlSetArray[1], false);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;
)
