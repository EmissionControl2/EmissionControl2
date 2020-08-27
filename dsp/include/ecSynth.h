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
#include <array>
#include <math.h>
#include <memory>

using namespace consts;

class ecSynth : public al::SynthVoice {
public:
  /**
   * Ringbuffers for oscilloscope
   */
  unsigned int oscBufferSize = 96001 * 3; // max size of scope window plus a
                                          // few samples because I'm paranoid -
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
  int clipL, clipR = 0;

  /**
   * Peak values
   */
  float peakL, peakR = 0;
  int peakResetL, peakResetR = 0;

  /**
   * Grain Counter
   */
  int grainCounter = 0;
  /**
   * The number of modulators in ecSynth.
   */
  const static int NUM_MODULATORS = 4;

  // array of lfo parameters to draw later
  std::vector<std::shared_ptr<LFOstruct>> LFOParameters;

  std::vector<std::shared_ptr<util::buffer<float>>> soundClip; /* Store audio buffers in memory */
  int mClipNum = 0; /* Number of sound files being stored in memory */
  int mModClip = 0;

  std::array<std::shared_ptr<ecParameter>, consts::NUM_PARAMS> ECParameters;
  std::array<std::unique_ptr<ecModParameter>, consts::NUM_PARAMS> ECModParameters;

  /**
   * PUBLIC PARAMETERS OF SYNTH
   */

  ecSynth(){};

  void initParameters() {
    using namespace consts;

    ECParameters[GRAIN_RATE] = std::make_shared<ecParameter>("grainRate", "1. Grain Rate", 1, 0.1,
                                                             100, 0, 500, PARAM, "%.3f Hz");

    ECModParameters[GRAIN_RATE] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modGrainRateDepth", "Grain Rate"});

    ECParameters[ASYNC] = std::unique_ptr<ecParameter>(
        new ecParameter{"Asynchronicity", "2. Asynchronicity", 0.0, 0.0, 1.0, 0, 1, PARAM});
    ECModParameters[ASYNC] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modAsynchronicityDepth", "Asynchronicity"});

    ECParameters[INTERM] = std::unique_ptr<ecParameter>(
        new ecParameter{"Intermittancy", "3. Intermittancy", 0, 0, 1, 0, 1, PARAM});
    ECModParameters[INTERM] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modIntermittencyDepth", "Intermittency"});

    ECParameters[STREAMS] = std::unique_ptr<ecParameter>(
        new ecParameter{"Streams", "4. Streams", 1, 1, 12, 1, 20, INT_PARAM});
    ECModParameters[STREAMS] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modStreamsDepth", "Streams"});

    ECParameters[GRAIN_DUR] = std::unique_ptr<ecParameter>(new ecParameter{
        "GrainDurms", "5. Grain Duration", 30, 0.01, 1000, 0.0000001, 10000, PARAM, "%.3f ms"});
    ECModParameters[GRAIN_DUR] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modGrainDurationDepth", "Grain Duration"});

    ECParameters[ENVELOPE] = std::unique_ptr<ecParameter>(
        new ecParameter{"Envelopeshape", "6. Envelope Shape", 0.5, 0, 1, 0, 1, PARAM});
    ECModParameters[ENVELOPE] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modEnvelopeShapeDepth", "Envelope Shape"});

    ECParameters[PITCH_SHIFT] = std::unique_ptr<ecParameter>(
        new ecParameter{"Pitchshift", "7. Pitch Shift", 1, -2, 2, -20, 20, PARAM});
    ECModParameters[PITCH_SHIFT] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modPitchShiftDepth", "Pitch Shift"});

    ECParameters[FILTER_CENTER] = std::unique_ptr<ecParameter>(new ecParameter{
        "Filterfreq", "8. Filter Center", 440, 60, 5000, 20, 24000, PARAM, "%.3f Hz"});
    ECModParameters[FILTER_CENTER] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modFilterCenterDepth", "Filter Center"});

    ECParameters[RESONANCE] = std::unique_ptr<ecParameter>(
        new ecParameter{"Resonance", "9. Resonance", 0, 0, 1, 0, 1, PARAM});
    ECModParameters[RESONANCE] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modResonanceDepth", "Resonance"});

    ECParameters[SOUND_FILE] = std::unique_ptr<ecParameter>(
        new ecParameter{"Soundfile", "10. Sound File", 1, 1, static_cast<float>(mClipNum), 1,
                        static_cast<float>(mClipNum), INT_PARAM});
    ECModParameters[SOUND_FILE] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modSoundFileDepth", "Sound File"});

    ECParameters[SCAN_POS] = std::unique_ptr<ecParameter>(
        new ecParameter{"scanPosition", "11. Scan Position", 0.0, 0, 1, 0, 1, PARAM});
    ECModParameters[SCAN_POS] = std::unique_ptr<ecModParameter>(
        new ecModParameter{"modScanPositionDepth", "Scan Position"});

    ECParameters[SCAN_WIDTH] = std::unique_ptr<ecParameter>(
        new ecParameter{"scanWidth", "12. Scan Width", 0.5, 0, 1, 0, 1, PARAM});
    ECModParameters[SCAN_WIDTH] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modScanWidthDepth", "Scan Width"});

    ECParameters[SCAN_SPEED] = std::unique_ptr<ecParameter>(
        new ecParameter{"ScanSpeed", "13. Scan Speed", 1, -2, 2, -32, 32, PARAM});
    ECModParameters[SCAN_SPEED] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modScanSpeedDepth", "Scan Speed"});

    ECParameters[PAN] =
        std::unique_ptr<ecParameter>(new ecParameter{"Pan", "14. Pan", 0, -1, 1, -1, 1, PARAM});
    ECModParameters[PAN] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modPanDepth", "Pan"});

    ECParameters[VOLUME] = std::unique_ptr<ecParameter>(
        new ecParameter{"AmplitudedB", "15. Amplitude", -6, -60, 6, -180, 48, PARAM, "%.3f dB"});
    ECModParameters[VOLUME] =
        std::unique_ptr<ecModParameter>(new ecModParameter{"modAmplitudeDepth", "Amplitude"});
  }

  /**
   * @brief Initilialize the synth.
   */
  void initialize(al::AudioIOData *io);
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
   * @brief Load sound file into memory, used only on init.
   *
   * @param[in] The the filepath to the audio file.
   */
  void loadSoundFileOffline(std::string fileName);

  /**
   * @brief Load sound file into memory.
   *
   * @param[in] The the filepath to the audio file.
   */
  void loadSoundFileRT(std::string fileName);

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
  int getNumActiveVoices() { return mActiveVoices; }

  /**
   * @brief Copy current indicies of active grains to array.
   *    Note: THREAD SAFE
   *
   * @param[out] array : Copy active grain indicies into this array, order
   *    doesn't matter.
   * @param[out] outSize : The actual number of grains stored starting from 0 to
   *    outSize - 1 where outSize <= maxSize.
   * @param[in] maxSize : The the maximum amount of active grains that can be
   *    stored.
   */
  void copyActiveGrainIndicies(float *array, int *outSize, int maxSize);

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
    std::string filename = soundClipFileName[mModClip];
    filename = filename.substr(filename.find_last_of("/") + 1);
    return filename;
  }

private:
  double mGlobalSamplingRate = consts::SAMPLE_RATE, mPrevSR = consts::SAMPLE_RATE;

  al::PolySynth grainSynth{};    /* Polyhpony and interface to audio
                                               callback */
  voiceScheduler grainScheduler; /* Schedule grains */
  std::vector<std::string> soundClipFileName;

  int controlRateCounter = 0;
  int mActiveVoices = 0;
  int mCounter = 0;
  float mAvgActiveVoices = 0;
  // float mPeakCPU;
  // float mAvgCPU;
  int *mPActiveVoices = nullptr;
  std::vector<std::shared_ptr<ecModulator>> Modulators;

  /**Accesing Audio Thread -- Scary :o **/
  std::mutex mVoicePassLock;

  /***localAudioThread variables***/
  float width;

  /***mScanner Tests***/
  util::line mScanner;
  float mCurrentIndex;
  int mPrevModClip;
  float prevTapeHeadVal, nowTapeHeadVal;
  float prev_scan_speed, scan_speed, prev_scan_width, scan_width;
};

#endif
