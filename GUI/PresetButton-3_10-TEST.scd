// PresetButton Test

///////////////////////////////////////////////////////////////////////////////

// array of ModulationSlider's
(
var	window, joystickControlSetArray, tiltControlSet, zoomSliderControlSet, sliders;
var	joystickWindow, joystick;
var	presetButtons;

window = SCWindow.new("< Test Area >", Rect.new(0, 1024, 615, 280), true, true);
joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
zoomSliderControlSet = ControlSet.new; // initially empty
sliders = Array.fill(10, {
	ModulationSlider.new(window, joystickControlSetArray, tiltControlSet,
			"modulation amount", 0, 100, 0, 100, \lin, 0.01, 0, "% "); });
sliders.do({ arg each, i;
	each.addModSrc("barberpole");
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20, 80);
	zoomSliderControlSet.add(each);
});
window.front;

joystickWindow = SCWindow.new("< Joystick 1 >",
		Rect.new(620, 1024, 235, 160), true, true);
joystick = Joystick.new(joystickWindow, "", Color.new255(255, 192, 203),
		joystickControlSetArray[0], joystickControlSetArray[1]);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;

presetButtons = Array.fill(10, { arg i;
	PresetButton.new(window, (i+1).asString, Preset.new(zoomSliderControlSet));
});
presetButtons.do({ arg each, i; each.draw(5 + (25 * i), 255, 20, 20); });
)

// array of ModulationSlider's (explicitly calls setZoomSpec)
(
var	window, joystickControlSetArray, tiltControlSet, zoomSliderControlSet, sliders;
var	joystickWindow, joystick;
var	presetButtons;

window = SCWindow.new("< Test Area >", Rect.new(0, 1024, 615, 280), true, true);
joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
zoomSliderControlSet = ControlSet.new; // initially empty
sliders = Array.fill(10, {
	ModulationSlider.new(window, joystickControlSetArray, tiltControlSet); });
sliders.do({ arg each, i;
	each.setZoomSpec("modulation amount", 0, 100, 0, 100, \lin, 0.01, 0, "% ");
	each.addModSrc("barberpole");
	each.draw(5, 5 + ( 25 * i), 20, 5, 200, 60, 20, 80);
	zoomSliderControlSet.add(each);
});
window.front;

joystickWindow = SCWindow.new("< Joystick 1 >",
		Rect.new(620, 1024, 235, 160), true, true);
joystick = Joystick.new(joystickWindow, "", Color.new255(255, 192, 203),
		joystickControlSetArray[0], joystickControlSetArray[1]);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;

presetButtons = Array.fill(10, { arg i;
	PresetButton.new(window, (i+1).asString, Preset.new(zoomSliderControlSet));
});
presetButtons.do({ arg each, i; each.draw(5 + (25 * i), 255, 20, 20); });
)