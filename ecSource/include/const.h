#ifndef CONSTS_H
#define CONSTS_H

#include <string>

namespace consts {

const char KEYBOARD_MIDI_LEARN = 'm';   // Hover over param - m
const char KEYBOARD_MIDI_UNLEARN = 'm'; // Hover over param - Shift - m

const char KEYBOARD_PARAM_LOG_TOGGLE = 'l'; // Hover over param - l

enum sliderType { PARAM = 0, MOD, LFO, INT_PARAM, INT_MOD, INT_LFO };
enum MIDIType { M_PARAM = 0, M_MOD, M_LFO, M_DUTY };

enum paramNames {
  GRAIN_RATE = 0,
  ASYNC,
  INTERM,
  STREAMS,
  PLAYBACK,
  FILTER_CENTER,
  RESONANCE,
  SOUND_FILE,
  SCAN_HEAD,
  SCAN_RANGE,
  SCAN_SPEED,
  GRAIN_DUR,
  ENVELOPE,
  PAN,
  AMPLITUDE
};
const int CR_EVERY_N_SAMPLES = 4;

const int NUM_PARAMS = 15;
const int NUM_LFOS = 6;
const int MAX_GRAIN_DISPLAY = 100;
const int MAX_MIDI_IN = 5;

enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, ASCEND, DESCEND, NOISE };
enum polarity { BI, UNI_POS, UNI_NEG, UNI };
enum bound { MIN, MAX };

/**
 *  JSON KEYS
 */

const std::string SOUND_OUTPUT_PATH_KEY = "USER_SOUND_OUTPUT_PATH";
const std::string SAMPLE_RATE_KEY = "SAMPLE_RATE";
const std::string LIGHT_MODE_KEY = "LIGHT_MODE";
const std::string FONT_SCALE_KEY = "FONT_SCALE";
const std::string MIDI_PRESET_NAMES_KEY = "MIDI_PRESET_NAMES";
const std::string WINDOW_WIDTH_KEY = "WINDOW_WIDTH";
const std::string WINDOW_HEIGHT_KEY = "WINDOW_HEIGHT";
const std::string FULLSCREEN_KEY = "FULLSCREEN";
const std::string IS_FIRST_LAUNCH_KEY = "FIRST_LAUNCH";
const std::string DEFAULT_AUDIO_DEVICE_KEY = "DEFAULT_AUDIO_DEVICE";

const int SAMPLE_RATE = 48000;
const int BLOCK_SIZE = 1024;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
const bool LIGHT_MODE = false;
const float FONT_SCALE = 1.0;
const float WINDOW_WIDTH = 1920;
const float WINDOW_HEIGHT = 1080;
const bool FULLSCREEN = false;
const bool IS_FIRST_LAUNCH = true;
const std::string DEFAULT_AUDIO_DEVICE = "";

/**
 *  DEFAULT USER PATHS
 */

#ifdef __APPLE__
const std::string PERSISTENT_DATA_PATH = "/Music/EmissionControl2";
const std::string DEFAULT_SAMPLE_PATH = PERSISTENT_DATA_PATH + "/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = PERSISTENT_DATA_PATH + "/soundOutput/";
const std::string DEFAULT_PRESETS_PATH = PERSISTENT_DATA_PATH + "/presets/";
const std::string DEFAULT_MIDI_PRESETS_PATH = PERSISTENT_DATA_PATH + "/midi_presets/";
const std::string DEFAULT_CONFIG_PATH = PERSISTENT_DATA_PATH + "/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif

#ifdef __linux__
const std::string DEFAULT_SAMPLE_PATH = "/usr/local/share/EmissionControl2/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = "/Music/EmissionControl2";
#endif

#ifdef _WIN32
const std::string PERSISTENT_DATA_PATH = "/EmissionControl2";
const std::string DEFAULT_SAMPLE_PATH = PERSISTENT_DATA_PATH + "/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = PERSISTENT_DATA_PATH + "/soundOutput/";
const std::string DEFAULT_PRESETS_PATH = PERSISTENT_DATA_PATH + "/presets/";
const std::string DEFAULT_MIDI_PRESETS_PATH = PERSISTENT_DATA_PATH + "/midi_presets/";
const std::string DEFAULT_CONFIG_PATH = PERSISTENT_DATA_PATH + "/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif
} // namespace consts
#endif
