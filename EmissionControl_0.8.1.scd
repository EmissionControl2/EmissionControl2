/* Emission Control 0.8.1 */

// 1. Double-click to select. Hit the enter key to boot the sound server.
(
s = Server.local;
Server.default = s;
s.boot;
)

// 2. Double-click to select. Hit the enter key to preload soundfiles.
// To add mono .aif or .wav soundfiles, simply copy and paste, specifying the
// soundfile index (beginning at 1) and relative path. You may add up to 128
// soundfiles. Set 'c' equal to the total.
(
s.sendMsg(\b_allocRead, 1, "sounds/pluck.aiff");
s.sendMsg(\b_allocRead, 2, "sounds/noise.aiff");
c = 2;
)

///////////////////////////////////////////////////////////////////////////////

// 3. Double-click to select. Hit the enter key to start the application.
// Note: SC3 does not use a multi-threaded startup loop.  This sometimes causes
// the interface to load before the soundfile(s) and synthesizer have been loaded
// onto the server. The simple solution is to close the application by closing
// the main window (which will close all the other windows automatically), and
// then repeat the startup procedure.
(
var	controlPanel;
var	horizontalDivider, verticalDivider;
var	joystickControlSetArray, tiltControlSet, zoomSliderControlSet;
var	labelsSpec, labels;
var	modulationSlidersSpec, modulationSliders;
var	parameterSlidersSpec, parameterSliders;
var	joystickPanels;
var	joysticksSpec, joysticks;
var	outputPanel;
var	outputSlidersSpec, outputSliders;
var	fxPanel;
var	fxSlidersSpec, fxSliders;
var	modPanel;
var	modSlidersSpec, modSliders;
var	modSynthDef, modSynth;
var 	cloudSynthDef, cloudSynth;
var	reverbSynthDef, reverbSynth;

controlPanel = Window.new("< Emission Control >",
						Rect.new(0, 0, 1245, 490 - 25), false, true);
//controlPanel.view.background = HiliteGradient.new(
//	Color.new255(69, 166, 247),
//	Color.new255(0, 82, 155), \h, 2048, 0.5);

horizontalDivider = StaticText.new(controlPanel, Rect.new(5, 5 + (25 * 18), 1235, 5));
horizontalDivider.background = HiliteGradient.new(Color.clear, Color.black, \h, 2048, 0.5);
verticalDivider = StaticText.new(controlPanel, Rect.new(620, 5 + (25 * 1), 5, 425));
verticalDivider.background = HiliteGradient.new(Color.clear, Color.black, \v, 1024, 1);

joystickControlSetArray = Array.fill(8, { ControlSet.new; });
tiltControlSet = ControlSet.new;
zoomSliderControlSet = ControlSet.new; // initially empty

labelsSpec = [
	[Rect.new(5, 5 + (25 * 0), 130, 20), "modulation source"],
	[Rect.new(190, 5 + (25 * 0), 20, 20), "x/y"],
	[Rect.new(280, 5 + (25 * 0), 200, 20), "modulation amount"],
	[Rect.new(550, 5 + (25 * 0), 145, 20), "current"],
	[Rect.new(765, 5 + (25 * 0), 200, 20), "parameters"],
	[Rect.new(1035, 5 + (25 * 0), 20, 20), "x/y"],
	[Rect.new(1110, 5 + (25 * 0), 130, 20), "presets"]
];

labels = labelsSpec.collect({ arg each;
	StaticText.new(controlPanel, each[0])
	.string_(each[1])
	.font_(Font.new("Helvetica", 12))  					.align_(\center)
	.stringColor_(Color.black.alpha_(0.7));
});

modulationSlidersSpec = ["streams",
					  "alignment",
					  "asynchronicity",
					  "burst on",
					  "burst off",
					  "intermittency",
					  "soundfile",
					  "scan phase",
					  "scan speed",
					  "formant",
					  "granularity",
					  "envelope shape",
					  "envelope skew",
					  "filter",
					  "resonance",
					  "space",
					  "amplitude"];

modulationSliders = Array.fill(modulationSlidersSpec.size, {
	ModulationSlider.new(controlPanel,
					  joystickControlSetArray,
					  tiltControlSet);
});

modulationSliders.do({ arg each, i;
	each.setZoomSpec(modulationSlidersSpec[i], 0, 100, 0, 100, \lin, 0.01, 0, "% ");
	each.modSrcPopUpMenu.value_(0);
	each.draw(5, 5 + (25 * (i + 1)), 20, 5, 200, 60, 20, 80);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

parameterSlidersSpec = [
	["streams", 1, 24, 1, 12, \lin, 1, 1, "# "],
	["alignment", 0, 180, 0, 180, \lin, 0.1, 180, 161.asAscii.asString ++ " "],
	["asynchronicity", 0, 100, 0, 100, \lin, 0.1, 0, "% "],
	["burst on", 1, 100, 1, 100, \lin, 1, 1, "# "],
	["burst off", 0, 100, 0, 100, \lin, 1, 0, "# "],
	["intermittency", 0, 100, 0, 100, \lin, 0.1, 100, "% "],
	["soundfile", 1, c, 1, c, \lin, 1, 1, "# "],
	["scan phase", 0, 100, 0, 100, \lin, 0.1, 0, "% "],
	["scan speed", -1, 1, -1, 1, \lin, 0.001, 0, "Hz "],
	["formant", -24, 48, -24, 48, \lin, 0.01, 0, "ht "],
	["granularity", 1, 3000, 1, 1000, \exp, 1, 50, "ms "],
	["envelope shape", 0, 100, 0, 100, \lin, 0.1, 50, "% "],
	["envelope skew", 0, 100, 0, 100, \lin, 0.1, 50, "% "],
	["filter", 60, 15000, 60, 15000, \exp, 1, 2000, "Hz "],
	["resonance", 1, 1000, 1, 1000, \exp, 1, 1, "Q "],
	["space", -180, 180, -180, 180, \lin, 0.1, 0, 161.asAscii.asString ++ " "],
	["amplitude", -96, 48, -96, 24, \sin, 1, 0, "dB "]
];

parameterSliders = Array.fill(parameterSlidersSpec.size, {
	ParameterSlider.new(controlPanel,
	                    joystickControlSetArray,
	                    tiltControlSet);
});

parameterSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(
			parameterSlidersSpec[i][j], parameterSlidersSpec[i][j+1], parameterSlidersSpec[i][j+2],
			parameterSlidersSpec[i][j+3], parameterSlidersSpec[i][j+4], parameterSlidersSpec[i][j+5],
			parameterSlidersSpec[i][j+6], parameterSlidersSpec[i][j+7], parameterSlidersSpec[i][j+8]);
	each.draw(635, 5 + (25 * (i + 1)), 20, 5, 200, 60, 20);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

controlPanel.front;

joystickPanels = Array.fill(4, { arg i;
	Window.new("< Joystick " ++ (i+1) ++ " >",
			    Rect.new(0 + (175 * i),
			    Window.screenBounds.height, 175, 150),
			    true, true);
});

joysticksSpec = [
	[Color.green.alpha_(0.3), joystickControlSetArray[0], joystickControlSetArray[1]],
	[Color.blue.alpha_(0.3), joystickControlSetArray[2], joystickControlSetArray[3]],
	[Color.new255(160, 32, 240), joystickControlSetArray[4], joystickControlSetArray[5]],
	[Color.gray.alpha_(0.3);, joystickControlSetArray[6], joystickControlSetArray[7]]
];

joysticks = Array.fill(joysticksSpec.size, { arg i;
	Joystick.new(joystickPanels[i],
			    "",
			    joysticksSpec[i][0],
			    joysticksSpec[i][1],
			    joysticksSpec[i][2],
			    false);
});

joysticks.do({ arg each; each.draw(5, 5, 20, 5, \bottom, \right, \top, true); });

joystickPanels.do({ arg each; each.front; });

outputPanel = Window.new("< Output >",
					 Rect.new(0, 515, 455, (25 * 1) + 5), false, true);
//outputPanel.view.background = HiliteGradient.new(
//	Color.new255(69, 166, 247),
//	Color.new255(0, 82, 155), \h, 2048, 0.5);

outputSlidersSpec = [
	["master gain", -120, 0, -120, 0, \db, 1, 0, "dB "]
];

outputSliders = Array.fill(outputSlidersSpec.size, {
	MappableSlider.new(outputPanel,
	                   joystickControlSetArray,
	                   tiltControlSet);
});

outputSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(
			outputSlidersSpec[i][j], outputSlidersSpec[i][j+1], outputSlidersSpec[i][j+2],
			outputSlidersSpec[i][j+3], outputSlidersSpec[i][j+4], outputSlidersSpec[i][j+5],
			outputSlidersSpec[i][j+6], outputSlidersSpec[i][j+7], outputSlidersSpec[i][j+8]);
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, \right, \left);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

outputPanel.front;

fxPanel = Window.new("< Effects >",
					 Rect.new(790, 515, 455, (25 * 2) + 5), false, true);
//fxPanel.view.background = HiliteGradient.new(
//	Color.new255(69, 166, 247),
//	Color.new255(0, 82, 155), \h, 2048, 0.5);

fxSlidersSpec = [
	["reverb time", 0, 3000, 0, 3000, \lin, 1, 50, "ms "],
	["reverb amount", 0, 100, 0, 100, \lin, 0.1, 0, "% "]
];

fxSliders = Array.fill(fxSlidersSpec.size, {
	MappableSlider.new(fxPanel,
	                   joystickControlSetArray,
	                   tiltControlSet);
});

fxSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(
			fxSlidersSpec[i][j], fxSlidersSpec[i][j+1], fxSlidersSpec[i][j+2],
			fxSlidersSpec[i][j+3], fxSlidersSpec[i][j+4], fxSlidersSpec[i][j+5],
			fxSlidersSpec[i][j+6], fxSlidersSpec[i][j+7], fxSlidersSpec[i][j+8]);
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, \right, \left);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

fxPanel.front;

modPanel = Window.new("< Modulation >",
					 Rect.new(790, Window.screenBounds.height, 455, (25 * 6) + 5), false, true);
//modPanel.view.background = HiliteGradient.new(
//	Color.new255(69, 166, 247),
//	Color.new255(0, 82, 155), \h, 2048, 0.5);

modSlidersSpec = [
	["sine frequency", 0.001, 1000, 0.001, 1, \exp, 0.001, 1, "Hz "],
	["sine phase", 0, 360, 0, 360, \lin, 0.1, 0, 161.asAscii.asString ++ " "],
	["square frequency", 0.001, 1000, 0.001, 1, \exp, 0.001, 1, "Hz "],
	["square width", 0, 100, 0, 100, \lin, 0.1, 50, "% "],
	["saw frequency", 0.001, 1000, 0.001, 1, \exp, 0.001, 1, "Hz "],
	["saw width", 0, 100, 0, 100, \lin, 0.1, 50, "% "],
];

modSliders = Array.fill(modSlidersSpec.size, {
	MappableSlider.new(modPanel,
	                   joystickControlSetArray,
	                   tiltControlSet);
});

modSliders.do({ arg each, i;
	var j = 0;
	each.setZoomSpec(
			modSlidersSpec[i][j], modSlidersSpec[i][j+1], modSlidersSpec[i][j+2],
			modSlidersSpec[i][j+3], modSlidersSpec[i][j+4], modSlidersSpec[i][j+5],
			modSlidersSpec[i][j+6], modSlidersSpec[i][j+7], modSlidersSpec[i][j+8]);
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, \right, \left);
	each.leftBox.background = Color.new255(0, 82, 155).alpha_(1);
	each.rightBox.background = Color.new255(0, 82, 155).alpha_(1);
	zoomSliderControlSet.add(each);
});

modPanel.front;

modSynthDef = Array.fill(17, { arg i; SynthDef("modulator_" ++ i, {
	arg	outBus = 2,
		selindex = 0,
		sineFreq = 1, sinePhase = 0, squareFreq = 1, squareWidth = 50, sawFreq = 1, sawWidth = 50;
	var	noise, sineA, sineB, square, saw;

	noise = WhiteNoise.kr;
	sineA = SinOsc.kr(sineFreq);
	sineB = SinOsc.kr(sineFreq, (sinePhase / 360) * 2pi);
	square = LFPulse.kr(squareFreq, 0, squareWidth * 0.01);
	saw = VarSaw.kr(sawFreq, 0, sawWidth * 0.01);

	Out.kr(outBus, Select.kr(selindex, [noise, sineA, sineB, square, saw]));
}).load(s); });

modSynth = Array.fill(17, { arg i; Synth.new("modulator_" ++ i,
		[\outBus, 2,
		\selindex, 0,
		\sineFreq, 1, \sinePhase, 0, \squareFreq, 1, \squareWidth, 50, \sawFreq, 1, \sawWidth, 50],
		nil, \addToHead); });

modulationSliders.do({ arg each, i;
	modSynth.at(i).set(\outBus, i + 2);
	each.modSrcPopUpMenu.action = { arg menu; modSynth.at(i).set(\selindex, menu.value); i.postln; menu.value.postln; };
});

cloudSynthDef = SynthDef.new("ECbeta", {
	arg 	inBus = 2, outBus = 0,
		nrstreams = 1, alignment = 180, asynch = 0, burston = 1, burstoff = 0,
		interm = 100, soundfile = 1, scanphase = 0, scanspeed = 0,
		pitch = 0, dur = 50, envshape = 50, envskew = 50,
		filter = 2000, res = 1, space = 50, amp = 0,
		nrstreamsModAmt=0, alignmentModAmt=0, asynchModAmt=0, burstonModAmt=0, burstoffModAmt=0,
		intermModAmt=0, soundfileModAmt=0, scanphaseModAmt=0, scanspeedModAmt=0,
		pitchModAmt=0, durModAmt=0, envshapeModAmt=0, envskewModAmt=0,
		filterModAmt=0, resModAmt=0, spaceModAmt=0, ampModAmt=0,
		masterGain=0;

	var	modulator;
	var	nrstreamsOut, alignmentOut, asynchOut, burstonOut, burstoffOut,
		intermOut, soundfileOut, scanphaseOut, scanspeedOut,
		pitchOut, durOut, envshapeOut, envskewOut,
		filterOut, resOut, spaceOut, ampOut;
	var	output;

	modulator = Array.fill(17, { arg i; In.kr(inBus + i); });

	nrstreamsOut = (nrstreams * (2 ** (modulator.at(0) * (nrstreamsModAmt * 0.01)))).max(1).min(12);
	alignmentOut = alignment/180;
	asynchOut = ((asynch * 0.01) * (2 ** (modulator.at(2) * (asynchModAmt * 0.01)))).max(0).min(1);
	burstonOut = burston;
	burstoffOut = burstoff;
	intermOut = ((interm * 0.01) * (2 ** (modulator.at(5) * (intermModAmt * 0.01)))).max(0).min(1);
	soundfileOut = Fold.kr(((soundfile + ((((modulator.at(6) * 0.5 + 0.5) * (c-1)) + 1) * (soundfileModAmt * 0.01)))).round(1), 1, c);
	scanphaseOut = Fold.kr((scanphase * 0.01) + ((modulator.at(7) * 0.5 + 0.5) * (scanphaseModAmt * 0.01)), 0, 1);
	scanspeedOut = (scanspeed * (2 ** (modulator.at(8) * (scanspeedModAmt * 0.01)))).max(-1).min(1);
	pitchOut = (pitch.midiratio * (2 ** (modulator.at(9) * (pitchModAmt * 0.1)))).max(0.25).min(16);
	durOut = ((dur * 0.001) * (2 ** (modulator.at(10) * (durModAmt * 0.1)))).max(0.001).min(1);
	envshapeOut = Fold.kr((envshape * 0.01) + ((modulator.at(11) * 0.5 + 0.5) * (envshapeModAmt * 0.01)), 0.001, 0.999);
	envskewOut = Fold.kr((envskew * 0.01 * 2) + ((modulator.at(12) * 0.5 + 0.5) * (envskewModAmt * 0.01)), 0.001, 0.999);
	filterOut = (filter * (2 ** (modulator.at(13) * (filterModAmt * 0.1)))).max(60).min(15000);
	resOut = ((res.reciprocal) * (2 ** (modulator.at(14) * (resModAmt * 0.1)))).max(0.001).min(1);
	spaceOut = ((space / 180) * 0.999).neg + (modulator.at(15) * (spaceModAmt * 0.01));
	ampOut = (amp.dbamp * (2 ** (modulator.at(16) * (ampModAmt * 0.1)))).min(250);

	output = RCloud.ar(nrstreamsOut, alignmentOut, asynchOut,
			intermOut, soundfileOut, scanphaseOut, scanspeedOut,
			pitchOut, durOut, envshapeOut, envskewOut,
			filterOut, resOut, spaceOut, ampOut);
	output = Out.ar(outBus, output * masterGain.dbamp);
}).load(s);

cloudSynth = Synth.new("ECbeta",
		[\inBus, 2, \outBus, 0,
		 \nrstreams, 1, \alignment, 180, \asynch, 0, \burston, 1, \burstoff, 0,
		 \interm, 100, \soundfile, 1, \scanphase, 0, \scanspeed, 0,
		 \pitch, 0, \dur, 50, \envshape, 50, \envskew, 50,
		 \filter, 2000, \res, 1, \space, 50, \amp, 0,
		 \nrstreamsModAmt, 0, \alignmentModAmt, 0, \burstonModAmt, 0, \burstoffModAmt, 0,
		 \intermModAmt, 0, \soundfileModAmt, 0, \scanphaseModAmt, 0, \scanspeedModAmt, 0,
		 \pitchModAmt, 0, \durModAmt, 0, \envshapeAmt, 0, \envskewAmt, 0,
		 \filterModAmt, 0, \resModAmt, 0, \spaceModAmt, 0, \ampModAmt, 0,
		 \masterGain, 0],
		 nil, \addToTail);

// a little reverb to spice things up...
// mono in, mono out - aux bus send/return
reverbSynthDef = SynthDef("reverb", {
//	arg	inBus = 0, outBus = 0;
	arg	inBus = 0, outBus = 0,
		reverbTime = 50, reverbAmt = 0;
	var	in, delay, combs, allpasses, out;

	// input gets full precision (since this is a send)
	in = In.ar(inBus, 1);

	// initial delay
	delay = DelayN.ar(in, 0.048);

	// 6 modulated comb delays in parallel
	combs = Mix.ar(CombL.ar(delay, 0.1, LFNoise1.kr(Array.fill(6, {0.1.rand}), 0.01, 0.05), reverbTime * 0.001));

	// chain of 4 allpass delays on each of two channels (8 total)
	allpasses = combs;
	4.do({ allpasses = AllpassN.ar(allpasses, 0.050, [0.050.rand,0.050.rand], 1); });

	// scale the output
	out = (reverbAmt * 0.01) * allpasses;
	out = Out.ar(outBus, out);
}).load(s);

reverbSynth = Synth.new("reverb",
		[\inBus, [0, 1], \outBus, [0, 1],
		\reverbTime, 50, \reverbAmt, 0],
		nil, \addToTail);
//s.sendMsg("\s_new", "reverb", -1, 1, 0, \inBus, [0,1], \outBus, [0,1], \reverbTime, 50, \reverbAmt, 0);

17.do({ arg i;
modSliders[0].plugIntoSynth(modSynth.at(i), \sineFreq);
modSliders[1].plugIntoSynth(modSynth.at(i), \sinePhase);
modSliders[2].plugIntoSynth(modSynth.at(i), \squareFreq);
modSliders[3].plugIntoSynth(modSynth.at(i), \squareWidth);
modSliders[4].plugIntoSynth(modSynth.at(i), \sawFreq);
modSliders[5].plugIntoSynth(modSynth.at(i), \sawWidth);
});

modulationSliders[0].plugIntoSynth(cloudSynth, \nrstreamsModAmt);
modulationSliders[1].plugIntoSynth(cloudSynth, \alignmentModAmt);
modulationSliders[2].plugIntoSynth(cloudSynth, \asynchModAmt);
modulationSliders[3].plugIntoSynth(cloudSynth, \burstonModAmt);
modulationSliders[4].plugIntoSynth(cloudSynth, \burstoffModAmt);
modulationSliders[5].plugIntoSynth(cloudSynth, \intermModAmt);
modulationSliders[6].plugIntoSynth(cloudSynth, \soundfileModAmt);
modulationSliders[7].plugIntoSynth(cloudSynth, \scanphaseModAmt);
modulationSliders[8].plugIntoSynth(cloudSynth, \scanspeedModAmt);
modulationSliders[9].plugIntoSynth(cloudSynth, \pitchModAmt);
modulationSliders[10].plugIntoSynth(cloudSynth, \durModAmt);
modulationSliders[11].plugIntoSynth(cloudSynth, \envshapeModAmt);
modulationSliders[12].plugIntoSynth(cloudSynth, \envskewModAmt);
modulationSliders[13].plugIntoSynth(cloudSynth, \filterModAmt);
modulationSliders[14].plugIntoSynth(cloudSynth, \resModAmt);
modulationSliders[15].plugIntoSynth(cloudSynth, \spaceModAmt);
modulationSliders[16].plugIntoSynth(cloudSynth, \ampModAmt);

parameterSliders[0].plugIntoSynth(cloudSynth, \nrstreams);
parameterSliders[1].plugIntoSynth(cloudSynth, \alignment);
parameterSliders[2].plugIntoSynth(cloudSynth, \asynch);
parameterSliders[3].plugIntoSynth(cloudSynth, \burston);
parameterSliders[4].plugIntoSynth(cloudSynth, \burstoff);
parameterSliders[5].plugIntoSynth(cloudSynth, \interm);
parameterSliders[6].plugIntoSynth(cloudSynth, \soundfile);
parameterSliders[7].plugIntoSynth(cloudSynth, \scanphase);
parameterSliders[8].plugIntoSynth(cloudSynth, \scanspeed);
parameterSliders[9].plugIntoSynth(cloudSynth, \pitch);
parameterSliders[10].plugIntoSynth(cloudSynth, \dur);
parameterSliders[11].plugIntoSynth(cloudSynth, \envshape);
parameterSliders[12].plugIntoSynth(cloudSynth, \envskew);
parameterSliders[13].plugIntoSynth(cloudSynth, \filter);
parameterSliders[14].plugIntoSynth(cloudSynth, \res);
parameterSliders[15].plugIntoSynth(cloudSynth, \space);
parameterSliders[16].plugIntoSynth(cloudSynth, \amp);

outputSliders[0].plugIntoSynth(cloudSynth, \masterGain);

fxSliders[0].plugIntoSynth(reverbSynth, \reverbTime);
fxSliders[1].plugIntoSynth(reverbSynth, \reverbAmt);

controlPanel.onClose = {
	cloudSynth.free(true);
	joystickPanels.do({ arg each; each.close; });
	outputPanel.close;
	fxPanel.close;
	modPanel.close;
};

)