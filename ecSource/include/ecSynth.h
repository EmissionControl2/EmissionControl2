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

#include <array>
#include <memory>

using namespace consts;

class ecSynth : public al::SynthVoice {
 public:
  /**
   * Ringbuffers for oscilloscope
   */
  unsigned int oscBufferSize = 96001 * 3;  // max size of scope window plus a
                                           // few samples because I'm paranoid -
                                           // RD
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
  int clipL = 0, clipR = 0;

  /**
   * Peak values
   */
  float peakL = 0, peakR = 0;
  int peakResetL = 0, peakResetR = 0;

  /**
   * Grain Counter
   */
  int grainCounter = 0;

  // array of lfo parameters to draw later
  std::vector<std::shared_ptr<LFOstruct>> LFOParameters;

  std::vector<std::string> soundClipFileName;
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

    ECParameters[GRAIN_RATE] = std::make_shared<ecParameter>("GrainRate", "1. Grain Rate", 1, 0.1,
                                                             100, 0, 500, PARAM, false, "%.3f Hz");
    ECModParameters[GRAIN_RATE] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modGrainRate", "1. Grain Rate"});

    ECParameters[ASYNC] = std::unique_ptr<ecParameter>(
      new ecParameter{"Asynchronicity", "2. Asynchronicity", 0.0, 0.0, 1.0, 0, 1, PARAM});
    ECModParameters[ASYNC] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modAsynchronicity", "2. Asynchronicity"});

    ECParameters[INTERM] = std::unique_ptr<ecParameter>(
      new ecParameter{"Intermittency", "3. Intermittency", 0, 0, 1, 0, 1, PARAM});
    ECModParameters[INTERM] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modIntermittency", "3. Intermittency"});

    ECParameters[STREAMS] = std::unique_ptr<ecParameter>(
      new ecParameter{"Streams", "4. Streams", 1, 1, 12, 1, 20, INT_PARAM});
    ECModParameters[STREAMS] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modStreams", "4. Streams"});

    ECParameters[PLAYBACK] = std::unique_ptr<ecParameter>(
      new ecParameter{"PlaybackRate", "5. Playback Rate", 1, -2, 2, -32, 32, PARAM});
    ECModParameters[PLAYBACK] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modPlaybackRate", "5. Playback Rate"});

    ECParameters[FILTER_CENTER] = std::unique_ptr<ecParameter>(new ecParameter{
      "FilterCenter", "6. Filter Center", 440, 60, 5000, 20, 24000, PARAM, false, "%.3f Hz"});
    ECModParameters[FILTER_CENTER] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modFilterCenter", "6. Filter Center"});

    ECParameters[RESONANCE] = std::unique_ptr<ecParameter>(
      new ecParameter{"Resonance", "7. Resonance", 0, 0, 1, 0, 1, PARAM});
    ECModParameters[RESONANCE] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modResonance", "7. Resonance"});

    ECParameters[SOUND_FILE] = std::unique_ptr<ecParameter>(
      new ecParameter{"SoundFile", "8. Sound File", 1, 1, static_cast<float>(mClipNum), 1,
                      static_cast<float>(mClipNum), INT_PARAM});
    ECModParameters[SOUND_FILE] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modSoundFile", "8. Sound File"});

    ECParameters[SCAN_BEGIN] = std::unique_ptr<ecParameter>(
      new ecParameter{"ScanBegin", "9. Scan Begin", 0.0, 0, 1, 0, 1, PARAM});
    ECModParameters[SCAN_BEGIN] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modScanBegin", "9. Scan Begin"});

    ECParameters[SCAN_RANGE] = std::unique_ptr<ecParameter>(
      new ecParameter{"ScanRange", "10. Scan Range", 0.5, -1, 1, -1, 1, PARAM});
    ECModParameters[SCAN_RANGE] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modScanRange", "10. Scan Range"});

    ECParameters[SCAN_SPEED] = std::unique_ptr<ecParameter>(
      new ecParameter{"ScanSpeed", "11. Scan Speed", 1, -2, 2, -32, 32, PARAM});
    ECModParameters[SCAN_SPEED] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modScanSpeed", "11. Scan Speed"});

    ECParameters[GRAIN_DUR] = std::unique_ptr<ecParameter>(new ecParameter{
      "GrainDuration", "12. Grain Duration", 30, 0.046, 1000, 0.046, 10000, PARAM, false, "%.3f ms"});
    ECModParameters[GRAIN_DUR] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modGrainDuration", "12. Grain Duration"});

    ECParameters[ENVELOPE] = std::unique_ptr<ecParameter>(
      new ecParameter{"EnvelopeShape", "13. Envelope Shape", 0.5, 0, 1, 0, 1, PARAM});
    ECModParameters[ENVELOPE] = std::unique_ptr<ecModParameter>(
      new ecModParameter{"modEnvelopeShape", "13. Envelope Shape"});

    ECParameters[PAN] =
      std::unique_ptr<ecParameter>(new ecParameter{"Pan", "14. Pan", 0, -1, 1, -1, 1, PARAM});
    ECModParameters[PAN] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modPan", "14. Pan"});

    ECParameters[AMPLITUDE] = std::unique_ptr<ecParameter>(new ecParameter{
      "Amplitude", "15. Amplitude", -6, -60, 24, -180, 48, PARAM, false, "%.3f dB"});
    ECModParameters[AMPLITUDE] =
      std::unique_ptr<ecModParameter>(new ecModParameter{"modAmplitude", "15. Amplitude"});
  }

  void setGlobalSamplingRate(float sampling_rate) { mGlobalSamplingRate = sampling_rate; }
  double getGlobalSamplingRate() { return mGlobalSamplingRate; }

  void setGlobalBufferSize(float buffer_size) { mGlobalBufferSize = buffer_size; }
  double getGlobalBufferSize() { return mGlobalBufferSize; }

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
  bool loadSoundFileOffline(std::string fileName);

  /**
   * @brief Load sound file into memory.
   *
   * @param[in] The the filepath to the audio file.
   */
  bool loadSoundFileRT(std::string fileName);

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

  /**
   * @brief Simple hardclipping to prevent distorting the DAC.
   *        Applied to the left and right channels.
   *        !Run in the process block!
   */
  void hardClip(al::AudioIOData &io);

  int getNumberOfAudioFiles() const { return soundClip.size(); }

  std::string getCurrentAudioFileName() {
    float sf_mod_val = ECParameters[consts::SOUND_FILE]->getModParam(
      ECModParameters[consts::SOUND_FILE]->getWidthParam());
    sf_mod_val = sf_mod_val - (int)sf_mod_val > 0.5 ? ceil(sf_mod_val) : floor(sf_mod_val);
    std::string filename = soundClipFileName[sf_mod_val - 1];
#ifdef _WIN32
    filename = filename.substr(filename.find_last_of("\\") + 1);
#else
    filename = filename.substr(filename.find_last_of("/") + 1);
#endif
    return filename;
  }

  float getCurrentIndex() const { return mCurrentIndex; }

  int getLeadChannel() const { return AudioChanIndex[0]; }

  void setOutChannels(int lead_channel, int max_possible_channels) {
    AudioChanIndex[0] = lead_channel;
    if (max_possible_channels == 1) {
      for (int i = 1; i < consts::MAX_AUDIO_OUTS; i++) {
        AudioChanIndex[i] = lead_channel;
      }
    } else {
      // assert(lead_channel + (consts::MAX_AUDIO_OUTS) < max_possible_channels);
      for (int i = 1; i < consts::MAX_AUDIO_OUTS; i++) {
        AudioChanIndex[i] = lead_channel + i;
      }
    }
  }

  void setHardScanBegin(bool hard) { isHardScanBegin = hard; }

 private:
  double mGlobalSamplingRate = consts::SAMPLE_RATE, mPrevSR = consts::SAMPLE_RATE;
  double mGlobalBufferSize = consts::BUFFER_SIZE, mPrevBS = consts::BUFFER_SIZE;

  al::PolySynth grainSynth{};    /* Polyhpony and interface to audio
                                               callback */
  voiceScheduler grainScheduler; /* Schedule grains */

  int controlRateCounter = 0;
  int mActiveVoices = 0;
  int *mPActiveVoices = nullptr;
  gam::Domain ControlRate;
  std::vector<std::shared_ptr<ecModulator>> Modulators;
  std::array<unsigned int, consts::MAX_AUDIO_OUTS> AudioChanIndex;

  /**Accesing Audio Thread -- Scary :o **/
  std::mutex mVoicePassLock;

  /***localAudioThread variables***/
  float width;
  float sample_0, sample_1;

  /***mScanner***/
  util::line<double> mScanner;
  float mCurrentIndex;
  int mPrevModClip;
  float prevScanBeginVal, nowScanBeginVal;
  float prev_scan_speed, scan_speed, prev_scan_width, scan_width;
  bool pleaseResetScanner = false, isHardScanBegin = false;
};

#endif
