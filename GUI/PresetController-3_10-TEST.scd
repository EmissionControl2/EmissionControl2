// PresetsController Test

///////////////////////////////////////////////////////////////////////////////

// array of ModulationSlider's
(
var	window, joystickControlSetArray, tiltControlSet, zoomSliderControlSet, sliders;
var	joystickWindow, joystick;
var	presetButtons1, presetButtons2, allPresetButtons;
var	presetInterpolator, presetsController;

window = SCWindow.new("< Test Area >", Rect.new(0, 1024, 615, 345), true, true);
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
		joystickControlSetArray[0], joystickControlSetArray[1], false);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;

presetButtons1 = Array.fill(10, { arg i;
	PresetButton.new(window, (i+1).asString, Preset.new(zoomSliderControlSet));
});
presetButtons1.do({ arg each, i; each.draw(70 + (20 * i), 275, 20, 20); });
presetButtons2 = Array.fill(10, { arg i;
	PresetButton.new(window, (i+11).asString, Preset.new(zoomSliderControlSet));
});
presetButtons2.do({ arg each, i; each.draw(70 + (20 * i), 300, 20, 20); });
allPresetButtons = presetButtons1 ++ presetButtons2;
presetInterpolator = PresetInterpolator.new(allPresetButtons, 10, 10);

presetsController = PresetsController.new(window, "presetsController",
		allPresetButtons, presetInterpolator);
presetsController.drawCtrlButtons(140, 255, 20, 0, 20);
presetsController.drawModeButtons(40, 255, 20, 0, 30);
presetsController.drawCurveButtons(210, 255, 20, 0, 30);
presetsController.drawStatusLabel(5, 285, 60, 20);
presetsController.drawTimeZoomSlider(40, 320, \h, 20, 5, 130, 60, \right);
presetsController.drawTimeRangeSlider(70, 295, 200, 5);

presetsController.timeZoomSlider.currentBox.bounds_(Rect.new(275, 285, 60, 20));
presetsController.timeZoomSlider.currentBoxLabel.bounds_(Rect.new(275, 285, 60, 20));
)

///////////////////////////////////////////////////////////////////////////////

// array of ModulationSlider's (explicitly calls setZoomSpec)
(
var	window, joystickControlSetArray, tiltControlSet, zoomSliderControlSet, sliders;
var	joystickWindow, joystick;
var	presetButtons1, presetButtons2, allPresetButtons;
var	presetInterpolator, presetsController;

window = SCWindow.new("< Test Area >", Rect.new(0, 1024, 615, 345), true, true);
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
		joystickControlSetArray[0], joystickControlSetArray[1], false);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
joystickWindow.front;

presetButtons1 = Array.fill(10, { arg i;
	PresetButton.new(window, (i+1).asString, Preset.new(zoomSliderControlSet));
});
presetButtons1.do({ arg each, i; each.draw(70 + (20 * i), 275, 20, 20); });
presetButtons2 = Array.fill(10, { arg i;
	PresetButton.new(window, (i+11).asString, Preset.new(zoomSliderControlSet));
});
presetButtons2.do({ arg each, i; each.draw(70 + (20 * i), 300, 20, 20); });
allPresetButtons = presetButtons1 ++ presetButtons2;
presetInterpolator = PresetInterpolator.new(allPresetButtons, 10, 10);

presetsController = PresetsController.new(window, "presetsController",
		allPresetButtons, presetInterpolator);
presetsController.drawCtrlButtons(140, 255, 20, 0, 20);
presetsController.drawModeButtons(40, 255, 20, 0, 30);
presetsController.drawCurveButtons(210, 255, 20, 0, 30);
presetsController.drawStatusLabel(5, 285, 60, 20);
presetsController.drawTimeZoomSlider(40, 320, \h, 20, 5, 130, 60, \right);
presetsController.drawTimeRangeSlider(70, 295, 200, 5);

presetsController.timeZoomSlider.currentBox.bounds_(Rect.new(275, 285, 60, 20));
presetsController.timeZoomSlider.currentBoxLabel.bounds_(Rect.new(275, 285, 60, 20));
)