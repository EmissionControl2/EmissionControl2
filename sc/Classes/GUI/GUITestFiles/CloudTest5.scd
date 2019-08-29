(
var	controllerWindow;
var	joystickControlSetArray, tiltControlSet, zoomSliderControlSet;
var	horizontalDivider, verticalDivider;
var	labelsSpec, labels;
var	modulationSlidersSpec, modulationSliders;
var	parameterSlidersSpec, parameterSliders;
var	joystickWindows;
var	joysticksSpec, joysticks;
var	tiltButtonLeft, tiltButtonCenter, tiltButtonRight;
var	presetButtonArray, presetInterpolator, presetsController;

var	path, buffer, sfInfo, sfDur;
var	synthDef, synth;
var	lfoWindow, lfoSlidersSpec, lfoSliders;

controllerWindow = Window.new("< Controller >", Rect.new(0, 0, 1245, 315), false, true);
controllerWindow.view.background = HiliteGradient.new(Color.new255(69, 166, 247), Color.new255(0, 82, 155), \h, 2048, 0.5);

joystickControlSetArray = Array.fill(16, { ControlSet.new; });
tiltControlSet = ControlSet.new;
zoomSliderControlSet = ControlSet.new; // initially empty (for Preset)

horizontalDivider = StaticText.new(controllerWindow, Rect.new(5, 5 + (25 * 12), 1235, 5));
horizontalDivider.background = HiliteGradient.new(Color.clear, Color.black, \h, 2048, 0.5);
verticalDivider = StaticText.new(controllerWindow, Rect.new(620, 5 + (25 * 2), 5, 255));
verticalDivider.background = HiliteGradient.new(Color.clear, Color.black, \v, 1024, 1);
labelsSpec = [
	[Rect.new(5, 5 + (25 * 1), 130, 20), "modulation source"],
	[Rect.new(190, 5 + (25 * 1), 20, 20), "x/y"],
	[Rect.new(280, 5 + (25 * 1), 200, 20), "modulation amount"],
	[Rect.new(550, 5 + (25 * 1), 145, 20), "current"],
	[Rect.new(765, 5 + (25 * 1), 200, 20), "parameters"],
	[Rect.new(1035, 5 + (25 * 1), 20, 20), "x/y"],
	[Rect.new(1110, 5 + (25 * 1), 130, 20), "options"]
];
labels = labelsSpec.collect({ arg each;
	StaticText.new(controllerWindow, each[0])
	.string_(each[1])
	.font_(Font.new("Helvetica", 12))  					.align_(\center)
	.stringColor_(Color.black.alpha_(0.7));
});

modulationSlidersSpec = ["soundfile", "density", "duration", "pitch", "time", "phase", "space", "filter", "resonance", "amplitude"];

modulationSliders = Array.fill(10, { ModulationSlider.new(controllerWindow, joystickControlSetArray, tiltControlSet); });

modulationSliders.do({ arg each, i;
	each.setZoomSpec(modulationSlidersSpec[i], 0, 100, 0, 100, \lin, 0.01, 0, "% ");
	each.addModSrc(["random", "variable saw", "random", "random", "variable saw", "variable saw", "random", "random", "random", "random"][i%10]);
	each.modSrcPopUpMenu.value_(10);
	each.draw(5, 5 + (25 * (i + 2)), 20, 5, 200, 60, 20, 80);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

parameterSlidersSpec = [
	["soundfile", 1, 128, 1, 10, \lin, 1, 1, "# "],
	["density", 1, 1000, 1, 200, \exp, 1, 10, "/s "],
	["duration", 1, 10000, 1, 1000, \exp, 1, 100, "ms "],
	["pitch", -48, 24, -24, 24, \sin, 0.01, 0, "ht "],
	["time", 0.1, 400, 25, 200, \exp, 0.1, 100, "% "],
	["phase", 0, 100, 0, 100, \lin, 0.01, 0, "% "],
	["space", -180, 180, -180, 180, \lin, 0.1, 0, 161.asAscii.asString ++ " "],
	["filter", 60, 18000, 60, 5000, \exp, 1, 2000, "Hz "],
	["resonance", 0.5, 1000, 1, 500, \exp, 0.1, 1, "Q "],
	["amplitude", -120, 12, -120, 0, \sin, 0.1, 0, "dB "]
];

parameterSliders = Array.fill(10, { ParameterSlider.new(controllerWindow, joystickControlSetArray, tiltControlSet); });

parameterSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(parameterSlidersSpec[i][j], parameterSlidersSpec[i][j+1], parameterSlidersSpec[i][j+2],
			parameterSlidersSpec[i][j+3], parameterSlidersSpec[i][j+4], parameterSlidersSpec[i][j+5],
			parameterSlidersSpec[i][j+6], parameterSlidersSpec[i][j+7], parameterSlidersSpec[i][j+8]);
	each.draw(635, 5 + (25 * (i + 2)), 20, 5, 200, 60, 20);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

joystickWindows = Array.fill(8, { arg i;
	if(i < 4, {
		Window.new("< Joystick " ++ (i+1) ++ " >", Rect.new(0 + (205 * i), Window.screenBounds.height, 200, 150), true, true);
	}, {
		Window.new("< Joystick " ++ (i+1) ++ " >", Rect.new(0 + (205 * (i-4)), Window.screenBounds.height - 370, 200, 150), true, true);
	});
});

joysticksSpec = [
	[Color.new255(255, 192, 203), joystickControlSetArray[0], joystickControlSetArray[1]], // pink
	[Color.red.alpha_(0.3), joystickControlSetArray[2], joystickControlSetArray[3]],       // red
	[Color.new255(255, 165, 0), joystickControlSetArray[4], joystickControlSetArray[5]],   // orange
	[Color.yellow.alpha_(0.3);, joystickControlSetArray[6], joystickControlSetArray[7]],   // yellow
	[Color.green.alpha_(0.3), joystickControlSetArray[8], joystickControlSetArray[9]],     // green
	[Color.blue.alpha_(0.3), joystickControlSetArray[10], joystickControlSetArray[11]],    // blue
	[Color.new255(160, 32, 240), joystickControlSetArray[12], joystickControlSetArray[13]],// purple
	[Color.gray.alpha_(0.3);, joystickControlSetArray[14], joystickControlSetArray[15]]    // gray
];

joysticks = Array.fill(8, { arg i; Joystick.new(joystickWindows[i], "", joysticksSpec[i][0], joysticksSpec[i][1], joysticksSpec[i][2], false); });

joysticks.do({ arg each; each.draw(5, 5, 20, 5, \bottom, \right, \top, true); });

tiltButtonLeft = ECTiltButton.new(controllerWindow, "TILT", ECTilt.new(tiltControlSet, \left, 0.1, 0.1, 4));
tiltButtonCenter = ECTiltButton.new(controllerWindow, "TILT", ECTilt.new(tiltControlSet, \center, 0.1, 0.1, 4));
tiltButtonRight = ECTiltButton.new(controllerWindow, "TILT", ECTilt.new(tiltControlSet, \right, 0.1, 0.1, 4));

[tiltButtonLeft, tiltButtonCenter, tiltButtonRight].do({ arg each, i;
	each.draw([5, 600, 1195].at(i), 5, 40, 20);
});

tiltButtonCenter.button.visible = false;
/*
presetButtonArray = Array.fill(20, { arg i; PresetButton.new(window, (i + 1).asString, Preset.new(zoomSliderControlSet)) });
presetButtonArray.do({ arg each, i; each.draw(422 + (20 * i), 5 + (25 * 1), 20, 20); });

presetInterpolator = PresetInterpolator.new(presetButtonArray);

presetsController = PresetsController.new(window, "presetsController", presetButtonArray, presetInterpolator);
presetsController.drawCtrlButtons(145, 525, 20, 0, 20);
presetsController.drawModeButtons(45, 525, 20, 0, 30);
presetsController.drawCurveButtons(215, 525, 20, 0, 30);
presetsController.drawStatusLabel(30, 555, 40, 20);
presetsController.drawTimeZoomSlider(45, 590, \h, 20, 5, 170, 40, \right);
presetsController.drawTimeRangeSlider(75, 565, 200, 5);
*/
controllerWindow.front;
joystickWindows.do({ arg each; each.front; });

controllerWindow.onClose = { arg controllerWindow;
	Window.closeAll;
	//s.sendBundle(nil, ["/n_free", 1000]);
	synth.free(true); // this doesn't always seem to call the garbage collector, or the nodeID isn't getting removed...
};

s = Server.internal;
Server.default = s;
s.boot;

path = Platform.resourceDir +/+ "sounds/a11wlk01.wav";
buffer = Buffer.read(s, path);
sfInfo = SoundFile.new;
sfInfo.openRead(path);
sfDur = sfInfo.duration;
("sfDur = " ++ sfDur).postln;

/* JKilg Comment
s.sendMsg(\b_allocRead, 1, "sounds/a11wlk01.wav");
s.sendMsg(\b_allocRead, 2, "sounds/Anne&Sophoclis.aiff");
s.sendMsg(\b_allocRead, 3, "sounds/Aram.aiff");
s.sendMsg(\b_allocRead, 4, "sounds/Beethoven.aiff");
s.sendMsg(\b_allocRead, 5, "sounds/Bell.aiff");
s.sendMsg(\b_allocRead, 6, "sounds/DrumLP 120bpm");
s.sendMsg(\b_allocRead, 7, "sounds/english_daisy.wav");
s.sendMsg(\b_allocRead, 8, "sounds/RudyGuitar.aiff");
s.sendMsg(\b_allocRead, 9, "sounds/Traviata.aiff");
s.sendMsg(\b_allocRead, 10, "sounds/welcome.man.wav");
*/

synthDef = SynthDef.new("CloudTest1", {
	arg	outBusNum = 0,
		soundfile = 1, density = 10, duration = 100, pitch = 0, time = 100, // specified in gui param units (converted within)
		phase = 0, space = 0, filter = 2000, resonance = 1, amplitude = 0,
		soundfileModAmt = 0, densityModAmt = 0, durationModAmt = 0, pitchModAmt = 0, timeModAmt = 0,
		phaseModAmt = 0, spaceModAmt = 0, filterModAmt = 0, resonanceModAmt = 0, amplitudeModAmt = 0,
		modFreq = 1, modWidth = 0;
	var	modSrc1, modSrc2;
	var	numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp;
	modSrc1 = WhiteNoise.ar; // random
	modSrc2 = VarSaw.ar(modFreq, 0, (modWidth * 0.01), 2, -1); // varsaw
	numChannels = 2; // stereo
	trigger = Impulse.ar(density * (2 ** (modSrc2 * (densityModAmt * 0.1))));
	bufnum = soundfile * (2 ** (modSrc1 * (soundfileModAmt * 0.1))); // this must be rounded to an integer!!! (somehow it still seems to work...however, it must be scaled properly!)
	rate = (pitch.midiratio) * (2 ** (modSrc1 * (pitchModAmt * 0.1)));
	centerPos = LFSaw.kr(((BufDur.kr(bufnum).reciprocal) * (time * 0.01)) * (2 ** (modSrc1 * (timeModAmt * 0.1))), -1, 0.5, 0.5) * BufDur.kr(bufnum); // time: works!!!
	//centerPos = LFSaw.kr((BufDur.kr(bufnum).reciprocal) * (time * 0.01), -1, 0.5, 0.5) * BufDur.kr(bufnum); // time: works!!!
	//centerPos = LFSaw.kr(scanRate * (2 ** (modSrc2 * (scanRateModAmt * 0.1))), -1, 0.5, 0.5) * BufDur.kr(bufnum); // scanRate: works!!!
	dur = (duration * 0.001) * (2 ** (modSrc1 * (durationModAmt * 0.1)));
	pan = ((space / 180) * 0.999).neg + (modSrc1 * (spaceModAmt * 0.01)); // quick fix for pan problems: [-1 -> 1] is reversed [r -> l], & extreme edge wraps to other side
	//amp = 1.0;
	interp = 2;
	freq = (filter * (2 ** (modSrc1 * (filterModAmt * 0.1)))).max(20).min(18000);
	rq = ((resonance.reciprocal) * (2 ** (modSrc1 * (resonanceModAmt * 0.1)))).max(0.001).min(1);
	gainComp = 1;
	//amp = 1.0;
	//amp = 1.0 * rq.reciprocal.min(10); // old compensation
	//amp = 1.0 + (rq.reciprocal * 0.1); // see SC1 SweepingGranQ p.2
	amp = ((amplitude.dbamp) * (2 ** (modSrc1 * (amplitudeModAmt * 0.1)))).min(4.0) * rq.reciprocal ** 0.7; // see SC2 CVox_ConstQ
	Out.ar(outBusNum, RCloud.ar(numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp));
}).load(s);

//s.sendMsg("\s_new", "CloudTest1", -1, 1, 0, \outBus, 0);
synth = Synth.new("CloudTest1", [\outBus, 0, \soundfile, 1, \density, 10, \duration, 100, \pitch, 0, \time, 100, \phase, 0, \space, 0, \filter, 2000, \resonance, 1, \amplitude, 0], nil, \addToHead);
//synth.nodeID = 1000;
//("synth.nodeID -> " ++ synth.nodeID).postln;

lfoWindow = Window.new("< Variable Saw LFO >", Rect.new(0, 405, 455, 55), false, true);
lfoWindow.view.background = HiliteGradient.new(Color.new255(69, 166, 247), Color.new255(0, 82, 155), \h, 2048, 0.5);

lfoSlidersSpec = [
	["modFreq", 0.01, 25, 0.01, 25, \exp, 0.01, 1, "Hz "],
	["modWidth", 0, 100, 0, 100, \lin, 0.1, 0, "% "]
];

lfoSliders = Array.fill(2, { MappableSlider.new(lfoWindow, joystickControlSetArray, tiltControlSet); });

lfoSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(lfoSlidersSpec[i][j], lfoSlidersSpec[i][j+1], lfoSlidersSpec[i][j+2],
			lfoSlidersSpec[i][j+3], lfoSlidersSpec[i][j+4], lfoSlidersSpec[i][j+5],
			lfoSlidersSpec[i][j+6], lfoSlidersSpec[i][j+7], lfoSlidersSpec[i][j+8]);
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, \right, \left);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

lfoWindow.front;

modulationSliders[0].plugIntoSynth(synth, \soundfileModAmt);
modulationSliders[1].plugIntoSynth(synth, \densityModAmt);
modulationSliders[2].plugIntoSynth(synth, \durationModAmt);
modulationSliders[3].plugIntoSynth(synth, \pitchModAmt);
modulationSliders[4].plugIntoSynth(synth, \timeModAmt);
modulationSliders[5].plugIntoSynth(synth, \phaseModAmt);
modulationSliders[6].plugIntoSynth(synth, \spaceModAmt);
modulationSliders[7].plugIntoSynth(synth, \filterModAmt);
modulationSliders[8].plugIntoSynth(synth, \resonanceModAmt);
modulationSliders[9].plugIntoSynth(synth, \amplitudeModAmt);

parameterSliders[0].plugIntoSynth(synth, \soundfile);
parameterSliders[1].plugIntoSynth(synth, \density);
parameterSliders[2].plugIntoSynth(synth, \duration);
parameterSliders[3].plugIntoSynth(synth, \pitch);
parameterSliders[4].plugIntoSynth(synth, \time);
parameterSliders[5].plugIntoSynth(synth, \phase);
parameterSliders[6].plugIntoSynth(synth, \space);
parameterSliders[7].plugIntoSynth(synth, \filter);
parameterSliders[8].plugIntoSynth(synth, \resonance);
parameterSliders[9].plugIntoSynth(synth, \amplitude);

lfoSliders[0].plugIntoSynth(synth, \modFreq);
lfoSliders[1].plugIntoSynth(synth, \modWidth);

)

///////////////////////////////////////////////////////////////////////////////