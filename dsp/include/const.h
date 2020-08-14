#ifndef CONSTS_H
#define CONSTS_H

#include <string>

namespace consts {


enum paramNames {
  GRAIN_RATE = 0,
  ASYNC,
  INTERM,
  STREAMS,
  GRAIN_DUR,
  ENVELOPE,
  PITCH_SHIFT,
  FILTER_CENTER,
  RESONANCE,
  SOUND_FILE,
  SCAN_POS,
  SCAN_WIDTH,
  SCAN_SPEED,
  PAN,
  VOLUME
};
const int NUM_PARAMS = 15;
const int NUM_LFOS = 4;

const int SAMPLE_RATE = 48000;
const int BLOCK_SIZE = 1024;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
const bool LIGHT_MODE = true;
const float FONT_SCALE = 1.0;
enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, ASCEND, DESCEND, NOISE };
enum polarity { BI, UNI_POS, UNI_NEG, UNI };
enum bound { MIN, MAX };
enum sliderType{ PARAM = 0, MOD, LFO, INT_PARAM, INT_MOD, INT_LFO };

/**
 *  JSON KEYS
 */

const std::string SOUND_OUTPUT_PATH_KEY = "USER_SOUND_OUTPUT_PATH";
const std::string SAMPLE_RATE_KEY = "SAMPLE_RATE";
const std::string LIGHT_MODE_KEY = "LIGHT_MODE";
const std::string FONT_SCALE_KEY = "FONT_SCALE";

/**
 *  DEFAULT USER PATHS
 */

#ifdef __APPLE__
const std::string CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-osx.sh";
const std::string DEFAULT_SAMPLE_PATH =
    "/Library/Application Support/EmissionControl2/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH =
    "/Library/Application Support/EmissionControl2/soundOutput/";
const std::string DEFAULT_PRESETS_PATH =
    "/Library/Application Support/EmissionControl2/presets/";
const std::string DEFAULT_CONFIG_PATH =
    "/Library/Application Support/EmissionControl2/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif

#ifdef __linux__
const std::string DEFAULT_SAMPLE_PATH =
    "/usr/local/share/EmissionControl2/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = "";
#endif

#ifdef _WIN32_
const std::string CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-osx.sh";
const std::string DEFAULT_SAMPLE_PATH =
    "/Library/Application Support/EmissionControl2/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH =
    "/Library/Application Support/EmissionControl2/soundOutput/";
const std::string DEFAULT_PRESETS_PATH =
    "/Library/Application Support/EmissionControl2/presets/";
const std::string DEFAULT_CONFIG_PATH =
    "/Library/Application Support/EmissionControl2/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif
} // namespace consts
#endif
