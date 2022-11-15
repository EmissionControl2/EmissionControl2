/**
 * ecInterface.h
 *
 * AUTHOR: Jack Kilgore
 */

#ifndef ECINTERFACE_H
#define ECINTERFACE_H

/**** Emission Control LIB ****/
#include "ecSynth.h"
#include "fonts.hpp"

/**** AlloLib LIB ****/
#include "al/app/al_App.hpp"
#include "al/io/al_MIDI.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"

/**** External LIB ****/
#include "../external/nativefiledialog/src/include/nfd.h"
#include "imgui_internal.h"

/**** C STD LIB ****/
#include <array>
#include <unordered_set>

class ecInterface : public al::App, public al::MIDIMessageHandler {
 public:
  /**
   * @brief Initilialize the synth interface.
   */
  virtual void onInit() override;

  /**
   * @brief Run once on starup.
   */
  virtual void onCreate() override;

  /**
   * @brief Audio rate processing of synth.
   */
  virtual void onSound(al::AudioIOData &io) override;

  /**
   * @brief Draw rate processing of synth interface.
   */
  virtual void onDraw(al::Graphics &g) override;

  virtual bool onMouseDown(const al::Mouse &m) override;

  /** MIDI Stuff **/
  void initMIDI();
  void updateActiveMIDIParams(const al::MIDIMessage &m);

  /**
   * @brief Called everytime a MIDI message is sent.
   */
  virtual void onMIDIMessage(const al::MIDIMessage &m) override;

  virtual bool onKeyDown(al::Keyboard const &k) override;

  virtual void onExit() override;

  /**
   * @brief OSC Input handling
   */
  virtual void onMessage(al::osc::Message &m) override;

  // struct pulled from al_ParameterGUI.hpp for custom preset draw function
  struct PresetHandlerState {
    std::string currentBank;
    int currentBankIndex = 0;
    std::vector<std::string> mapList;
    int presetHandlerBank = 0;
    bool newMap = false;
    std::string enteredText;
    std::string newMapText;
    bool storeButtonState{false};
  };

  // Custom preset draw function (copied and modified from al_ParameterGUI.hpp)
  PresetHandlerState &ECdrawPresetHandler(al::PresetHandler *presetHandler, int presetColumns,
                                          int presetRows);

  /**
   * @brief Modified version of al's soundfilerecordGUI.
   *
   * @param[in] Output recorder object.
   *
   * @param[in] Path of directory where the outputted sound files will be
   * stored.
   *
   * @param[in] Frame rate of outputted file.
   *
   * @param[in] Number of channels of outputted file.
   *
   * @param[in] Amount of space allocated for sound.
   */
  void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate, uint32_t numChannels,
                          std::string directory = "", uint32_t bufferSize = 0);

 private:
  std::mutex mLock;
  float windowWidth, windowHeight;
  bool isFullScreen, isFirstLaunch;
  std::vector<std::string> failed_paths;
  std::string currentAudioDevice;

  bool light, firstFrame = true, readyToTrigNoSoundFilePopup = true, isPaused = false,
              saveDefaultAudio = false, isStereo = true, isHardClip = true,
              isHardResetScanBegin = false, isOmitSoundFileParam = false;
  float background = 0.21;
  ecSynth granulator;
  std::vector<std::string> SamplePaths;
  std::unique_ptr<al::PresetHandler> mPresets;
  std::map<int, std::string> currentPresetMap;
  al::OutputRecorder mRecorder;
  Clipper mHardClip;

  KeyDown mLastKeyDown;

  bool captureKeyboardTextInput = false;

  std::array<RtMidiIn, consts::MAX_MIDI_IN> midiIn;
  std::vector<MIDIKey> ActiveMIDI;
  bool mIsLinkingParamAndMIDI = false;
  char mCurrentMIDIPresetName[128] = "midi_preset";
  char mCurrentOSCPresetName[128] = "osc_preset";
  char mCurrentSoundFilePresetName[128] = "sound_file_preset";
  bool allowMIDIPresetOverwrite = false;
  bool allowOSCPresetOverwrite = false;
  bool allowSoundFilePresetOverwrite = false;
  MIDILearnBool mMIDILearn;
  MIDIKey mCurrentLearningMIDIKey;
  std::unordered_set<std::string> MIDIPresetNames;
  std::unordered_set<std::string> OSCPresetNames;
  std::unordered_set<std::string> SoundFilePresetNames;
  std::vector<bool> SelectedMIDIDevices;
  int unlearnFlash = 0;

  /*
  OSC
  */
  std::string oscValString;
  float oscValFloat;
  int oscValInt;
  double oscValDouble;

  std::string oscMessageType;
  bool isOSCOn = false;
  int oscPort = 16447;                        // osc port
  std::string oscAddr = "127.0.0.1";          // ip address
  int previousOscPort = 16447;                // osc port
  std::string previousOscAddr = "127.0.0.1";  // ip address
  float oscTimeout = 0.02;
  ImGuiInputTextCallback oscAddrCallback;
  void *oscAddrCallbackUserData;
  std::unique_ptr<al::osc::Recv> oscServer;  // create an osc server (listener)
  bool isOscWarningWindow = false;

  std::string morphTimeOSCArg = "/morphTime";
  ImGuiInputTextCallback morphTimeOSCCallback;
  void *morphTimeOSCCallbackUserData;
  float morphTimeOscMin = 0;
  float morphTimeOscMax = MAX_MORPH_TIME;
  bool morphTimeOscCustomRange = 0;

  std::string presetOSCArg = "/preset";
  ImGuiInputTextCallback presetOSCCallback;
  void *presetOSCCallbackUserData;

  std::string fileNameOSCArg = "/fileName";
  ImGuiInputTextCallback fileNameOSCCallback;
  void *fileNameOSCCallbackUserData;

  std::string outputFolderOSCArg = "/outputFolder";
  ImGuiInputTextCallback outputFolderOSCCallback;
  void *outputFolderOSCCallbackUserData;

  std::string recordOSCArg = "/record";
  ImGuiInputTextCallback recordOSCCallback;
  void *recordOSCCallbackUserData;

  std::string buf1 = "test.wav";
  ImGuiInputTextCallback buf1Callback;
  void *buf1CallbackUserData;
  std::string recordFilename;

  void resetOSC() {
    if (oscServer != nullptr) oscServer->stop();
    oscServer.reset();
    oscServer = std::make_unique<al::osc::Recv>(oscPort, oscAddr.c_str(), 0.02);
    if (oscServer->isOpen()) {
      oscServer->handler(oscDomain()->handler());
      oscServer->start();
      std::cout << "OSC IP Address: " << oscAddr << std::endl;
      std::cout << "OSC Port: " << oscPort << std::endl;
      std::cout << "OSC Timeout: " << oscTimeout << std::endl;
      previousOscAddr = oscAddr;
      previousOscPort = oscPort;
    } else {
      std::cerr << "Could not bind to UDP socket. Is there a server already bound to that port?"
                << std::endl;
      oscAddr = previousOscAddr;
      oscPort = previousOscPort;
      oscServer.reset();
      oscServer = std::make_unique<al::osc::Recv>(oscPort, oscAddr.c_str(), 0.02);
      isOscWarningWindow = true;
    }
  }

  void clearActiveMIDI() {
    ActiveMIDI.clear();
    mIsLinkingParamAndMIDI = false;
  }

  /**
   * @brief: Removes all MIDI tied to paramKey in the ActiveMIDI vector.
   */
  void unlinkParamAndMIDI(MIDIKey &paramKey);

  /**
   * @brief: Update ECParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in ECParameters structure.
   */
  inline void updateECParamMIDI(float val, int index) {
    float result = util::outputValInRange(val, granulator.ECParameters[index]->getCurrentMin(),
                                          granulator.ECParameters[index]->getCurrentMax(),
                                          granulator.ECParameters[index]->isLog(), 3);
    granulator.ECParameters[index]->setParam(result);
  }

  /**
   * @brief: Update ECModParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in ECModParameters structure.
   */
  inline void updateECModParamMIDI(float val, int index) {
    float result =
      util::outputValInRange(val, granulator.ECModParameters[index]->param.getCurrentMin(),
                             granulator.ECModParameters[index]->param.getCurrentMax(),
                             granulator.ECModParameters[index]->param.isLog(), 3);
    granulator.ECModParameters[index]->param.setParam(result);
  }

  /**
   * @brief: Update LFOParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in LFOParameters structure.
   */
  inline void updateLFOParamMIDI(float val, int index) {
    float result =
      util::outputValInRange(val, granulator.LFOParameters[index]->frequency->getCurrentMin(),
                             granulator.LFOParameters[index]->frequency->getCurrentMax(),
                             granulator.LFOParameters[index]->frequency->isLog(), 3);
    granulator.LFOParameters[index]->frequency->setParam(result);
  }

  /**
   * @brief: Update duty cycle of LFOParameters object at index based on
   * value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in LFOParameters structure.
   */
  inline void updateLFODutyParamMIDI(float val, int index) {
    granulator.LFOParameters[index]->duty->set(val);
  }

  inline void updatePresetMorphParamMIDI(float val) {
    // Hard code to be logarithmic in scale.
    // Offset by 1.0 because it FEEEELS nice.
    float result = util::outputValInRange(val, 1.0, consts::MAX_MORPH_TIME + 1.0, true, 3);
    mPresets->setMorphTime(result - 1.0);
  }

  std::string opener = "open ";
  std::string manualURL =
    "https://raw.githubusercontent.com/EmissionControl2/EmissionControl2/master/docs/"
    "EmissionControl2-Manual.pdf";
  std::string bugReportURL = "https://github.com/jackkilgore/EmissionControl2/issues";
  std::string licenseURL = "https://www.gnu.org/licenses/gpl-3.0.html";
  std::vector<std::string> aboutLines = {
    ("Version " + consts::VERSION_NUMBER).c_str(),
    "Project Manager: Curtis Roads",
    "Developers: Jack Kilgore, Rodney DuPlessis",
    "CREATE (Center for Research in Electronic Art Technology)",
    "University of California, Santa Barbara",
    " ",
    "Thanks to the Allosphere Research Group for their work on Allolib,",
    "the framework on which this software is built.",
    "Special thanks to Dr. Andres Cabrera for invaluable help and guidance",
    "and for handling the Windows port of the software",
    "Supported by a Faculty Research Grant from the UCSB Academic Senate",
    " ",
    "Copyright 2020 Curtis Roads, Jack Kilgore, Rodney Duplessis",
    "This program comes with absolutely no warranty.",
    "See the GNU General Public License, version 3 or later for details."};
  std::vector<std::string> MIDIHelpLines = {
    "MIDI Learn ",
    "MIDI Learn allows you to control a parameter in EC2",
    "with a MIDI fader or knob on your MIDI controller.",
    " ",
    "After you have set up your MIDI Device in EC2, you can MIDI Learn",
    "by hovering over a parameter and pressing \"m\" on your keyboard.",
    "Then, simply move the control on your device that you want to pair.",
    "To unlearn, hover over a parameter and press \"shift+m\" on your keyboard.",
    " ",
    "The parameters that are MIDI Learnable are:",
    " ",
    "All of the sliders in GRANULATION CONTROLS",
    "All of the sliders in MODULATION CONTROLS",
    "All of the sliders in LFO CONTROLS",
    "The Morph Time slider in PRESETS"};
  std::string soundOutput, execDir, execPath, userPath, configFile, presetsPath, midiPresetsPath,
    oscPresetsPath, samplePresetsPath;
  nfdchar_t *outPath = NULL;
  nfdpathset_t pathSet;
  nfdresult_t result;
  std::string currentFile = "No file selected";
  std::string previousFile = "No file selected";
  ImFont *bodyFont;
  ImFont *titleFont;
  ImFont *engineFont;
  float fontScale = 1.0;
  float adjustScaleY = 1.0;

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
  ImGuiWindowFlags graphFlags = flags;

  int framecounter = 0;
  std::vector<float> streamHistory = std::vector<float>(80, 0);
  int highestStreamCount = 2;
  int grainAccum = 0;
  int grainsPerSecond = 0;
  float oscilloscopeFrame = 1.0f;

  std::vector<float> blackLine = std::vector<float>(2, 0);

  std::vector<std::unique_ptr<std::vector<float>>> audioThumbnails;

  float GrainDisplayIndices[consts::MAX_GRAIN_DISPLAY];
  int numGrainsToDisplay;

  // Colors
  int colPushCount = 0;

  // light color scheme
  ImColor PrimaryLight = ImColor(149, 176, 176);  // Background
  ImColor YellowLight = ImColor(237, 224, 39);    // Yellow
  ImColor RedLight = ImColor(212, 35, 89);        // Red
  ImColor GreenLight = ImColor(69, 201, 69);      // Green
  ImColor BlueLight = ImColor(44, 113, 175);      // Blue
  ImColor Shade1Light = ImColor(176, 196, 196);   // Slider Color 1
  ImColor Shade2Light = ImColor(199, 213, 213);   // Slider Color 2
  ImColor Shade3Light = ImColor(221, 230, 230);   // Slider Color 3
  ImColor TextLight = ImColor(0, 0, 0);           // Text Color

  // dark color scheme
  ImColor PrimaryDark = ImColor(33, 38, 40);   // Background
  ImColor YellowDark = ImColor(122, 114, 0);   // Yellow
  ImColor RedDark = ImColor(170, 8, 76);       // Red
  ImColor GreenDark = ImColor(8, 159, 8);      // Green
  ImColor BlueDark = ImColor(15, 75, 129);     // Blue
  ImColor Shade1Dark = ImColor(55, 63, 66);    // Slider Color 1
  ImColor Shade2Dark = ImColor(76, 88, 92);    // Slider Color 2
  ImColor Shade3Dark = ImColor(98, 113, 118);  // Slider Color 3
  ImColor TextDark = ImColor(255, 255, 255);   // Text Color

  ImColor *PrimaryColor;
  ImColor *ECyellow;
  ImColor *ECred;
  ImColor *ECgreen;
  ImColor *ECblue;
  ImColor *Shade1;
  ImColor *Shade2;
  ImColor *Shade3;
  ImColor *Text;

  void drawAudioIO(al::AudioIO *io);

  void setGUIParams();

  int getSampleRateIndex();

  int getBufSizeIndex();

  /**** Configuration File Stuff****/

  bool initJsonConfig();

  // These will have dependencies on the userPath member, MAKE SURE TO INIT IT
  // FIRST.˝
  bool jsonWriteSoundOutputPath(std::string path);

  template <typename T>
  bool jsonWriteToConfig(T value, std::string key);

  bool jsonWriteMapToConfig(std::unordered_set<std::string> &presetNames, std::string key);

  /**
   * @brief Read json config file and write output path to soundOutput member
   * variable.
   *
   *
   */
  json jsonReadConfig();
  void setMIDIPresetNames(json preset_names);
  void setOSCPresetNames(json preset_names);
  void setSoundFilePresetNames(json preset_names);
  void setSoundOutputPath(std::string sound_output_path);
  void setAudioSettings(float sample_rate, int buffer_size);
  void setColorSchemeMode(bool is_light);
  void setFontScale(float font_scale);
  void setWindowDimensions(float width, float height);
  void setFirstLaunch(bool is_first_launch) { isFirstLaunch = is_first_launch; }
  void setInitFullscreen(bool fullscreen) { isFullScreen = fullscreen; }
  void setAudioDevice(std::string audio_device) { currentAudioDevice = audio_device; }
  void setHardClip(bool hard) { isHardClip = hard; }
  void setHardResetScanBegin(bool hard) {
    isHardResetScanBegin = hard;
    granulator.setHardScanBegin(hard);
  }
  void setOmitSoundFileParam(bool omit) { isOmitSoundFileParam = omit; }

  // MIDI Preset Json files
  void writeJSONMIDIPreset(std::string name, bool allowOverwrite);
  void loadJSONMIDIPreset(std::string midi_preset_name);
  void deleteJSONMIDIPreset(std::string midi_preset_name);

  // OSC Preset Json files
  void writeJSONOSCPreset(std::string name, bool allowOverwrite);
  void loadJSONOSCPreset(std::string osc_preset_name);
  void deleteJSONOSCPreset(std::string osc_preset_name);

  // Sound File Preset Json files
  void writeJSONSoundFilePreset(std::string name, bool allowOverwrite);
  std::vector<std::string> loadJSONSoundFilePreset(std::string sound_file_preset_name);
  void deleteJSONSoundFilePreset(std::string sound_file_preset_name);

  // make a new audioThumbnail when a new sound file is loaded.
  void createAudioThumbnail(float *soundFile, int lengthInSamples);

  IMGUI_API bool InputText(const char *label, std::string *str, ImGuiInputTextFlags flags = 0,
                           ImGuiInputTextCallback callback = NULL, void *user_data = NULL);
};

#endif
