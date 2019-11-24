#ifndef ECINTERFACE_H
#define ECINTERFACE_H

/**** Emission Control LIB ****/
#include "ecSynth.h"

/**** AlloLib LIB ****/
#include "al/app/al_App.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"
#include "al/ui/al_FileSelector.hpp"

class ecInterface : public al::App
{
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
  float background = 0.21;
  ecSynth granulator;
  al::PresetHandler mPresets{"presets"};
  al::OutputRecorder mRecorder;
  al::FileSelector selector;
  std::string currentFile = "No file selected";

};

#endif