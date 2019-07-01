// SliderOptions Test

///////////////////////////////////////////////////////////////////////////////

// SliderOptions Test
(
var window, parameterSlidersArraySpec, parameterSliders, sliderOptions;
window = Window.new("", Rect.new(256, 256, 615, 255));
parameterSlidersArraySpec = [
	["soundfile (#)", 1, 128, 1, 128, \lin, 1, 1],
	["density (g/s)", 0.1, 1000, 0.1, 1000, \exp, 0.1, 10],
	["duration (ms)", 1, 10000, 1, 10000, \exp, 1, 100],
	["pitch (ht)", -48, 24, -48, 24, \sin, 0.01, 0],
	["time (%)", 0.1, 400, 0.1, 400, \exp, 0.1, 100],
	["phase (%)", 0, 100, 0, 100, \lin, 0.01, 0],
	["space(" ++ 161.asAscii.asString ++ ")", -180, 180, -180, 180, \lin, 0.1, 0],
	["filter (Hz)", 60, 18000, 60, 18000, \exp, 1, 2000],
	["resonance (Q)", 0.5, 1000, 0.5, 1000, \exp, 0.1, 1],
	["amplitude (dB)", 0.001.ampdb, 1.ampdb, 0.001.ampdb, 1.ampdb, \sin, 0.1, 1.ampdb]
];

parameterSliders = parameterSlidersArraySpec.collect({ arg e;
	ParameterSlider.new(window,[ControlSet.new, ControlSet.new],ControlSet.new, e[0], e[1], e[2], e[3], e[4], e[5], e[6], e[7]);
});

parameterSliders.do({ arg each, i;
	each.draw(5, 5 + (25 * i), 20, 5, 260, 40, 20);
});
sliderOptions = SliderOptions.new(window, "", parameterSliders);
sliderOptions.drawDensityRow(480, 5 + (25 * 1), 20, 5, 85, 40);
sliderOptions.drawPitchRow(480, 5 + (25 * 3), 20, 5, 40);
sliderOptions.drawTimeRow(480, 5 + (25 * 4), 20, 5, 40, 85);
sliderOptions.drawPhaseRow(480, 5 + (25 * 5), 130, 20);
sliderOptions.drawAmplitudeRow(480, 5 + (25 * 9), 20, 5, 60);
window.front;
)

