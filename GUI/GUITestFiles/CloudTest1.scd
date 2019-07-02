
/*
 * David Thall
 * CloudTest1.rtf (20040728)
 * CG2 Project
 */

/* Note: This uses ParticleUGens_20040501_e3.cpp, last compiled May 11, 2004 */

(
var	window, zoomSlidersArraySpec, zoomSliders;
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
		soundfile = 1, density = 10, duration = 100, pitch = 0, scanRate = 0.25, // specified in gui param units (converted within)
		phase = 0, space = 0, filter = 2000, resonance = 1, amplitude = 0;
	var	numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp;
	numChannels = 2; // stereo
	trigger = Impulse.ar(density);
	bufnum = soundfile;
	rate = pitch.midiratio;
	//centerPos = LFSaw.kr(sfDur.reciprocal, -1, 0.5, 0.5);
	//centerPos = LFSaw.kr((sfDur * (time * 0.01)).reciprocal, -1, 0.5, 0.5) * BufDur.kr(bufnum); // scan time & phase don't map correctly!
	centerPos = LFSaw.kr(scanRate, -1, 0.5, 0.5) * BufDur.kr(bufnum); // scanRate: works!!!
	dur = duration * 0.001;
	pan = ((space / 180) * 0.999).neg; // quick fix for pan problems: [-1 -> 1] is reversed [r -> l], & extreme edge wraps to other side
	//amp = 1.0;
	interp = 2;
	freq = filter;
	rq = resonance.reciprocal;
	gainComp = 1;
	//amp = 1.0;
	//amp = 1.0 * rq.reciprocal.min(10); // old compensation
	//amp = 1.0 + (rq.reciprocal * 0.1); // see SC1 SweepingGranQ p.2
	amp = (amplitude.dbamp) * rq.reciprocal ** 0.7; // see SC2 CVox_ConstQ
	Out.ar(outBusNum, RCloud.ar(numChannels, trigger, bufnum, rate, centerPos, dur, pan, amp, interp, freq, rq, gainComp));
}).load(s);

//s.sendMsg("\s_new", "CloudTest1", -1, 1, 0, \outBus, 0);
synth = Synth.new("CloudTest1", [\outBus, 0, \soundfile, 1, \density, 10, \duration, 100, \pitch, 0, \scanRate, 0.25, \phase, 0, \space, 0, \filter, 2000, \resonance, 1, \amplitude, 0], nil, \addToHead);
//synth.nodeID = 1000;

window = Window.new("< CloudTest1 >", Rect.new(0, 1024, 465, 255), false, true);

zoomSlidersArraySpec = [
	["soundfile", 1, 128, 1, 128, \lin, 1, 1, "# "],
	["density", 1, 1000, 1, 1000, \exp, 1, 10, "/s "],
	["duration", 1, 10000, 1, 10000, \exp, 1, 100, "ms "],
	["pitch", -48, 24, -48, 24, \sin, 0.01, 0, "ht "],
	["scan rate", 0.001, 10, 0.001, 10, \exp, 0.001, 0.25, "Hz "],
	["phase", 0, 100, 0, 100, \lin, 0.01, 0, "% "],
	["space", -180, 180, -180, 180, \lin, 0.1, 0, 161.asAscii.asString ++ " "],
	["filter", 60, 18000, 60, 18000, \exp, 1, 2000, "Hz "],
	["resonance", 0.5, 1000, 0.5, 1000, \exp, 0.1, 1, "Q "],
	["amplitude", 1e-06.ampdb, 1.ampdb, 1e-06.ampdb, 1.ampdb, \sin, 0.1, 1.ampdb, "dB "]
];

zoomSliders = zoomSlidersArraySpec.collect({ arg each;
	ZoomSlider.new(window, each[0], each[1], each[2], each[3], each[4], each[5], each[6], each[7], each[8]);
});

zoomSliders.do({ arg each, i;
	each.draw(5, 5 + (25 * i), \horizontal, 20, 5, 260, 60, \right);
});

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

zoomSliders[0].plugIntoSynth(synth, \soundfile);
zoomSliders[1].plugIntoSynth(synth, \density);
zoomSliders[2].plugIntoSynth(synth, \duration);
zoomSliders[3].plugIntoSynth(synth, \pitch);
zoomSliders[4].plugIntoSynth(synth, \scanRate);
zoomSliders[5].plugIntoSynth(synth, \phase);
zoomSliders[6].plugIntoSynth(synth, \space);
zoomSliders[7].plugIntoSynth(synth, \filter);
zoomSliders[8].plugIntoSynth(synth, \resonance);
zoomSliders[9].plugIntoSynth(synth, \amplitude);

window.front;

window.onClose = {
	//s.sendBundle(nil, ["/n_free", 1000]);
	synth.free(true);
};
)