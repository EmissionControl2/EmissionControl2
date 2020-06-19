#ifndef CONSTS_H
#define CONSTS_H

#include <string>

namespace consts {

const int SAMPLE_RATE = 48000;
const int BLOCK_SIZE = 1024;
const int AUDIO_OUTS = 2;
const int DEVICE_NUM = -1;
enum streamType { synchronous, asynchronous, sequenced, derived };
enum waveform { SINE, SQUARE, SAW, NOISE, RAMP };
enum bound { MIN, MAX };

/**
 *  JSON KEYS
 */

const std::string SOUND_OUTPUT_PATH_KEY = "USER_SOUND_OUTPUT_PATH";
const std::string SAMPLE_RATE_KEY = "SAMPLE_RATE";

/**
 *  DEFAULT USER PATHS
 */

#ifdef __APPLE__
const std::string CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-osx.sh";
const std::string DEFAULT_SAMPLE_PATH =
    "/Library/Application Support/EmissionControl20/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH =
    "/Library/Application Support/EmissionControl20/soundOutput/";
const std::string DEFAULT_PRESETS_PATH =
    "/Library/Application Support/EmissionControl20/presets/";
const std::string DEFAULT_CONFIG_PATH =
    "/Library/Application Support/EmissionControl20/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif

#ifdef __linux__
const std::string CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-linux.sh";
const std::string DEFAULT_SAMPLE_PATH = "/opt/EmissionControl20/samples";
const std::string DEFAULT_SOUND_OUTPUT_PATH =
    "/opt/EmissionControl20/soundOutput";
const std::string DEFAULT_PRESETS_PATH = "/opt/EmissionControl20/presets";
const std::string DEFAULT_CONFIG_PATH = "/opt/EmissionControl20/configs";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
const std::string DEFAULT_RESOURCE_PATH = "/opt/EmissionControl20/Resources/";
#endif

#ifdef _WIN32_
const std::string CONFIG_DIR_SCRIPT_PATH =
    "Resources/config_scripts/configAbsoDirectories-osx.sh";
const std::string DEFAULT_SAMPLE_PATH =
    "/Library/Application Support/EmissionControl20/samples/";
const std::string DEFAULT_SOUND_OUTPUT_PATH =
    "/Library/Application Support/EmissionControl20/soundOutput/";
const std::string DEFAULT_PRESETS_PATH =
    "/Library/Application Support/EmissionControl20/presets/";
const std::string DEFAULT_CONFIG_PATH =
    "/Library/Application Support/EmissionControl20/configs/";
const std::string DEFAULT_CONFIG_FILE = DEFAULT_CONFIG_PATH + "config.json";
#endif
}
#endif