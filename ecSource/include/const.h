#ifndef CONSTS_H
#define CONSTS_H

#include <string>

namespace consts {

const std::string VERSION_NUMBER = "1.2";

const char KEYBOARD_MIDI_LEARN = 'm';     // Hover over param - m
const char KEYBOARD_MIDI_UNLEARN = 'm';   // Hover over param - Shift - m
const char KEYBOARD_TOGGLE_ENGINE = ' ';  // Press space bar to start/stop audio engine

const char KEYBOARD_PARAM_LOG_TOGGLE = 'l';  // Hover over param - l

enum sliderType { PARAM = 0, MOD, LFO, INT_PARAM, INT_MOD, INT_LFO };
enum MIDIType { M_PARAM = 0, M_MOD, M_LFO, M_DUTY, M_MORPH };

enum paramNames {
  GRAIN_RATE = 0,
  ASYNC,
  INTERM,
  STREAMS,
  PLAYBACK,
  FILTER_CENTER,
  RESONANCE,
  SOUND_FILE,
  SCAN_BEGIN,
  SCAN_RANGE,
  SCAN_SPEED,
  GRAIN_DUR,
  ENVELOPE,
  PAN,
  AMPLITUDE
};
const int CR_EVERY_N_SAMPLES = 4;
const unsigned MAX_NUM_FLOATS_PER_AUDIO_FILE = 256000000;  // 1 gb worth of floats

const int NUM_PARAMS = 15;
const int NUM_LFOS = 6;
const int MAX_GRAIN_DISPLAY = 100;
const int MAX_MIDI_IN = 5;
const float MAX_MORPH_TIME = 50.0f;

enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, ASCEND, DESCEND, NOISE };
enum polarity { BI, UNI_POS, UNI_NEG, UNI };
enum bound { MIN, MAX };

/**
 *  JSON KEYS
 */

const std::string SOUND_OUTPUT_PATH_KEY = "USER_SOUND_OUTPUT_PATH";
const std::string SAMPLE_RATE_KEY = "SAMPLE_RATE";
const std::string BUFFER_SIZE_KEY = "BUFFER_SIZE";

const std::string LIGHT_MODE_KEY = "LIGHT_MODE";
const std::string FONT_SCALE_KEY = "FONT_SCALE";
const std::string MIDI_PRESET_NAMES_KEY = "MIDI_PRESET_NAMES";
const std::string OSC_PRESET_NAMES_KEY = "OSC_PRESET_NAMES";
const std::string SAMPLE_PRESET_NAMES_KEY = "SAMPLE_PRESET_NAMES";
const std::string WINDOW_WIDTH_KEY = "WINDOW_WIDTH";
const std::string WINDOW_HEIGHT_KEY = "WINDOW_HEIGHT";
const std::string FULLSCREEN_KEY = "FULLSCREEN";
const std::string IS_FIRST_LAUNCH_KEY = "FIRST_LAUNCH";
const std::string DEFAULT_AUDIO_DEVICE_KEY = "DEFAULT_AUDIO_DEVICE";
const std::string LEAD_CHANNEL_KEY = "LEAD_CHANNEL";
const std::string CLIP_AUDIO_KEY = "CLIP_AUDIO";
const std::string OMIT_SOUNDFILE_PARAM_KEY = "OMIT_SOUNDFILE_PARAM";
const std::string HARD_RESET_SCANBEGIN_KEY = "HARD_RESET_SCANBEGIN";

const int SAMPLE_RATE = 48000;
const int BUFFER_SIZE = 1024;
const int MAX_AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
const bool LIGHT_MODE = false;
const float FONT_SCALE = 1.0;
const float WINDOW_WIDTH = 1920;
const float WINDOW_HEIGHT = 1080;
const bool FULLSCREEN = false;
const bool IS_FIRST_LAUNCH = true;
const std::string DEFAULT_AUDIO_DEVICE = "";
const size_t DEFAULT_LEAD_CHANNEL = 0;
const bool DEFAULT_CLIP_AUDIO = true;
const bool DEFAULT_OMIT_SOUNDFILE_PARAM = false;
const bool DEFAULT_HARD_RESET_SCANBEGIN = false;

/**
 *  DEFAULT USER PATHS
 */

#ifdef __APPLE__
const std::string PERSISTENT_DATA_PATH = "/Music/EmissionControl2/";
const std::string DEFAULT_SAMPLE_PATH = PERSISTENT_DATA_PATH + "samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = PERSISTENT_DATA_PATH + "soundOutput/";
const std::string DEFAULT_PRESETS_PATH = PERSISTENT_DATA_PATH + "presets/";
const std::string DEFAULT_MIDI_PRESETS_PATH = PERSISTENT_DATA_PATH + "midi_presets/";
const std::string DEFAULT_OSC_PRESETS_PATH = PERSISTENT_DATA_PATH + "osc_presets/";
const std::string DEFAULT_SAMPLE_PRESETS_PATH = PERSISTENT_DATA_PATH + "sample_presets/";
const std::string DEFAULT_CONFIG_PATH = PERSISTENT_DATA_PATH + "configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";

#endif

#ifdef __linux__
const std::string DEFAULT_SAMPLE_PATH = "/usr/share/emissioncontrol2/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = "";
#endif

#ifdef _WIN32
const std::string PERSISTENT_DATA_PATH = "/Music/EmissionControl2/";
const std::string DEFAULT_SAMPLE_PATH = PERSISTENT_DATA_PATH + "samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH = PERSISTENT_DATA_PATH + "soundOutput/";
const std::string DEFAULT_PRESETS_PATH = PERSISTENT_DATA_PATH + "presets/";
const std::string DEFAULT_MIDI_PRESETS_PATH = PERSISTENT_DATA_PATH + "midi_presets/";
const std::string DEFAULT_OSC_PRESETS_PATH = PERSISTENT_DATA_PATH + "osc_presets/";
const std::string DEFAULT_SAMPLE_PRESETS_PATH = PERSISTENT_DATA_PATH + "sample_presets/";
const std::string DEFAULT_CONFIG_PATH = PERSISTENT_DATA_PATH + "configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif
}  // namespace consts
#endif
