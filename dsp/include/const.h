#ifndef CONSTS_H
#define CONSTS_H

namespace consts {

const int SAMPLE_RATE = 48000;
const int BLOCK_SIZE = 1024;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, SAW, NOISE };
enum bound { MIN, MAX };
}
#endif