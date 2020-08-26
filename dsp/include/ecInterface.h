/**
 * ecInterface.h
 *
 * AUTHOR: Jack Kilgore
 */

#ifndef ECINTERFACE_H
#define ECINTERFACE_H

/**** Emission Control LIB ****/
#include "ecSynth.h"

/**** AlloLib LIB ****/
#include "al/app/al_App.hpp"
#include "al/io/al_MIDI.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"

/**** External LIB ****/
#include "../external/nativefiledialog/src/include/nfd.h"

/**** C STD LIB ****/
#include <array>

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

  /** MIDI Stuff **/
  void initMIDI();
  void updateActiveMIDIParams(const al::MIDIMessage &m);

  /**
   * @brief Called everytime a MIDI message is sent.
   */
  virtual void onMIDIMessage(const al::MIDIMessage &m) override;

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
  PresetHandlerState &ECdrawPresetHandler(al::PresetHandler *presetHandler,
                                          int presetColumns, int presetRows);

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
  void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate,
                          uint32_t numChannels, std::string directory = "",
                          uint32_t bufferSize = 0);

private:
  float windowWidth, windowHeight;

  bool noSoundFiles, light, isPaused = false, writeSampleRate = false;
  float background = 0.21;
  ecSynth granulator;
  al::PresetHandler mPresets;
  al::OutputRecorder mRecorder;
  Clipper mHardClip;

  RtMidiIn midiIn;
  std::vector<MIDIKey> ActiveMIDI;
  bool mIsLinkingParamAndMIDI = false;
  MIDILearnBool mMIDILearn;
  MIDIKey mCurrentLearningMIDIKey;

  /**
   * TESTING MIDI Presets
   */

  void writeMIDIPreset(std::string name) {
    json midi_config = json::array();
    std::ifstream ifs(userPath + "/Desktop/test.json");
    if (ifs.is_open()) {
      json temp;
      for (int index = 0; index < ActiveMIDI.size(); index++) {
        ActiveMIDI[index].toJSON(temp);
        midi_config.push_back(temp);
      }
    } else {
      json temp;
      for (int index = 0; index < ActiveMIDI.size(); index++) {
        ActiveMIDI[index].toJSON(temp);
        midi_config.push_back(temp);
      }
    }

    std::ofstream file((userPath + "/Desktop/test.json").c_str());
    if (file.is_open())
      file << midi_config;
  }

  /**
   * @brief: Removes all MIDI tied to paramKey in the ActiveMIDI vector.
   */
  void unlinkParamAndMIDI(MIDIKey &paramKey) {
    int index;
    bool found = false;
    for (index = 0; index < ActiveMIDI.size(); index++) {
      if (ActiveMIDI[index].getKeysIndex() == paramKey.getKeysIndex() &&
          ActiveMIDI[index].getType() == paramKey.getType()) {
        found = true;
        break;
      }
    }
    if (found)
      ActiveMIDI.erase(ActiveMIDI.begin() + index);
  }

  /**
   * @brief: Update ECParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in ECParameters structure.
   */
  void updateECParamMIDI(float val, int index) {
    val = granulator.ECParameters[index]->getCurrentMin() +
          (val * abs(granulator.ECParameters[index]->getCurrentMax() -
                     granulator.ECParameters[index]->getCurrentMin()));
    granulator.ECParameters[index]->setParam(val);
  }

  /**
   * @brief: Update ECModParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in ECModParameters structure.
   */
  void updateECModParamMIDI(float val, int index) {
    val = granulator.ECModParameters[index]->param.getCurrentMin() +
          (val * abs(granulator.ECModParameters[index]->param.getCurrentMax() -
                     granulator.ECModParameters[index]->param.getCurrentMin()));
    granulator.ECModParameters[index]->param.setParam(val);
  }

  /**
   * @brief: Update LFOParameters object at index based on value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in LFOParameters structure.
   */
  void updateLFOParamMIDI(float val, int index) {
    val = granulator.LFOParameters[index]->frequency->getCurrentMin() +
          (val *
           abs(granulator.LFOParameters[index]->frequency->getCurrentMax() -
               granulator.LFOParameters[index]->frequency->getCurrentMin()));
    granulator.LFOParameters[index]->frequency->setParam(val);
  }

  /**
   * @brief: Update duty cycle of LFOParameters object at index based on
   * value.
   *
   * @param[in] value: A value between 0 and 1. Percentage of parameter range.
   * @param[in] index: Index in LFOParameters structure.
   */
  void updateLFODutyParamMIDI(float val, int index) {
    granulator.LFOParameters[index]->duty->set(val);
  }

  std::string soundOutput, execDir, execPath, userPath, configFile, presetsPath;
  al::File f;
  nfdchar_t *outPath = NULL;
  nfdpathset_t pathSet;
  nfdresult_t result;
  std::string currentFile = "No file selected";
  std::string previousFile = "No file selected";
  ImFont *bodyFont;
  ImFont *titleFont;
  float fontScale = 1.0;
  float adjustScaleY = 1.0;

  double globalSamplingRate = consts::SAMPLE_RATE;

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse |
                           ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
                           ImGuiWindowFlags_NoSavedSettings;
  ImGuiWindowFlags graphFlags = flags;

  int framecounter = 0;
  std::vector<float> streamHistory = std::vector<float>(80, 0);
  int highestStreamCount = 2;
  int grainAccum = 0;
  float oscFrame = 1;
  double lastSamplingRate = globalSamplingRate;

  std::vector<float> oscDataL =
      std::vector<float>(int(oscFrame *globalSamplingRate), 0);
  std::vector<float> oscDataR =
      std::vector<float>(int(oscFrame *globalSamplingRate), 0);
  std::vector<float> blackLine = std::vector<float>(2, 0);

  int VUdataSize = globalSamplingRate / 30;
  int lastVUdataSize = VUdataSize;

  std::vector<float> VUdataLeft = std::vector<float>(VUdataSize, 0);
  std::vector<float> VUdataRight = std::vector<float>(VUdataSize, 0);

  // Colors

  // light color scheme
  ImColor PrimaryLight = ImColor(143, 157, 163); // Background
  ImColor YellowLight = ImColor(181, 137, 0);    // Yellow
  ImColor RedLight = ImColor(120, 29, 57);       // Red
  ImColor GreenLight = ImColor(58, 106, 10);     // Green
  ImColor Shade1Light = ImColor(171, 182, 186);  // Slider Color 1
  ImColor Shade2Light = ImColor(199, 206, 209);  // Slider Color 2
  ImColor Shade3Light = ImColor(227, 231, 232);  // Slider Color 3
  ImColor TextLight = ImColor(0, 0, 0);          // Text Color

  // dark color scheme
  ImColor PrimaryDark = ImColor(33, 38, 40);   // Background
  ImColor YellowDark = ImColor(208, 193, 113); // Yellow
  ImColor RedDark = ImColor(184, 100, 128);    // Red
  ImColor GreenDark = ImColor(106, 154, 60);   // Green
  ImColor Shade1Dark = ImColor(55, 63, 66);    // Slider Color 1
  ImColor Shade2Dark = ImColor(76, 88, 92);    // Slider Color 2
  ImColor Shade3Dark = ImColor(98, 113, 118);  // Slider Color 3
  ImColor TextDark = ImColor(255, 255, 255);   // Text Color

  ImColor *PrimaryColor;
  ImColor *ECyellow;
  ImColor *ECred;
  ImColor *ECgreen;
  ImColor *Shade1;
  ImColor *Shade2;
  ImColor *Shade3;
  ImColor *Text;

  void drawAudioIO(al::AudioIO *io);

  void setGUIParams();

  int getSampleRateIndex();

  /**** Configuration File Stuff****/

  bool initJsonConfig();

  // These will have dependencies on the userPath member, MAKE SURE TO INIT IT
  // FIRST.˝
  bool jsonWriteSoundOutputPath(std::string path);

  template <typename T> bool jsonWriteToConfig(T value, std::string key);

  /**
   * @brief Read json config file and write output path to soundOutput member
   * variable.
   *
   *
   */
  void jsonReadAndSetSoundOutputPath();
  void jsonReadAndSetAudioSettings();
  void jsonReadAndSetColorSchemeMode();
  void jsonReadAndSetFontScale();
};

#endif