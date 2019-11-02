#ifndef EMISSIONCONTROL_H
#define EMISSIONCONTROL_H

#include "../external/allolib/external/Gamma/Gamma/Oscillator.h"
#include "Gamma/Oscillator.h"
//#include "al/util/imgui/al_Imgui.hpp"
#include <iostream>
#include <string>

#include "al/io/al_Imgui.hpp"
#include "al/math/al_Random.hpp"
#include "al/scene/al_DynamicScene.hpp"
#include "al/ui/al_Parameter.hpp"

#include "const.h"
#include "utility.h"

using namespace al;

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
   * Generate envelope in real-time.
   */
  float operator()();

  /**
   * Set grainEnvelope parameters.
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

  void set(float duration, float envelope) {
    this->setDuration(duration);
    this->setEnvelope(envelope);
  }

  void setEnvelope(float envelope) {
    if (envelope > 1)
      mEnvelope = 1;
    else if (envelope < 0)
      mEnvelope = 0;
    else
      mEnvelope = envelope;
  }

  //  in seconds
  void setDuration(float duration) {
    if (duration <= 0) {
      mDuration = 1;
    } else
      mDuration = duration;
    mTurkeyEnv.set(duration);
    mExpoEnv.set(duration, 0);
    mRExpoEnv.set(duration, 1);
  }

  /**
   * Mark envelope as done.
   */
  bool done() { return mTurkeyEnv.done(); }

  /**
   * Call to reset envelope parameters to original starting position.
   */
  void reset() {
    mExpoEnv.set();
    mRExpoEnv.set();
    mTurkeyEnv.set();
  }

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
   * Constructor for ecModulator.
   *
   * param[in] An enum type denoting the modulator source.
   * param[in] The frequency of the modulator.
   * param[in] The width of the modulator.
   */
  ecModulator(consts::waveform modWaveform = consts::SINE, float frequency = 1,
              float width = 1)
      : mFrequency(frequency), mWidth(width) {
    std::cout << "ecModulator Constructor\n";
    this->setWaveform(modWaveform);
    mLFO.set(frequency, 0, width);
  }

  /**
   * Processing done at the audio rate.
   */
  float operator()() {
    if (mModWaveform == consts::SINE) {
      return mLFO.cos();
    } else if (mModWaveform == consts::SAW) {
      return mLFO.up2();
    } else if (mModWaveform == consts::SQUARE) {
      return mLFO.stair();
    } else if (mModWaveform == consts::NOISE) {
      return rand.uniform(-1.0, 1.0);
    } else {
      return mLFO.cos();
    }
  }

  consts::waveform getWaveform() { return mModWaveform; }
  float getFrequency() { return mFrequency; }
  float getWidth() { return mWidth; }

  void setWaveform(consts::waveform modWaveform) {
    if (modWaveform != consts::SINE && modWaveform != consts::SAW &&
        modWaveform != consts::SQUARE && modWaveform != consts::NOISE) {
      std::cerr << "invalid waveform" << std::endl;
      return;
    }
    mModWaveform = modWaveform;
  }

  void setFrequency(float frequency) {
    mFrequency = frequency;
    mLFO.freq(frequency);
  }

  void setWidth(float width) {
    mWidth = width;
    mLFO.mod(width);
  }

  void setPhase(float phase) { mLFO.phase(phase); }

 private:
  gam::LFO<> mLFO{};
  al::rnd::Random<> rand;
  consts::waveform mModWaveform;
  float mFrequency;
  float mWidth;
};

/**
 * Derived from Parameter class.
 * Allows for dynamically allocating an internal modulator,
 * as well as using an external modulation source (choose from four different
 * sources).
 */
class ecParameter {
 public:
  ecParameter(std::string parameterName, float defaultValue = 0,
              float min = -99999.0, float max = 99999.0,
              float absMin = -1 * FLT_MAX, float absMax = FLT_MAX,
              consts::waveform modWaveform = consts::SINE,
              bool independent = 0) {
    mParameter = new Parameter{parameterName, defaultValue, min, max};
    mLowRange =
        new Parameter{(parameterName + "Low").c_str(), min, absMin, absMax};
    mHighRange =
        new Parameter{(parameterName + "High").c_str(), max, absMin, absMax};
    mMin = min;
    mMax = max;
    mModWaveform = modWaveform;
    mIndependent = independent;
    if (mIndependent)  // if true, this parameter will have its own modulator
      mModulator = new ecModulator{mModWaveform, 1, 1};
  }

  ecParameter(std::string parameterName, std::string Group,
              float defaultValue = 0, std::string prefix = "",
              float min = -99999.0, float max = 99999.0,
              float absMin = -1 * FLT_MAX, float absMax = FLT_MAX,
              consts::waveform modWaveform = consts::SINE,
              bool independent = 0) {
    mParameter =
        new Parameter{parameterName, Group, defaultValue, prefix, min, max};
    mLowRange = new Parameter{
        (parameterName + "Low").c_str(), Group, min, prefix, absMin, absMax};
    mHighRange = new Parameter{
        (parameterName + "High").c_str(), Group, max, prefix, absMin, absMax};
    mMin = min;
    mMax = max;
    mModWaveform = modWaveform;
    mIndependent = independent;
    if (mIndependent)  // if true, this parameter will have its own modulator
      mModulator = new ecModulator{mModWaveform, 1, 1};
  }
  ~ecParameter() {
    delete mParameter;
    if (mIndependent) delete mModulator;
  }

  void setIndependence(bool independent) {
    mIndependent = independent;
    if (mIndependent && mModulator == nullptr)
      mModulator = new ecModulator{mModWaveform, 1, 1};
    else
      delete mModulator;
  }

  // NEXT UP, draw dragFloats here, but using the members of type parameter
  // instead.
  void draw() {
    float valueSlider, valueLow, valueHigh;
    bool changed;
    // ImGuiCol_ x = ImGuiCol_TextDisabled;
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
    // ImGui::PushStyleColor(ImGuiCol_Text,149);;
    valueLow = mLowRange->get();
    changed = ImGui::DragFloat((mLowRange->displayName()).c_str(), &valueLow,
                               0.1, mLowRange->min(), mLowRange->max());
    ImGui::SameLine();
    if (changed) mLowRange->set(valueLow);
    // if(valueLow > mHighRange->get()) mParameter->min(mMin);
    mParameter->min(valueLow);

    ImGui::PopItemWidth();
    // ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
    valueSlider = mParameter->get();
    changed =
        ImGui::SliderFloat((mParameter->displayName()).c_str(), &valueSlider,
                           mParameter->min(), mParameter->max());
    if (changed) mParameter->set(valueSlider);
    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
    valueHigh = mHighRange->get();
    changed = ImGui::DragFloat((mHighRange->displayName()).c_str(), &valueHigh,
                               0.1, mHighRange->min(), mHighRange->max());
    if (changed) mHighRange->set(valueHigh);
    mParameter->max(valueHigh);

    ImGui::PopItemWidth();

    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
    ImGui::Text((mParameter->getName()).c_str());
    ImGui::PopItemWidth();
  }

  /**
   * Function to set the waveform using an integer value rather than the
   * mModWaveform type. param[in] 0 = SINE 1 = SQUARE 2 = SAW 3 = NOISE
   *
   */
  void setWaveformIndex(int index) {
    switch (index) {
      case 0:
        mModWaveform = consts::SINE;
        break;
      case 1:
        mModWaveform = consts::SQUARE;
        break;
      case 2:
        mModWaveform = consts::SAW;
        break;
      case 3:
        mModWaveform = consts::NOISE;
        break;
      default:
        mModWaveform = consts::SINE;
    }
  }

  float getParam() { return mParameter->get(); }

  /**
   * Function that returns the ecParameter value transformed by AN EXTERNAL
   * modulation source. (ie independence set to false) This function assumes
   * that there are four external modulation sources. Runs at the Audio/Control
   * rate.
   *
   * param[in] The current value of the SINE modulator.
   * param[in] The current value of the SQUARE modulator.
   * param[in] The current value of the SAW modulator.
   * param[in] The current value of the NOISE modulator.
   * param[in] FROM 0 to 1; The width of the modulation source.
   */
  int counter = 0;
  float getModParam(float modSineValue, float modSquareValue, float modSawValue,
                    float modNoiseValue, float modWidth) {
    switch (mModWaveform) {
      case consts::SINE:
        return mParameter->get() * ((modSineValue * modWidth) + 1);
      case consts::SQUARE:
        return mParameter->get() * ((modSquareValue * modWidth) + 1);
      case consts::SAW:
        return mParameter->get() * ((modSawValue * modWidth) + 1);
      case consts::NOISE:
        return mParameter->get() * ((modNoiseValue * modWidth) + 1);
      default:
        return mParameter->get() * ((modSineValue * modWidth) + 1);
    }
  }

  /**
   * Function that returns the ecParameter value transformed by its INTERNAL
   * modulation source. (ie independence set to true) Runs at Audio/Control
   * Rate.
   *
   * param[in] FROM 0 to 1; The width of the modulation source.
   */
  float getModParam(float modWidth) {
    if (!mIndependent) {
      std::cerr << "PARAMETER must have independence set to true if you want "
                   "to use this getModParam function\n";
      return -9999999999;
    }
    return mParameter->get() * (((*mModulator)() * modWidth) + 1);
  }

  consts::waveform mModWaveform;
  ecModulator* mModulator = nullptr;  // This is for dynamically allocating a
                                      // parameter's own modulator.
  Parameter* mParameter = nullptr;
  Parameter* mLowRange =
      nullptr;  // Parameter designed to bound low mParameter.
  Parameter* mHighRange =
      nullptr;  // Parameter designed to bound high mParameter.

 private:
  float mMax, mMin;
  bool mIndependent;
};

/**
 * Derived from ParameterInt class.
 * Allows for dynamically allocating an internal modulator,
 * as well as using an external modulation source (choose from four different
 * sources).
 */
class ecParameterInt {
 public:
  ecParameterInt(std::string parameterName, std::string Group,
                 int defaultValue = 0, std::string prefix = "", int min = 0,
                 int max = 127, consts::waveform modWaveform = consts::SINE,
                 bool independent = 0) {
    mParameterInt =
        new ParameterInt{parameterName, Group, defaultValue, prefix, min, max};
    mModWaveform = modWaveform;
    mIndependent = independent;
    if (mIndependent)  // if true, this parameter will have its own modulator
      mModulator = new ecModulator{mModWaveform, 1, 1};
  }

  ~ecParameterInt() {
    delete mParameterInt;
    if (mIndependent) delete mModulator;
  }

  void setIndependence(bool independent) {
    mIndependent = independent;
    if (mIndependent && mModulator == nullptr)
      mModulator = new ecModulator{mModWaveform, 1, 1};
    else
      delete mModulator;
  }

  /**
   * Function to set the waveform using an integer value rather than the
   * mModWaveform type. param[in] 0 = SINE 1 = SQUARE 2 = SAW 3 = NOISE
   *
   */
  void setWaveformIndex(int index) {
    switch (index) {
      case 0:
        mModWaveform = consts::SINE;
        break;
      case 1:
        mModWaveform = consts::SQUARE;
        break;
      case 2:
        mModWaveform = consts::SAW;
        break;
      case 3:
        mModWaveform = consts::NOISE;
        break;
      default:
        mModWaveform = consts::SINE;
    }
  }

  float getParam() { return mParameterInt->get(); }

  /**
   * Function that returns the ecParameterInt value transformed by AN EXTERNAL
   * modulation source. (ie independence set to false) This function assumes
   * that there are four external modulation sources. Runs at the Audio/Control
   * rate.
   *
   * param[in] The current value of the SINE modulator.
   * param[in] The current value of the SQUARE modulator.
   * param[in] The current value of the SAW modulator.
   * param[in] The current value of the NOISE modulator.
   * param[in] FROM 0 to 1; The width of the modulation source.
   */
  int getModParam(float modSineValue, float modSquareValue, float modSawValue,
                  float modNoiseValue, float modWidth) {
    switch (mModWaveform) {
      case consts::SINE:
        return mParameterInt->get() * ((modSineValue * modWidth) + 1);
      case consts::SQUARE:
        return mParameterInt->get() * ((modSquareValue * modWidth) + 1);
      case consts::SAW:
        return mParameterInt->get() * ((modSawValue * modWidth) + 1);
      case consts::NOISE:
        return mParameterInt->get() * ((modNoiseValue * modWidth) + 1);
      default:
        return mParameterInt->get() * ((modSineValue * modWidth) + 1);
    }
  }

  /**
   * Function that returns the ecParameterInt value transformed by its INTERNAL
   * modulation source. (ie independence set to true) Runs at Audio/Control
   * Rate.
   *
   * param[in] FROM 0 to 1; The width of the modulation source.
   */
  int getModParam(float modWidth) {
    if (!mIndependent) {
      std::cerr << "PARAMETER must have independence set to true if you want "
                   "to use this getModParam function\n";
      return -99999;
    }
    return mParameterInt->get() * (((*mModulator)() * modWidth) + 1);
  }

  consts::waveform mModWaveform;
  ParameterInt* mParameterInt = nullptr;
  ecModulator* mModulator = nullptr;  // This is for dynamically allocating a
                                      // parameter's own modulator.
  bool mIndependent;
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
  util::Buffer<float>* source;
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
  // Grain();
  int counter = 0;  // USED for debugging

  // Initialize voice. This function will nly be called once per voice
  virtual void init() override {}

  /**
   * Processing done at the audio rate.
   */
  virtual void onProcess(al::AudioIOData& io) override {
    while (io()) {
      envVal = gEnv();
      sourceIndex = index();
      if (sourceIndex > source->size) sourceIndex -= source->size;
      io.out(0) += source->get(sourceIndex) * envVal;
      io.out(1) += source->get(sourceIndex) * envVal;
      if (gEnv.done()) {
        *mPActiveVoices -= 1;  // This will remove a grain from the active list
                               // in the Granulator.
        free();
        counter = 0;
        break;
      }
    }
  }

  virtual void onTriggerOn() override { gEnv.reset(); }

  /**
   * Configure grain parameters before being sent to scheduler.
   * param[in] A struct containing a list of all grain parameters that need to
   * be set.
   *  - Note: see struct grainParameters for details.
   */

  void configureGrain(grainParameters& list) {
    float startSample, endSample;

    mPActiveVoices = list.activeVoices;

    if (list.modGrainDurationWidth > 0)
      setDurationMs(list.grainDurationMs.getModParam(
          list.modSineVal, list.modSquareVal, list.modSawVal, list.modNoiseVal,
          list.modGrainDurationWidth));
    else
      setDurationMs(list.grainDurationMs.getParam());

    if (list.modEnvelopeWidth > 0)
      gEnv.set(mDurationMs / 1000,
               list.envelope.getModParam(list.modSineVal, list.modSquareVal,
                                         list.modSawVal, list.modNoiseVal,
                                         list.modEnvelopeWidth));
    else
      gEnv.set(mDurationMs / 1000, list.envelope.getParam());

    this->source = list.source;

    if (list.modTapeHeadWidth > 0)
      startSample = list.source->size *
                    (list.tapeHead.getModParam(
                        list.modSineVal, list.modSquareVal, list.modSawVal,
                        list.modNoiseVal, list.modTapeHeadWidth));
    else
      startSample = list.source->size * list.tapeHead.getParam();

    if (list.modPlaybackRateWidth > 0)
      endSample = startSample +
                  (mDurationMs / 1000) * consts::SAMPLE_RATE *
                      abs(list.playbackRate.getModParam(
                          list.modSineVal, list.modSquareVal, list.modSawVal,
                          list.modNoiseVal, list.modPlaybackRateWidth));
    else
      endSample = startSample + (mDurationMs / 1000) * consts::SAMPLE_RATE *
                                    abs(list.playbackRate.getParam());
    if (list.playbackRate.getParam() < 0)
      index.set(endSample, startSample, mDurationMs / 1000);
    else
      index.set(startSample, endSample, mDurationMs / 1000);
  }

  float getDurationMs() const { return mDurationMs; }

  void setDurationMs(float dur) { mDurationMs = dur; }

 private:
  util::Buffer<float>* source = nullptr;
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
   * Constructor of the voice scheduler.
   *
   * param[in] The audio samplingRate.
   */
  voiceScheduler(double samplingRate) { mSamplingRate = samplingRate; }

  /**
   * Used to configure all parameters necessary for voiceScheduler.
   *
   * param[in] The frequency in which voices are emitted.
   * param[in] FROM 0 to 1; The value of the asynchronicity parameter.
   * param[in] FROM 0 to 1; The value of the intermittence parameter.
   */
  void configure(double frequency, double async, double intermittence) {
    if (async > 1.0) {
      async = 1.0;
    } else if (async < 0.0) {
      async = 0.0;
    }
    if (intermittence > 1.0) {
      intermittence = 1.0;
    } else if (intermittence < 0.0) {
      intermittence = 0.0;
    }
    mAsync = async;
    mFrequency = frequency;
    mIntermittence = intermittence;
    mIncrement = mFrequency / mSamplingRate;
  }

  /**
   * When return true, trigger a voice.
   * Run at the audio rate.
   */
  bool trigger() {
    if (mCounter >= 1.0) {
      // std::cout << mIncrement * mSamplingRate << std::endl;
      mCounter -= 1.0;
      if (rand.uniform() < mIntermittence) return false;
      mCounter += rand.uniform(-mAsync, mAsync);
      mCounter += mIncrement;
      return true;
    }
    mCounter += mIncrement;
    return false;
  }

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
   * Set the amount of voices running in parallel.
   *
   * IN PROGRESS
   */
  void setPolyStream(consts::streamType type, int numStreams) {
    if (type == consts::synchronous) {
      // std::cout << numStreams << std::endl;
      setFrequency(static_cast<double>(mFrequency * numStreams));
    } else {
      std::cerr << "Not implemented yet, please try again later.\n";
    }
  }

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
 * Inspired by David Thall's Adaptive Flow Control Algorithm:
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
  bool throttle(float time, float ratio, int activeVoices) {
    if (mCounter < time * consts::SAMPLE_RATE) {
      mCounter++;
      mAvgActiveVoices += activeVoices;
      return false;
    } else {
      mCounter++;
      mAvgActiveVoices /= mCounter;
      mCounter = 0;
    }

    float adaptThresh;

    if (getPeakCPU() > adaptThresh) {
      return true;
    }
    if (getAvgCPU() > adaptThresh) {
      return true;
    } else {
      return false;
    }
  }

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
