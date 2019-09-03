#ifndef CONST_H 
#define CONST_H

const int SAMPLE_RATE = 44100;
const int BLOCK_SIZE = 128;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
enum streamType {synchronous, asynchronous, sequenced, derived};
enum waveform {SINE, SQUARE, SAW, NOISE};


#endif