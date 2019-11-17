#ifndef EMISSIONCONTROL_H
#define EMISSIONCONTROL_H

/**** Emission Control LIB ****/
#include "const.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "Gamma/Oscillator.h"
#include "al/math/al_Random.hpp"
#include "al/scene/al_DynamicScene.hpp"
#include "al/ui/al_Parameter.hpp"

/**** CSTD LIB ****/
#include <string>

/**
 * Wrapper class of three envelopes:
 *  expo,
 *  reverese expo,
 *  and tukey.
 *
 * Allows for a single value [0,1] to interpolate between all three envelopes.
 */
class grainEnvelope {
 public:

  /**
   * @brief Generate envelope in real-time.
   * 
   * @return Amplitude value at a point in time.
   */
  float operator()();

  /**
   * @brief Set grainEnvelope parameters.
   *
   * @param[in] sets the duration of the envelope in SECONDS, usually equal to
   * duration of grain.
   * @param[in] FROM 0 to 1, where 0 to 0.5 interplates between expo and tukey
   * and 0.5 to 1 interpolated between tukey and reverse expo.
   */
  grainEnvelope(float duration = 1, float envelope = 0) {
    this->setDuration(duration);
    this->setEnvelope(envelope);
  }

  /**
   * @brief Set enevlope type.
   * 
   * @param[in] FROM 0 to 1, where 0 to 0.5 interplates between expo and tukey
   * and 0.5 to 1 interpolated between tukey and reverse expo.
   */
  void setEnvelope(float envelope);

  /**
   * @brief Set duration of envelope.
   * 
   * @param[in] Duration in seconds.
   */
  void setDuration(float duration);

  void set(float duration, float envelope) {
    this->setDuration(duration);
    this->setEnvelope(envelope);
  }

  /**
   * @brief Mark envelope as done.
   */
  bool done();

  /**
   * @brief Call to reset envelope parameters to original starting position.
   */
  void reset();

  float getEnvelope() const { return mEnvelope; }

  float getDuration() const { return mDuration; }

 private:
  util::expo mExpoEnv;
  util::tukey mTurkeyEnv;
  util::expo mRExpoEnv;
  float mEnvelope;  // assumes between 0 and 1
  float mDuration;  // in seconds
};

/**
 * Wrapper class containing all unit generators needed to modulate the
 * grain/voiceScheduler parameters/
 */
class ecModulator {
 public:
  /**
   * @brief Constructor for ecModulator.
   *
   * @param[in] An enum type defined in consts.h. 
   *            SINE, SQUARE, SAW, or NOISE.
   * @param[in] The frequency of the modulator.
   * @param[in] The width of the modulator.
   */
  ecModulator(consts::waveform modWaveform = consts::SINE, float frequency = 1,
              float width = 1)
      : mWidth(width) {
    this->setWaveform(modWaveform);
    mLFO.set(frequency, 0, width);
  }

  /**
   * @brief Processing done at the audio rate.
   * 
   * @return Modulation value at a point in time.
   */
  float operator()();

  /**
   * @brief Set the waveform of the modulator.
   * 
   * @param[in] An enum type defined in consts.h. 
   *            SINE, SQUARE, SAW, or NOISE.
   */ 
  void setWaveform(consts::waveform modWaveform);

  /**
   * @brief Set the frequency of the modulator.
   * 
   * @param[in] Frequency in Hz.
   */ 
  void setFrequency(float frequency);

  /**
   * @brief Set the width of the modulator.
   * 
   * @param[in] Frequency in Hz.
   */ 
  void setWidth(float width);

  /**
   * Set the phase of the modulator.
   * 
   * @param[in] The phase as a number between 0 and 1.
   */ 
  void setPhase(float phase);

  consts::waveform getWaveform() const { return mModWaveform; }

  float getFrequency() const { return mLFO.freq(); }

  float getWidth() const { return mWidth; }

  float getPhase() const { return mLFO.phase(); }

 private:
  gam::LFO<> mLFO{};
  al::rnd::Random<> rand;
  consts::waveform mModWaveform;
  float mWidth;
};

/**
 * Derived from Parameter class.
 * Allows for arbitrary bounds on the parameter space.
 * Allows for dynamically allocating an internal modulator,
 * as well as using an external modulation source (choose from four different
 * sources).
 */
class ecParameter {
public:

  /**
   * PUBLIC OBJECTS
   * 
   */
  ecModulator* mModulator = nullptr;  // This is for dynamically allocating a
                                      // parameter's own modulator.
  al::Parameter* mParameter = nullptr;
  al::Parameter* mLowRange = nullptr;  // Parameter designed to bound low mParameter.
  al::Parameter* mHighRange = nullptr;  // Parameter designed to bound high mParameter.

  /**
   * @brief ecParameter Constructor.
   *
   * @param[in] parameterName The name of the parameter
   * @param[in] defaultValue The initial value for the parameter
   * @param[in] Default minimum value for the parameter.
   * @param[in] Default maximum value for the parameter.
   * @param[in] Absolute minimum value for the parameter.
   * @param[in] Absolute maximum value for the parameter.
   * @param[in] Waveform used to modulate the parameters current value.
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling getParamMod().
   */
  ecParameter(std::string parameterName, float defaultValue = 0,
              float defaultMin = -99999.0, float defaultMax = 99999.0,
              float absMin = -1 * FLT_MAX, float absMax = FLT_MAX,
              consts::waveform modWaveform = consts::SINE,
              bool independentMod = 0);

  /**
   * @brief ecParameter Constructor.
   *
   * @param[in] parameterName The name of the parameter
   * @param[in] Group The group the parameter belongs to
   * @param[in] defaultValue The initial value for the parameter
   * @param[in] prefix An address prefix that is prepended to the parameter's OSC
   * address
   * @param[in] Default minimum value for the parameter.
   * @param[in] Default maximum value for the parameter.
   * @param[in] Absolute minimum value for the parameter.
   * @param[in] Absolute maximum value for the parameter.
   * @param[in] Waveform used to modulate the parameters current value.
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling getParamMod().
   * 
   */
  ecParameter(std::string parameterName, std::string Group,
              float defaultValue = 0, std::string prefix = "",
              float defaultMin = -99999.0, float defaultMax = 99999.0,
              float absMin = -1 * FLT_MAX, float absMax = FLT_MAX,
              consts::waveform modWaveform = consts::SINE,
              bool independentMod = 0);

  /**
   * @brief ecParameter destructor.
   */
  ~ecParameter();


  /**
   * @brief Function to set the waveform using an integer value rather than the
   * mModWaveform type. 
   * 
   * param[in] 0 = SINE | 1 = SQUARE | 2 = SAW  | 3 = NOISE
   *
   */
  void setWaveformIndex(int index);

  /**
   * @brief Decide if there will be a modulator contained within instance.
   * 
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling getParamMod().
   */
  void setIndependentMod(bool independentMod);

   /**
   * @brief Get current parameter value.
   * 
   * @return Parameter value.
   */
  float getParam() const { return mParameter->get(); }

  /**
   * @brief Function that returns the ecParameter value transformed by AN EXTERNAL
   * modulation source. (ie independence set to false) This function assumes
   * that there are four external modulation sources (see ecModulator). Runs at the Audio/Control
   * rate.
   *
   * param[in] The current value of the SINE modulator.
   * param[in] The current value of the SQUARE modulator.
   * param[in] The current value of the SAW modulator.
   * param[in] The current value of the NOISE modulator.
   * param[in] FROM 0 to 1; The width of the modulation source.
   * 
   * @return Modified version of the current parameter value.
   */
  float getModParam(float modSineValue, float modSquareValue, float modSawValue,
                    float modNoiseValue, float modWidth);

  /**
   * @brief Function that returns the ecParameter value transformed by its INTERNAL
   * modulation source. (ie independence set to true) Runs at Audio/Control
   * Rate.
   *
   * param[in] FROM 0 to 1; The width of the modulation source.
   * 
   * @return Modified version of the current parameter value.
   */
  float getModParam(float modWidth);


  /**
   * @brief Draw the parameter range slider.
   */
  void draw();

 private:
  consts::waveform mModWaveform;
  float mMax, mMin;
  bool mIndependentMod;
};

/**
 * Derived from ParameterInt class.
 * Allows for dynamically allocating an internal modulator,
 * as well as using an external modulation source (choose from four different
 * sources).
 */
class ecParameterInt {
public:

  /**
   * PUBLIC OBJECTS
   * 
   */
  al::ParameterInt* mParameterInt = nullptr; //Main Parameter.
  al::ParameterInt* mLowRange = nullptr;  // Parameter designed to bound low mParameter.
  al::ParameterInt* mHighRange = nullptr;  // Parameter designed to bound high mParameter.
  ecModulator* mModulator = nullptr;  // This is for dynamically allocating a
                                      // parameter's own modulator.
                                      
/**
   * @brief ecParameterInt Constructor.
   *
   * @param[in] parameterName The name of the parameter
   * @param[in] Group The group the parameter belongs to
   * @param[in] defaultValue The initial value for the parameter
   * @param[in] prefix An address prefix that is prepended to the parameter's OSC
   * address
   * @param[in] Default minimum value for the parameter. 
   * @param[in] Default maximum value for the parameter. 
   * @param[in] Absolute minimum value for the parameter.
   * @param[in] Absolute maximum value for the parameter.
   * @param[in] Waveform used to modulate the parameters current value.
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling getParamMod().
   * 
   */
  ecParameterInt(std::string parameterName, std::string Group,
              int defaultValue = 0, std::string prefix = "",
              int defaultMin = -99999, int defaultMax = 99999,
              int absMin = -1 * INT_MAX, int absMax = INT_MAX,
              consts::waveform modWaveform = consts::SINE,
              bool independentMod = 0);

  /**
   * @bried ecParameterInt destructor.
   */
  ~ecParameterInt();

  /**
   * @brief Function to set the waveform using an integer value rather than the
   * mModWaveform type. 
   * 
   * param[in] 0 = SINE | 1 = SQUARE | 2 = SAW  | 3 = NOISE
   *
   */
  void setWaveformIndex(int index);

  /**
   * @brief Decide if there will be a modulator contained within instance.
   * 
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling getParamMod().
   */
  void setIndependentMod(bool independentMod);

  /**
   * @brief Get current parameter value.
   * 
   * @return Parameter value.
   */
  float getParam() { return mParameterInt->get(); }

  /**
   * @brief Function that returns the ecParameterInt value transformed by AN EXTERNAL
   * modulation source. (ie independence set to false) This function assumes
   * that there are four external modulation sources (see ecModulator). Runs at the Audio/Control
   * rate.
   *
   * param[in] The current value of the SINE modulator.
   * param[in] The current value of the SQUARE modulator.
   * param[in] The current value of the SAW modulator.
   * param[in] The current value of the NOISE modulator.
   * param[in] FROM 0 to 1; The width of the modulation source.
   * 
   * @return Modified version of the current parameter value.
   */
  int getModParam(float modSineValue, float modSquareValue, float modSawValue,
                  float modNoiseValue, float modWidth);

  /**
   * @brief Function that returns the ecParameterInt value transformed by its INTERNAL
   * modulation source. (ie independence set to true) Runs at Audio/Control
   * Rate.
   *
   * param[in] FROM 0 to 1; The width of the modulation source.
   * 
   * @return Modified version of the current parameter value.
   */
  int getModParam(float modWidth);

  /**
   * @brief Draw the parameter range slider. ****TODO
   */
  void draw();

private:
  bool mIndependentMod;
  int mMax, mMin;
  consts::waveform mModWaveform;
};

struct grainParameters {
  ecParameter& grainDurationMs;
  float modGrainDurationWidth;
  ecParameter& envelope;
  float modEnvelopeWidth;
  ecParameter& tapeHead;
  float modTapeHeadWidth;
  ecParameter& playbackRate;
  float modPlaybackRateWidth;
  util::buffer<float>* source;
  float modSineVal;
  float modSquareVal;
  float modSawVal;
  float modNoiseVal;
  int* activeVoices;
};

/**
 * Grain class containing an audio buffer and an envelope. Used as the voice for
 * the voiceScheduler class
 */
class Grain : public al::SynthVoice {
  public:

  /**
   * @brief Initialize voice. This function will only be called once per voice
   */
  virtual void init() override;

  /**
   * @brief Configure grain parameters before being sent to scheduler.
   * 
   * param[in] A struct containing a list of all grain parameters that need to
   * be set.
   *  - Note: see struct grainParameters above for details.
   */
  void configureGrain(grainParameters& list);

  /**
   * @brief Processing done at the audio rate.
   */
  virtual void onProcess(al::AudioIOData& io) override;

  /**
   * @brief This function will only be called once per trigger.
   */
  virtual void onTriggerOn() override;

  /**
   * @brief Sets duration of grain. 
   * 
   * @param[in] Set duration of grain in milliseconds.
   */
  void setDurationMs(float dur) { mDurationMs = dur; }

  float getDurationMs() const { return mDurationMs; }

 private:
  util::buffer<float>* source = nullptr;
  util::line index;
  grainEnvelope gEnv;
  int* mPActiveVoices;
  float envVal, sourceIndex, tapeHead, mDurationMs;
};

/**
 * Class used to schedule the emission of an arbitrary voice.
 */
class voiceScheduler {
 public:

  /**
   * @brief Constructor of the voice scheduler.
   *
   * @param[in] The audio samplingRate.
   */
  voiceScheduler(double samplingRate) { mSamplingRate = samplingRate; }

  /**
   * @brief Used to configure all parameters necessary for voiceScheduler.
   *
   * @param[in] The frequency in which voices are emitted.
   * @param[in] FROM 0 to 1; The value of the asynchronicity parameter.
   * @param[in] FROM 0 to 1; The value of the intermittence parameter.
   */
  void configure(double frequency, double async, double intermittence);

  /**
   * When return true, trigger a voice.
   * Run at the audio rate.
   */
  bool trigger();

  /**
   * Sets frequency in which voices are emitted.
   */
  void setFrequency(double frequency) {
    configure(frequency, mAsync, mIntermittence);
  }

  /**
   * Sets the asynchronicity parameter.
   * This randomly triggers a voice between its scheduled trigger point and a
   * percentage of one period of the scheduler.
   *
   * param[in] FROM 0 to 1. This determines the width of the random trigger.
   *  0 being completely synchronous and 1 being completely asynchronous.
   *
   * DOES NOT AFFECT DENSITY OF EMISSION.
   */
  void setAsynchronicity(double async) {
    configure(mFrequency, async, mIntermittence);
  }

  /**
   * Set the intermittence parameter.
   * This randomly drops a voice from being triggered.
   *
   * param[in] FROM 0 to 1. Probability of dropping voice is determined by this
   * value. 0 being a 0% chance of dropping the voice and 1 being a 100% chance
   * of dropping the voice.
   *
   * AFFECTS DENSITY OF EMISSION.
   */
  void setIntermittence(double intermittence) {
    configure(mFrequency, mAsync, intermittence);
  }

  /**
   * @brief Set the amount of voices running in parallel.
   *
   * IN PROGRESS
   */
  void setPolyStream(consts::streamType type, int numStreams);

 private:
  gam::LFO<> mPulse;
  al::rnd::Random<> rand;

  double mCounter{1.0};
  double mSamplingRate;
  double mAsync{0.0};
  double mFrequency{1.0};
  double mIncrement{0.0};
  double mIntermittence{0.0};
};

/** 
 * ****TODO : VERY MUCH A WIP
 * Inspired by David Thall's Adaptive Flow Control Algorithm:
 * 
 *
 * Class used to throttle grain rate and grain duration to avoid CPU spikes.
 *
 */
class flowControl {
  // friend class Granular;
 public:

  /**
   * This class will be calculate if it is necessary to reduce grain
   * rate/duration. Run at the audio rate (tentative).
   *
   * param[in] The time used to take CPU average usage and activeVoices average.
   * param[in] The ratio used to reduce grain duration and rate.
   * param[in] The current number of active voices.
   *
   * Return true if necessary to throttle.
   *
   */
  bool throttle(float time, float ratio, int activeVoices);

  float getPeakCPU() { return -11992.1; }

  float getAvgCPU() { return -11992.1; }

 private:
  int mCounter;
  float targetDuration;
  float targetRate;
  float mAvgActiveVoices;
  float mPeakCPU;
  float mAvgCPU;
};

#endif
