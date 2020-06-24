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

 private:
  bool noSoundFiles, isPaused = false, writeSampleRate = false;
  float background = 0.21;
  ecSynth granulator;
  al::PresetHandler mPresets;
  al::OutputRecorder mRecorder;

  std::string soundOutput, execDir, execPath, userPath;
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

  // Colors

  // light color scheme
  ImColor PrimaryLight = ImColor(0.459f, 0.592f, 0.518f);
  ImColor SecondaryLight = ImColor(0.655f, 0.561f, 0.451f);
  ImColor TertiaryLight = ImColor(0.569f, 0.388f, 0.459f);
  ImColor Shade1Light = ImColor(0.612f, 0.690f, 0.647f);
  ImColor Shade2Light = ImColor(0.772f, 0.807f, 0.788f);
  ImColor Shade3Light = ImColor(0.929f, 0.933f, 0.929f);

  // dark color scheme
  ImColor PrimaryDark = ImColor(0.384f, 0.443f, 0.463f);
  ImColor SecondaryDark = ImColor(0.569f, 0.388f, 0.459f);
  ImColor TertiaryDark = ImColor(0.655f, 0.561f, 0.451f);
  ImColor Shade1Dark = ImColor(0.537f, 0.643f, 0.675f);
  ImColor Shade2Dark = ImColor(0.772f, 0.807f, 0.788f);
  ImColor Shade3Dark = ImColor(0.929f, 0.933f, 0.929f);

  ImColor *PrimaryColor = &PrimaryLight;
  ImColor *SecondaryColor = &SecondaryLight;
  ImColor *TertiaryColor = &TertiaryLight;
  ImColor *Shade1 = &Shade1Light;
  ImColor *Shade2 = &Shade2Light;
  ImColor *Shade3 = &Shade3Light;

  bool light = true;

  void drawAudioIO(al::AudioIO *io);

  void drawLFOcontrol(ecSynth &synth, int lfoNumber);

  void drawModulationControl(al::ParameterMenu &menu, al::Parameter *slider);

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
   * @brief Read json config file and write output path to soundOutput member variable.
   *
   *
   */
  void jsonReadAndSetSoundOutputPath();
  void jsonReadAndSetAudioSettings();
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