/**
 * ecSynth.h
 *
 * AUTHOR: Jack Kilgore
 */

#ifndef ECSYNTH_H
#define ECSYNTH_H

/**** Emission Control LIB ****/
#include "const.h"
#include "emissionControl.h"

/**** AlloLib LIB ****/
#include "al/scene/al_PolySynth.hpp"
#include "al/ui/al_Parameter.hpp"

/**** CSTD LIB ****/
#include <math.h>
#include <memory>

class ecSynth : public al::SynthVoice {
 public:
  /**
   * Ringbuffers for oscilloscope
   */
  unsigned int oscBufferSize = 96000 * 5;
  util::RingBuffer oscBufferL{oscBufferSize};
  util::RingBuffer oscBufferR{oscBufferSize};

  /**
   * The number of modulators in ecSynth.
   */
  const static int NUM_MODULATORS = 4;

  // array of lfo parameters to draw later
  std::vector<LFOstruct *> LFOparameters;

  /**
   * PUBLIC PARAMETERS OF SYNTH
   */
  ecParameter grainRate{"Grainrate", "1. Grain rate", "", 1, "", 0.1, 100, 0, 500, consts::SINE, 0};
  al::ParameterMenu grainRateLFO{"##grainRateLFO"};
  ecParameter modGrainRateDepth{"modGrainRateDepth", "modGrainRateDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter asynchronicity{"Asynchronicity", "2. Asynchronicity", "", 0.0, "", 0.0, 1.0, 0, 1,
                             consts::SINE};
  al::ParameterMenu asyncLFO{"##asyncLFO"};
  ecParameter modAsynchronicityDepth{
    "modAsynchronicityDepth", "modAsynchronicityDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter intermittency{"Intermittancy", "3. Intermittancy", "", 0, "", 0, 1, 0, 1};
  al::ParameterMenu intermittencyLFO{"##intermittencyLFO"};
  ecParameter modIntermittencyDepth{
    "modIntermittencyDepth", "modIntermittencyDepth", "", 0, "", 0, 1, 0, 1};

  ecParameterInt streams{"Streams", "4. Streams", "", 1, "", 1, 12, 1, 20};
  al::ParameterMenu streamsLFO{"##streamsLFO"};
  ecParameter modStreamsDepth{"modStreamsDepth", "modStreamsDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter grainDurationMs{"GrainDurms", "5. Grain Dur (ms)", "",   30, "", 0.01,
                              1000,         0.0000001,           10000};
  al::ParameterMenu grainDurationLFO{"##grainDurationLFO"};
  ecParameter modGrainDurationDepth{
    "modGrainDurationDepth", "modGrainDurationDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter envelope{"Envelopeshape", "6. Envelope shape", "", 0.5, "", 0, 1, 0, 1};
  al::ParameterMenu envelopeLFO{"##envelopeLFO"};
  ecParameter modEnvelopeDepth{"modEnvelopeDepth", "modEnvelopeDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter transposition{"Pitchshift", "7. Pitch shift", "", 1, "", -2, 2, -20, 20};
  al::ParameterMenu transpositionLFO{"##transpositionLFO"};
  ecParameter modTranspositionDepth{
    "modTranspositionDepth", "modTranspositionDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter filter{"Filterfreq", "8. Filter freq", "", 440, "", 60, 5000, 20, 24000};
  al::ParameterMenu filterLFO{"##filterLFO"};
  ecParameter modFilterDepth{"modFilterDepth", "modFilterDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter resonance{"Resonance", "9. Resonance", "", 0, "", 0, 1, 0, 1};
  al::ParameterMenu resonanceLFO{"##resonanceLFO"};
  ecParameter modResonanceDepth{"modResonanceDepth", "modResonanceDepth", "", 0, "", 0, 1, 0, 1};

  ecParameterInt soundFile{"Soundfile", "10. Sound file", "", 1, "", 1, mClipNum, 1, mClipNum};
  al::ParameterMenu soundFileLFO{"##soundFileLFO"};
  ecParameter modSoundFileDepth{"modSoundFileDepth", "modSoundFileDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter tapeHead{"Scan", "11. Scan", "", 0.5, "", 0, 1, 0, 1};
  al::ParameterMenu tapeHeadLFO{"##scanLFO"};
  ecParameter modTapeHeadDepth{"modScanDepth", "modScanDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter pan{"Pan", "12. Pan", "", 0, "", -1, 1, -1, 1};
  al::ParameterMenu panLFO{"##panLFO"};
  ecParameter modPanDepth{"modPanDepth", "modPanDepth", "", 0, "", 0, 1, 0, 1};

  ecParameter volumeDB{"AmplitudedB", "13. Amplitude (dB)", "", -6, "", -60, 6, -180, 24};
  al::ParameterMenu volumeLFO{"##volumeLFO"};
  ecParameter modVolumeDepth{"modVolumeDepth", "modVolumeDepth", "", 0, "", 0, 1, 0, 1};

  ecSynth() {}

  /**
   * @brief Initilialize the synth.
   */
  virtual void init(al::AudioIOData *io);

  /**
   * @brief Set data needed to perform granulation correctly.
   */
  void setIO(al::AudioIOData *io);

  /**
   * @brief Audio rate processing of synth.
   */
  virtual void onProcess(al::AudioIOData &io) override;

  /**
   * @brief Runs every time a voice is triggered.
   */
  virtual void onTriggerOn() override;

  /**
   * @brief Runs every time a voice is marked as inactive.
   */
  virtual void onTriggerOff() override;

  /**
   * @brief Load sound file into memory.
   *
   * @param[in] The the filepath to the audio file.
   */
  void loadSoundFile(std::string fileName);

  /**
   * @brief Load sound files from designated sample folder.
   *
   * @return Return false if the directory is empty.
   */
  bool loadInitSoundFiles(std::string directory);

  /**
   * @brief Resample all files in SoundClip to match globalSamplingRate.
   */
  void resampleSoundFiles();

  /**
   * @brief Clear sound files stored in memory.
   *
   */
  void clearSoundFiles();

  /**
   * @brief Remove soundfile at a specified index.
   *
   */
  bool removeSoundFile(int index);

  /**
   * @brief Remove soundfile at the index determinded by the soundFile
   * parameters.
   */
  bool removeCurrentSoundFile();

  /**
   * @brief Print out what the synth is doing at runtime.
   *        Used for debugging purposes.
   *
   * @param[in] Verbose toggled on if true, toggled off otherwise.
   */
  void verbose(bool toggle) { grainSynth.verbose(toggle); }

  /**
   * @brief Return the number of active voices.
   *        ie, The number of voices running in the audio callback loop.
   *
   * @return The number of active voices.
   */
  int getActiveVoices() { return mActiveVoices; }

  /** WIP -- see cpp file
   * @brief Nonlinear cubic distortion used for soft clipping.
   *        Applied to the left and right channels.
   *        !Run in the process block!
   */
  void softClip(al::AudioIOData &io);

  /**
   * @brief Simple hardclipping to prevent distorting the DAC.
   *        Applied to the left and right channels.
   *        !Run in the process block!
   */
  void hardClip(al::AudioIOData &io);

  /**
   * WORK IN PROGRESS
   * @brief Throttles the amount of grains that can be active at once. TODO
   *
   * @param[in] The amount of time to the throttle the synth in seconds.
   * @param[in] The ratio in which to throttle the synth. A number from 0 to 1.
   */
  void throttle(float time, float ratio);

  int getNumberOfAudioFiles() const { return soundClip.size(); }

 private:
  double mGlobalSamplingRate = consts::SAMPLE_RATE, mPrevSR = consts::SAMPLE_RATE;

  al::PolySynth grainSynth{};                                  /* Polyhpony and interface to audio
                                                                             callback */
  voiceScheduler grainScheduler;                               /* Schedule grains */
  std::vector<std::shared_ptr<util::buffer<float>>> soundClip; /* Store audio buffers in memory */
  std::vector<std::string> soundClipFileName;
  int mClipNum = 0; /* Number of sound files being stored in memory */
  int mModClip = 0;

  int controlRateCounter = 0;
  int mActiveVoices = 0;
  int mCounter = 0;
  float mAvgActiveVoices = 0;
  float mPeakCPU;
  float mAvgCPU;
  int *mPActiveVoices = nullptr;

  std::vector<std::shared_ptr<ecModulator>> Modulators;
};

#endif
