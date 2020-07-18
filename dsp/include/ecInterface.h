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
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"

/**** External LIB ****/
#include "../external/nativefiledialog/src/include/nfd.h"

class ecInterface : public al::App {
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
  static PresetHandlerState &ECdrawPresetHandler(al::PresetHandler *presetHandler,
                                                 int presetColumns, int presetRows);

 private:
  bool noSoundFiles, light, isPaused = false, writeSampleRate = false;
  float background = 0.21;
  ecSynth granulator;
  al::PresetHandler mPresets;
  al::OutputRecorder mRecorder;

  std::string soundOutput, execDir, execPath, userPath, configFile, presetsPath;
  ;
  al::File f;
  nfdchar_t *outPath = NULL;
  nfdpathset_t pathSet;
  nfdresult_t result;
  std::string currentFile = "No file selected";
  std::string previousFile = "No file selected";

  double globalSamplingRate = consts::SAMPLE_RATE;

  ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                           ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;

  int framecounter = 0;
  std::vector<float> streamHistory = std::vector<float>(80, 0);
  float oscFrame = 1;
  double lastSamplingRate = globalSamplingRate;

  std::vector<float> oscDataL = std::vector<float>(int(oscFrame *globalSamplingRate), 0);
  std::vector<float> oscDataR = std::vector<float>(int(oscFrame *globalSamplingRate), 0);
  std::vector<float> blackLine = std::vector<float>(2, 0);

  int VUdataSize = globalSamplingRate / 30;
  int lastVUdataSize = VUdataSize;

  std::vector<float> VUdataLeft = std::vector<float>(VUdataSize, 0);
  std::vector<float> VUdataRight = std::vector<float>(VUdataSize, 0);

  // Colors

  // light color scheme
  ImColor PrimaryLight = ImColor(98, 113, 118);    // Background
  ImColor SecondaryLight = ImColor(139, 127, 58);  // Oscilloscope L
  ImColor TertiaryLight = ImColor(123, 52, 76);    // Oscilloscope R
  ImColor Shade1Light = ImColor(133, 144, 148);    // Slider Color 1
  ImColor Shade2Light = ImColor(167, 175, 178);    // Slider Color 2
  ImColor Shade3Light = ImColor(202, 207, 208);    // Slider Color 3
  ImColor TextLight = ImColor(0, 0, 0);            // Text Color

  // dark color scheme
  ImColor PrimaryDark = ImColor(33, 38, 40);       // Background
  ImColor SecondaryDark = ImColor(208, 193, 113);  // Oscilloscope L
  ImColor TertiaryDark = ImColor(184, 100, 128);   // Oscilloscope R
  ImColor Shade1Dark = ImColor(55, 63, 66);        // Slider Color 1
  ImColor Shade2Dark = ImColor(76, 88, 92);        // Slider Color 2
  ImColor Shade3Dark = ImColor(98, 113, 118);      // Slider Color 3
  ImColor TextDark = ImColor(255, 255, 255);       // Text Color

  ImColor *PrimaryColor;
  ImColor *SecondaryColor;
  ImColor *TertiaryColor;
  ImColor *Shade1;
  ImColor *Shade2;
  ImColor *Shade3;
  ImColor *Text;

  // // old green color scheme
  // ImColor PrimaryLight = ImColor(0.459f, 0.592f, 0.518f);    // Background
  // ImColor SecondaryLight = ImColor(0.655f, 0.561f, 0.451f);  // Oscilloscope
  // L
  // ImColor TertiaryLight = ImColor(0.569f, 0.388f, 0.459f);   // Oscilloscope
  // R
  // ImColor Shade1Light = ImColor(0.612f, 0.690f, 0.647f);     // Slider Color
  // 1
  // ImColor Shade2Light = ImColor(0.772f, 0.807f, 0.788f);     // Slider Color
  // 2
  // ImColor Shade3Light = ImColor(0.929f, 0.933f, 0.929f);     // Slider Color
  // 3
  // ImColor TextLight = ImColor(0.929f, 0.933f, 0.929f);       // Text Color

  void drawAudioIO(al::AudioIO *io);

  void drawLFOcontrol(ecSynth &synth, int lfoNumber);

  void drawModulationControl(al::ParameterMenu &menu, ecParameter &slider);

  void setGUIColors();

  void setPlotConfig();

  int getSampleRateIndex();

  /**** Configuration File Stuff****/
  // TO DO TO DO -- Make Linux cross platform with Rodney

  void initFileIOPaths();

  bool initJsonConfig();

  // These will have dependencies on the userPath member, MAKE SURE TO INIT IT
  // FIRST.˝
  bool jsonWriteSoundOutputPath(std::string path);

  template <typename T>
  bool jsonWriteToConfig(T value, std::string key);

  /**
   * @brief Read json config file and write output path to soundOutput member
   * variable.
   *
   *
   */
  void jsonReadAndSetSoundOutputPath();
  void jsonReadAndSetAudioSettings();
  void jsonReadAndSetColorSchemeMode();
};

/**
 * @brief Modified version of al's soundfilerecordGUI.
 *
 * @param[in] Output recorder object.
 *
 * @param[in] Path of directory where the outputted sound files will be stored.
 *
 * @param[in] Frame rate of outputted file.
 *
 * @param[in] Number of channels of outputted file.
 *
 * @param[in] Amount of space allocated for sound.
 */
static void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate, uint32_t numChannels,
                               std::string directory = "", uint32_t bufferSize = 0);

#endif