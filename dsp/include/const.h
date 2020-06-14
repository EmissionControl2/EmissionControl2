#ifndef CONSTS_H
#define CONSTS_H

#include <string>

namespace consts {

const int SAMPLE_RATE = 48000;
const int BLOCK_SIZE = 1024;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, SAW, NOISE };
enum bound { MIN, MAX };

const std::string OSX_CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-osx.sh";
const std::string OSX_DEFAULT_SAMPLE_PATH =
    "/Library/Application Support/EmissionControl20/samples/";
const std::string OSX_DEFAULT_SOUND_OUTPUT_PATH =
    "/Library/Application Support/EmissionControl20/soundOutput/";
const std::string OSX_DEFAULT_PRESETS_PATH =
    "/Library/Application Support/EmissionControl20/presets/";
const std::string OSX_DEFAULT_CONFIG_PATH =
    "/Library/Application Support/EmissionControl20/configs/";
const std::string OSX_DEFAULT_CONFIG_FILE =
    OSX_DEFAULT_CONFIG_PATH + "config.json";
}
#endif