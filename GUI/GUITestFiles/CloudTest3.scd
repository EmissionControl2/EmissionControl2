/*
 * David Thall
 * CloudTest1.rtf (20040728)
 * CG2 Project
 */

/* Note: This uses ParticleUGens_20040501_e3.cpp, last compiled May 11, 2004 */

// Server.internal.latency; // get/set latency

(
var	window, modulationSlidersArraySpec, modulationSliders, parameterSlidersArraySpec, parameterSliders;
var	lfoWindow, lfoSlidersArraySpec, lfoSliders;
var	synthDef, synth;
var	path, buffer, sfInfo, sfDur;

s = Server.internal;
Server.default = s;
s.boot;

path = "sounds/a11wlk01.wav";
buffer = Buffer.read(s, path);
sfInfo = SoundFile.new;
sfInfo.openRead(path);
sfDur = sfInfo.duration;

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

synthDef = SynthDef.new("CloudTest1", {
	arg	outBusNum = 0,
		soundfile = 1, density = 10, duration = 100, pitch = 0, time = 100, // specified in gui param units (converted within)
		phase = 0, space = 0, filter = 2000, resonance = 1, amplitude = 0,
		soundfileModAmt = 0, densityModAmt = 0, durationModAmt = 0, pitchModAmt = 0, timeModAmt = 0,
		phaseModAmt = 0, spaceModAmt = 0, filterModAmt = 0, resonanceModAmt = 0, amplitudeModAmt = 0,
		modFreq = 1, modWidth = 0;
	var	modSrc;
	var	numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp;
	//modSrc = WhiteNoise.ar; // random
	modSrc = VarSaw.ar(modFreq, 0, (modWidth * 0.01), 2, -1);
	numChannels = 2; // stereo
	trigger = Impulse.ar(density * (2 ** (modSrc * (densityModAmt * 0.1))));
	bufnum = soundfile * (2 ** (modSrc * (soundfileModAmt * 0.1))); // this must be rounded to an integer!!! (somehow it still seems to work...however, it must be scaled properly!)
	rate = (pitch.midiratio) * (2 ** (modSrc * (pitchModAmt * 0.1)));
	centerPos = LFSaw.kr(((BufDur.kr(bufnum).reciprocal) * (time * 0.01)) * (2 ** (modSrc * (timeModAmt * 0.1))), -1, 0.5, 0.5) * BufDur.kr(bufnum); // time: works!!!
	//centerPos = LFSaw.kr((BufDur.kr(bufnum).reciprocal) * (time * 0.01), -1, 0.5, 0.5) * BufDur.kr(bufnum); // time: works!!!
	//centerPos = LFSaw.kr(scanRate * (2 ** (modSrc * (scanRateModAmt * 0.1))), -1, 0.5, 0.5) * BufDur.kr(bufnum); // scanRate: works!!!
	dur = (duration * 0.001) * (2 ** (modSrc * (durationModAmt * 0.1)));
	pan = ((space / 180) * 0.999).neg + (modSrc * (spaceModAmt * 0.01)); // quick fix for pan problems: [-1 -> 1] is reversed [r -> l], & extreme edge wraps to other side
	//amp = 1.0;
	interp = 1;
	freq = (filter * (2 ** (modSrc * (filterModAmt * 0.1)))).max(20).min(18000);
	rq = ((resonance.reciprocal) * (2 ** (modSrc * (resonanceModAmt * 0.1)))).max(0.001).min(2);
	gainComp = 1;
	//amp = 1.0;
	//amp = 1.0 * rq.reciprocal.min(10); // old compensation
	//amp = 1.0 + (rq.reciprocal * 0.1); // see SC1 SweepingGranQ p.2
	amp = ((amplitude.dbamp) * (2 ** (modSrc * (amplitudeModAmt * 0.1)))).min(4.0) * rq.reciprocal ** 0.7; // see SC2 CVox_ConstQ
	Out.ar(outBusNum, Cloud.ar(numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp));
}).load(s);

//s.sendMsg("\s_new", "CloudTest1", -1, 1, 0, \outBus, 0);
synth = Synth.new("CloudTest1", [\outBus, 0, \soundfile, 1, \density, 10, \duration, 100, \pitch, 0, \time, 100, \phase, 0, \space, 0, \filter, 2000, \resonance, 1, \amplitude, 0], nil, \addToHead);
//synth.nodeID = 1000;

window = SCWindow.new("< CloudTest1 >", Rect.new(0, 1024, 1245, 255), false, true);

modulationSlidersArraySpec = [
	["soundfile", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["density", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["duration", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["pitch", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["time", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["phase", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["space", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["filter", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["resonance", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
	["amplitude", 0, 100, 0, 100, \exp, 0.01, 0, "% "],
];

modulationSliders = modulationSlidersArraySpec.collect({ arg each;
	ModulationSlider.new(window, each[0], each[1], each[2], each[3], each[4], each[5], each[6], each[7], each[8]);
});

modulationSliders.do({ arg each, i;
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, 80);
});

parameterSlidersArraySpec = [
	["soundfile", 1, 128, 1, 128, \lin, 1, 1, "# "],
	["density", 1, 1000, 1, 1000, \exp, 1, 10, "/s "],
	["duration", 1, 10000, 1, 10000, \exp, 1, 100, "ms "],
	["pitch", -48, 24, -48, 24, \sin, 0.01, 0, "ht "],
	["time", 0.1, 400, 0.1, 400, \exp, 0.1, 100, "% "],
	["phase", 0, 100, 0, 100, \lin, 0.01, 0, "% "],
	["space", -180, 180, -180, 180, \lin, 0.1, 0, 161.asAscii.asString ++ " "],
	["filter", 60, 18000, 60, 18000, \exp, 1, 2000, "Hz "],
	["resonance", 0.5, 1000, 0.5, 1000, \exp, 0.1, 1, "Q "],
	["amplitude", -120, 12, -120, 12, \sin, 0.1, 0, "dB "]
];

parameterSliders = parameterSlidersArraySpec.collect({ arg each;
	ParameterSlider.new(window, each[0], each[1], each[2], each[3], each[4], each[5], each[6], each[7], each[8]);
});

parameterSliders.do({ arg each, i;
	each.draw(625, 5 + (25 * i), 20, 5, 200, 60, 20);
});

lfoWindow = SCWindow.new("< Saw/Tri LFO >", Rect.new(0, 475, 455, 55), false, true);
lfoSlidersArraySpec = [
	["modFreq", 0.01, 25, 0.01, 25, \exp, 0.01, 1, "Hz "],
	["modWidth", 0, 100, 0, 100, \lin, 0.1, 0, "% "]
];
lfoSliders = lfoSlidersArraySpec.collect({ arg each;
	MappableSlider.new(lfoWindow, each[0], each[1], each[2], each[3], each[4], each[5], each[6], each[7], each[8]);
});
lfoSliders.do({ arg each, i;
	each.draw(5, 5 + (25 * i), 20, 5, 200, 60, 20, \right, \left);
});
lfoWindow.front;

("sfDur = " ++ sfDur).postln;
//("synth.nodeID -> " ++ synth.nodeID).postln;

/*
zoomSliders[0].currentBoxActionList.add({ arg currentBox;
	//s.sendMsg("\n_set", -1, \soundfile, currentBox.value);
	synth.set(\soundfile, currentBox.value);
	//("currentBox.value -> " ++ currentBox.value).postln;
});
zoomSliders[1].currentBoxActionList.add({ arg currentBox;
	//s.sendMsg("\n_set", -1, \density, currentBox.value);
	synth.set(\density, currentBox.value);
	//("currentBox.value -> " ++ currentBox.value).postln;
});
zoomSliders[2].currentBoxActionList.add({ arg currentBox;
	//s.sendMsg("\n_set", -1, \duration, currentBox.value);
	synth.set(\duration, currentBox.value);
	//("currentBox.value -> " ++ currentBox.value).postln;
});
zoomSliders[3].currentBoxActionList.add({ arg currentBox;
	//s.sendMsg("\n_set", -1, \pitch, currentBox.value);
	synth.set(\pitch, currentBox.value);
	//("currentBox.value -> " ++ currentBox.value).postln;
});
zoomSliders[4].currentBoxActionList.add({ arg currentBox;
	//s.sendMsg("\n_set", -1, \time, currentBox.value);
	synth.set(\time, currentBox.value);
	("currentBox.value -> " ++ currentBox.value).postln;
});
*/

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

window.front;

window.onClose = {
	//s.sendBundle(nil, ["/n_free", 1000]);
	synth.free(true); // this doesn't always seem to call the garbage collector, or the nodeID isn't getting removed...
};
)