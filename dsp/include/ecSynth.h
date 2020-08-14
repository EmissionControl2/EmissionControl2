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
#include <array>

class ecSynth : public al::SynthVoice {
public:
  std::array<std::unique_ptr<ecParameter>,consts::NUM_PARAMS> ECParameters;
  std::array<std::unique_ptr<ecModParameter>,consts::NUM_PARAMS> ECModParameters;

  // ecModParameter test{"grainRate"};
  // std::array<std::unique_ptr<ecPar

  /**
   * Ringbuffers for oscilloscope
   */
  unsigned int oscBufferSize = 96001 * 3; // max size of scope window plus a few
                                          // samples because I'm paranoid -
                                          // Rodney
  util::RingBuffer oscBufferL{oscBufferSize};
  util::RingBuffer oscBufferR{oscBufferSize};

  /**
   * Ringbuffers for VU Meter
   */
  unsigned int vuBufferSize = 4096;
  util::RingBuffer vuBufferL{vuBufferSize};
  util::RingBuffer vuBufferR{vuBufferSize};
  /**
   * Vars for indicating clipping
   */
  int clipL = 0;
  int clipR = 0;

  /**
   * The number of modulators in ecSynth.
   */
  const static int NUM_MODULATORS = 4;

  // array of lfo parameters to draw later
  std::vector<LFOstruct *> LFOparameters;

  std::vector<std::shared_ptr<util::buffer<float>>>
      soundClip;    /* Store audio buffers in memory */
  int mClipNum = 0; /* Number of sound files being stored in memory */
  int mModClip = 0;

  /**
   * PUBLIC PARAMETERS OF SYNTH
   */
  ecParameter grainRate{"Grainrate", "1. Grain Rate", "", 1, "", 0.1, 100, 0,
                        500,         consts::PARAM};
  al::ParameterMenu grainRateLFO{"##grainRateLFO"};
  ecParameter modGrainRateDepth{
      "modGrainRateDepth", "modGrainRateDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter asynchronicity{
      "Asynchronicity", "2. Asynchronicity", "", 0.0, "", 0.0, 1.0, 0, 1,
      consts::PARAM};
  al::ParameterMenu asyncLFO{"##asyncLFO"};
  ecParameter modAsynchronicityDepth{"modAsynchronicityDepth",
                                     "modAsynchronicityDepth",
                                     "",
                                     0,
                                     "",
                                     0,
                                     1,
                                     0,
                                     1,
                                     consts::MOD};

  ecParameter intermittency{
      "Intermittancy", "3. Intermittancy", "", 0, "", 0, 1, 0, 1};
  al::ParameterMenu intermittencyLFO{"##intermittencyLFO"};
  ecParameter modIntermittencyDepth{"modIntermittencyDepth",
                                    "modIntermittencyDepth",
                                    "",
                                    0,
                                    "",
                                    0,
                                    1,
                                    0,
                                    1,
                                    consts::MOD};

  ecParameter streams{"Streams", "4. Streams", "", 1,  "",
                      1,         12,           1,  20, consts::INT_PARAM};
  al::ParameterMenu streamsLFO{"##streamsLFO"};
  ecParameter modStreamsDepth{
      "modStreamsDepth", "modStreamsDepth", "", 0, "", 0, 1, 0, 1, consts::MOD};

  ecParameter grainDurationMs{
      "GrainDurms", "5. Grain Dur (ms)", "",   30, "", 0.01,
      1000,         0.0000001,           10000};
  al::ParameterMenu grainDurationLFO{"##grainDurationLFO"};
  ecParameter modGrainDurationDepth{"modGrainDurationDepth",
                                    "modGrainDurationDepth",
                                    "",
                                    0,
                                    "",
                                    0,
                                    1,
                                    0,
                                    1,
                                    consts::MOD};

  ecParameter envelope{
      "Envelopeshape", "6. Envelope Shape", "", 0.5, "", 0, 1, 0, 1};
  al::ParameterMenu envelopeLFO{"##envelopeLFO"};
  ecParameter modEnvelopeDepth{
      "modEnvelopeDepth", "modEnvelopeDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter transposition{
      "Pitchshift", "7. Pitch Shift", "", 1, "", -2, 2, -20, 20};
  al::ParameterMenu transpositionLFO{"##transpositionLFO"};
  ecParameter modTranspositionDepth{"modTranspositionDepth",
                                    "modTranspositionDepth",
                                    "",
                                    0,
                                    "",
                                    0,
                                    1,
                                    0,
                                    1,
                                    consts::MOD};
  ecParameter filter{
      "Filterfreq", "8. Filter Center", "", 440, "", 60, 5000, 20, 24000};
  al::ParameterMenu filterLFO{"##filterLFO"};
  ecParameter modFilterDepth{
      "modFilterDepth", "modFilterDepth", "", 0, "", 0, 1, 0, 1, consts::MOD};

  ecParameter resonance{"Resonance", "9. Resonance", "", 0, "", 0, 1, 0, 1};
  al::ParameterMenu resonanceLFO{"##resonanceLFO"};
  ecParameter modResonanceDepth{
      "modResonanceDepth", "modResonanceDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter soundFile{"Soundfile",
                        "10. Sound File",
                        "",
                        1,
                        "",
                        1,
                        static_cast<float>(mClipNum),
                        1,
                        static_cast<float>(mClipNum),
                        consts::INT_PARAM};

  al::ParameterMenu soundFileLFO{"##soundFileLFO"};
  ecParameter modSoundFileDepth{
      "modSoundFileDepth", "modSoundFileDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter tapeHead{"Scan", "11. Scan Position", "", 0.5, "", 0, 1, 0, 1};
  al::ParameterMenu tapeHeadLFO{"##scanLFO"};
  ecParameter modTapeHeadDepth{
      "modScanDepth", "modScanDepth", "", 0, "", 0, 1, 0, 1, consts::MOD};

  ecParameter scanWidth{"scanWidth", "13. Scan Width", "", 1, "", 0, 1, 0, 1};
  al::ParameterMenu scanWidthLFO{"##scanWidthLFO"};
  ecParameter modScanWidthDepth{
      "modScanWidthDepth", "modScanWidthDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter scanSpeed{"ScanSpeed", "12. Scan Speed", "", 1, "", -2, 2, -4, 4};
  al::ParameterMenu scanSpeedLFO{"##scanSpeedLFO"};
  ecParameter modScanSpeedDepth{
      "modScanSpeedDepth", "modScanSpeedDepth", "", 0, "", 0, 1, 0, 1,
      consts::MOD};

  ecParameter pan{"Pan", "14. Pan", "", 0, "", -1, 1, -1, 1};
  al::ParameterMenu panLFO{"##panLFO"};
  ecParameter modPanDepth{"modPanDepth", "modPanDepth", "", 0, "", 0, 1, 0, 1,
                          consts::MOD};

  ecParameter volumeDB{
      "AmplitudedB", "15. Amplitude (dB)", "", -6, "", -60, 6, -180, 48};
  al::ParameterMenu volumeLFO{"##volumeLFO"};
  ecParameter modVolumeDepth{
      "modVolumeDepth", "modVolumeDepth", "", 0, "", 0, 1, 0, 1, consts::MOD};

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

  std::string getCurrentAudioFileName() {
    std::string filename = soundClipFileName
        [soundFile.getModParam(modSoundFileDepth.mParameter->get()) - 1];
    filename = filename.substr(filename.find_last_of("/") + 1);
    return filename;
  }

private:
  double mGlobalSamplingRate = consts::SAMPLE_RATE,
         mPrevSR = consts::SAMPLE_RATE;

  al::PolySynth grainSynth{};    /* Polyhpony and interface to audio
                                               callback */
  voiceScheduler grainScheduler; /* Schedule grains */
  std::vector<std::string> soundClipFileName;

  int controlRateCounter = 0;
  int mActiveVoices = 0;
  int mCounter = 0;
  float mAvgActiveVoices = 0;
  float mPeakCPU;
  float mAvgCPU;
  int *mPActiveVoices = nullptr;

  std::vector<std::shared_ptr<ecModulator>> Modulators;

  /***mScanner Tests***/
  util::line mScanner;
  float mCurrentIndex;
  float mLastScanPos = 0;
  int mPrevModClip;
  float prevTapeHeadVal, nowTapeHeadVal;
  float prev_scan_speed, scan_speed, prev_scan_width, scan_width;
};

#endif
