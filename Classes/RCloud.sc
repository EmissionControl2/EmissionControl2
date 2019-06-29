// RCloudUGens by David Thall
 RCloud : MultiOutUGen {
	*ar { arg nrstreams = 1, alignment = 1, asynch = 0, interm = 1, 
			bufnum = 0, pphase = 0, pspeed = 1, rate = 1, 
			dur = 0.1, env = 0.5, skew = 1, 
			freq = 2000, rq = 0.707, pan = 0, amp = 1; 
		^this.multiNew('audio', nrstreams, alignment, asynch, interm, bufnum, 
				pphase, pspeed, rate, dur, env, skew, freq, rq, pan, amp)
	}
	
	/*
	// return an OutputProxy for 'numChannels' of outputs...
	init { arg argNumChannels ... theInputs;
		inputs = theInputs;
		^this.initOutputs(argNumChannels, rate);
	}
	
	argNamesInputsOffset { ^1 }
	*/
	
	// init copied from Pan2 (defaults to 2 output channels)
	init { arg ... theInputs;
		inputs = theInputs;		
		channels = [ 
			OutputProxy(rate, this, 0), 
			OutputProxy(rate, this, 1) 
		];
		^channels
	}
}

/*
(
s = Server.local;
Server.default = s;
s.boot;
)
s.options.blockSize = 64;
(
s.sendMsg(\b_allocRead, 1, "sounds/beats_1sec.aiff");
)

(
{
	//RCloud.ar(1, 1, LFTri.kr(0.05,-1,0.5,0.5), 1, 6, MouseX.kr(0,1,0,0), 1/8 * 0.01, MouseY.kr(4,0.5,1,0), SinOsc.kr(0.1,0,0.049,0.05), 0.5, 1, 2000, 0.707, SinOsc.kr(0.25), 2);
	RCloud.ar(1, 1, 0, 1, 1, 0, 0, MouseY.kr(4,1,1,0), MouseX.kr(0.1,0.01,1,0), 0.5, 1, 2000, 0.707, SinOsc.kr(0.25), 2);
	//RCloud.ar(1, 1, 0, 1, 6, 0, 0.001, 1, 0.01, MouseY.kr(1,0,0,0), MouseX.kr(0,2,0,0), 2000, 0.707, 0, 2);
	//RCloud.ar(MouseX.kr(1,32,0,0), 1, 0, MouseY.kr(1,0,0,0), 6, 0.3, 0, 1, 0.01, 0.5, 1, 2000, 0.707, SinOsc.kr(0.25), 2);
}.play;
) 
*/
