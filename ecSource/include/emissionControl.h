/**
 * emissionControl.h
 *
 * AUTHOR: Jack Kilgore
 */

#ifndef EMISSIONCONTROL_H
#define EMISSIONCONTROL_H

/**** Emission Control LIB ****/
#include "const.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "Gamma/Filter.h"
#include "Gamma/Oscillator.h"
#include "al/app/al_App.hpp"
#include "al/io/al_Imgui.hpp"
#include "al/math/al_Random.hpp"
#include "al/scene/al_DynamicScene.hpp"
#include "al/ui/al_Parameter.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;

/**** External LIB ****/
#include "imgui_internal.h"

/**** CSTD LIB ****/
#include <string>

/*** Keyboard Objects ***/
struct KeyDown {
  al::Keyboard key;
  bool readyToTrig = false;
  bool lastParamCheck = false;
};

/*** MIDI Objects ***/
struct MIDILearnBool {
  bool mParamAdd = false;
  bool mParamDel = false;
};

class MIDIKey {
 public:
  std::vector<al::MIDIMessage> mInfo;

  MIDIKey(){};
  MIDIKey(al::MIDIMessage m, int paramIndex, consts::MIDIType type) {
    mInfo.push_back(m);
    setKeysIndex(paramIndex, type);
  }
  // NOTE: the index is highly depenent on on the mType. USE with caution.
  int getKeysIndex() { return mKeysIndex; }
  consts::MIDIType getType() { return mType; }
  void setKeysIndex(int index, consts::MIDIType type) {
    mKeysIndex = index;
    mType = type;
  }

  void toJSON(json &j) {
    j["MIDI_TYPE"] = mType;
    j["MIDI_INDEX"] = mKeysIndex;

    json infoArray = json::array();
    for (int index = 0; index < mInfo.size(); index++) {
      json data;
      data["MIDI_DATA"] = mInfo[index].bytes;
      data["PORT"] = mInfo[index].port();
      infoArray.push_back(data);
    }
    j["MIDI_INFO"] = infoArray;
  }

  void fromJSON(const json &j) {
    mType = j.at("MIDI_TYPE");
    mKeysIndex = j.at("MIDI_INDEX");
    for (int index = 0; index < j.at("MIDI_INFO").size(); index++) {
      al::MIDIMessage temp(
        0, j.at("MIDI_INFO")[index].at("PORT"), j.at("MIDI_INFO")[index].at("MIDI_DATA")[0],
        j.at("MIDI_INFO")[index].at("MIDI_DATA")[1], j.at("MIDI_INFO")[index].at("MIDI_DATA")[2]);
      mInfo.push_back(temp);
    }
  }

 private:
  int mKeysIndex;
  consts::MIDIType mType;
};

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
  void setSamplingRate(float samplingRate);

  float getSamplingRate() { return mSamplingRate; }

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
  float mSamplingRate;
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
   *            SINE, SQUARE, ASCEND, DESCEND, or NOISE.
   * @param[in] The frequency of the modulator.
   * @param[in] The width of the modulator.
   */
  ecModulator(consts::waveform modWaveform = consts::SINE,
              consts::polarity modPolarity = consts::BI, float frequency = 1, float width = 1) {
    this->setWaveform(modWaveform);
    this->setPolarity(modPolarity);
    mLFO.set(frequency, 0, width);
  }

  /**
   * @brief Processing done at the audio rate.
   *
   * @return Modulation value at a point in time.
   */
  float operator()();

  /**
   * @brief Calls the operator() function and then calls
   * setCurrentSample(float). For convenience.
   */
  void sampleAndStore() { setCurrentSample((*this)()); }

  /**
   * @brief If you are using these as global modulators it might be useful to
   * store the current sample.
   *
   * @param sample : The current sample to be stored, usually comes from
   * this->operator()
   */
  void setCurrentSample(float sample) { mCurrentSample = sample; }

  /**
   * @brief If you are using these as global modulators it might be useful to
   * get the current sample.
   *
   * @return The current sample being stored.
   */
  float getCurrentSample() const { return mCurrentSample; }

  /**
   * @brief Set the waveform of the modulator.
   *
   * @param[in] An enum type defined in consts.h.
   *            SINE, SQUARE, ASCEND, DESCEND, or NOISE.
   *
   */
  void setWaveform(consts::waveform modWaveform);

  /**
   * @brief Set the waveform using an integer.
   * @param[in] modwaveformIndex :
   *                           0 = SINE
   *                           1 = SQUARE
   *                           2 = ASCEND
   *                           3 = DESCEND
   *                           4 = NOISE
   *
   * @param[in] mod
   */
  void setWaveform(unsigned modwaveformIndex);

  /**
   * @brief Set the polarity of the modulator.
   *
   * @param[in] An enum type defined in consts.h.
   *            BI, UNI_POS, UNI_NEG.
   *
   */
  void setPolarity(consts::polarity modPolarity);

  /**
   * @brief Set the waveform using an integer.
   * @param[in] modwaveformIndex :
   *                           0 = BI
   *                           1 = UNI_POS
   *                           2 = UNI_NEG
   *
   * @param[in] Polarity index.
   */
  void setPolarity(unsigned modPolarityIndex);

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

  float getWidth() const { return mLFO.mod(); }

  float getPhase() const { return mLFO.phase(); }

  gam::LFO<> &getLFO() { return mLFO; }

 private:
  gam::LFO<> mLFO{};
  al::rnd::Random<> rand;
  consts::waveform mModWaveform;
  consts::polarity mPolarity;
  int mSign;
  float mFrequency, mCurrentSample, mHoldNoiseSample, mWidth = 0.5;
  unsigned int lastPhase, currentPhase;

  float sampleAndHoldUniform(float low, float high);
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
  ecModulator *mModulator = nullptr;  // This is for dynamically allocating a
                                      // parameter's own modulator.
  al::Parameter *mParameter = nullptr;
  al::Parameter *mLowRange = nullptr;   // Parameter designed to bound low mParameter.
  al::Parameter *mHighRange = nullptr;  // Parameter designed to bound high mParameter.

  // variables for OSC control
  std::string mOscArgument = "";
  ImGuiInputTextCallback inputTextCallback;
  void *CallbackUserData;
  float mOscMin = 0;
  float mOscMax = 0;
  bool mOscCustomRange = 0;

  /**
   * @brief ecParameter Constructor.
   * @param[in] parameterName The name of the parameter
   * @param[in] displayName The displayed name of the parameter
   * @param[in] defaultValue The initial value for the parameter
   * @param[in] Default minimum value for the parameter.
   * @param[in] Default maximum value for the parameter.
   * @param[in] Absolute minimum value for the parameter.
   * @param[in] Absolute maximum value for the parameter.
   * @param[in] slideType: Sets the type of slider drawn
   *    -- PARAM, PARAM_INT, LFO, LFO_INT, MOD, MOD_INT
   * @param[in] sliderText: Set the text to be displayed on the slider
   *    Example "%i Hz"
   *      This will display the current value of the slider with Hz next to it.
   * @param[in] independentMod:
   *            If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling
   *              getParamMod().
   */
  ecParameter(std::string parameterName, std::string displayName, float defaultValue = 0,
              float defaultMin = -99999.0, float defaultMax = 99999.0, float absMin = -1 * FLT_MAX,
              float absMax = FLT_MAX, consts::sliderType slideType = consts::PARAM,
              bool isLog = false, std::string sliderText = "", bool independentMod = 0);

  /**
   * @brief ecParameter Constructor.
   *
   * @param[in] parameterName The name of the parameter
   * @param[in] displayName The displayed name of the parameter
   * @param[in] Group The group the parameter belongs to
   * @param[in] defaultValue The initial value for the parameter
   * @param[in] prefix An address prefix that is prepended to the parameter's
   *            OSC address.
   * @param[in] Default minimum value for the parameter.
   * @param[in] Default maximum value for the parameter.
   * @param[in] Absolute minimum value for the parameter.
   * @param[in] Absolute maximum value for the parameter.
   * @param[in] Waveform used to modulate the parameters current value.
   * @param[in] slideType: Sets the type of slider drawn
   *    -- PARAM, PARAM_INT, LFO, LFO_INT, MOD, MOD_INT
   * @param[in] sliderText: Set the text to be displayed on the slider
   *    Example "%i Hz"
   *      This will display the current value of the slider with Hz next to it.
   * @param[in] independentMod:
   *            If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling
   *              getParamMod().
   */
  ecParameter(std::string parameterName, std::string displayName, std::string Group,
              float defaultValue = 0, float defaultMin = -99999.0, float defaultMax = 99999.0,
              float absMin = -1 * FLT_MAX, float absMax = FLT_MAX,
              consts::sliderType slideType = consts::PARAM, bool isLog = false,
              std::string sliderText = "", bool independentMod = 0);

  /**
   * @brief ecParameter destructor.
   */
  ~ecParameter();

  /**
   * @brief Set which external modulation source to use.
   *
   * @param modSource: Pointer to the external ecModulator for this parameter.
   */
  void setModulationSource(const std::shared_ptr<ecModulator> &modSource) {
    mModSource = modSource;
  }

  /**
   * @brief Decide if there will be a modulator contained within instance.
   *
   * @param[in] If set to true, the parameter will contain its own modulator.
   *            If false, must input data on an outside modulator when calling
   * getParamMod().
   */
  void setIndependentMod(bool independentMod);

  /**
   * @brief Set current parameter value.
   *
   * @param[in] New parameter value
   *
   */
  void setParam(float value) {
    if (value < mLowRange->get())
      mParameter->set(mLowRange->get());
    else if (value > mHighRange->get())
      mParameter->set(mHighRange->get());
    else
      mParameter->set(value);
  }

  /**
   * @brief Get current parameter value.
   *
   * @return Parameter value.
   */
  float getParam() const { return mParameter->get(); }

  /**
   * @brief Function that returns the ecParameter value transformed by its
   * INTERNAL modulation source. (ie independence set to true) Runs at
   * Audio/Control Rate.
   *
   * param[in] FROM 0 to 1; The width of the modulation source.
   *
   * @return Modified version of the current parameter value.
   */
  float getModParam(float modDepth);

  /**
   * @brief Set current min value of range slider.
   *
   */
  void setCurrentMin(float min) {
    mLowRange->set(min);
    if (mOscCustomRange == 0) mOscMin = min;
  }

  /**
   * @brief Set current max value of range slider.
   *
   */
  void setCurrentMax(float max) {
    mHighRange->set(max);
    if (mOscCustomRange == 0) mOscMax = max;
  }

  void setAbsoluteMinMax(float min, float max) {
    mLowRange->min(min);
    mHighRange->min(min);

    mLowRange->max(max);
    mHighRange->max(max);
  }

  float getAbsoluteMin() { return mLowRange->min(); }

  float getAbsoluteMax() { return mHighRange->max(); }

  /**
   * @brief Get current max value.
   *
   * @return Max value.
   */
  float getCurrentMin() const { return mLowRange->get(); }

  /**
   * @brief Get current max value.
   *
   * @return Max value.
   */
  float getCurrentMax() const { return mHighRange->get(); }

  /**
   * @brief Registers all parameters within ecParameter to a preset
   * handler.
   *
   * @param[in] presetHandler : A reference to a preset handler.
   */
  void addToPresetHandler(al::PresetHandler &presetHandler);
  void skipParamPresetHandler(al::PresetHandler &presetHandler, bool skip);

  void setLog(bool isLog) { mIsLog = isLog; }

  bool isLog() const { return mIsLog; }

  bool isIntVal() const { return mSliderType > 2; }

  /**
   * @brief Set the text to be displayed on the slider
   * Example "%i Hz"
   *    This will display the current value of the slider with Hz next to it.
   */
  void setSliderText(std::string sliderText) { mSliderText = sliderText; }

  /**
   * @brief Draw the parameter range slider.
   *
   * @param[out] isMIDILearn: Returns true if main slider needs to be learned,
   * false ow.
   */
  void drawRangeSlider(MIDILearnBool *isMIDILearn, KeyDown *k);

  std::string getDisplayName() const { return mDisplayName; }

  void setDisplayName(std::string name) { mDisplayName = name; }

  consts::sliderType getSliderType() const { return mSliderType; }

  void getSliderType(consts::sliderType s) { mSliderType = s; }

 private:
  std::string mDisplayName;
  std::string mSliderText;
  consts::sliderType mSliderType;
  std::shared_ptr<ecModulator> mModSource;
  bool mIndependentMod;
  bool mIsLog;
  // Draw flags
  bool editing = false;
};

struct ecModParameter {
  ecModParameter(std::string parameterName, std::string displayName)
    : param(parameterName, displayName, "", 0, 0, 1, 0, 1, consts::MOD, false),
      lfoMenu("##lfo" + parameterName) {
    std::string tempArgument = parameterName;
    tempArgument.insert(tempArgument.begin(), '/');
    mOscArgument = tempArgument;
  }

  // variables for OSC control
  std::string mOscArgument = "";
  ImGuiInputTextCallback inputTextCallback;
  void *CallbackUserData;
  float mOscMin = 0;
  float mOscMax = 1;
  bool mOscCustomRange = 0;

  void setMenuElements(std::vector<std::string> elements) { lfoMenu.setElements(elements); }

  float getWidthParam() { return param.getParam(); }

  void registerMenuChangeCallback(std::function<void(int)> cb) {
    lfoMenu.registerChangeCallback(cb);
  }

  void addToPresetHandler(al::PresetHandler &presetHandler) {
    param.addToPresetHandler(presetHandler);
    presetHandler.registerParameter(lfoMenu);
  }

  void skipParamPresetHandler(al::PresetHandler &presetHandler, bool skip) {
    param.skipParamPresetHandler(presetHandler, skip);
    presetHandler.skipParameter(lfoMenu.getFullAddress(), skip);
  }

  void drawModulationControl(MIDILearnBool *isMIDILearn, KeyDown *k) {
    ImGui::PushItemWidth(70 * ImGui::GetIO().FontGlobalScale);
    al::ParameterGUI::drawMenu(&lfoMenu);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    param.drawRangeSlider(isMIDILearn, k);
    if (mOscCustomRange == 0) {
      mOscMin = param.getCurrentMin();
      mOscMax = param.getCurrentMax();
    }
  }

  ecParameter param;
  al::ParameterMenu lfoMenu;
};

// a struct to wrap LFO parameters
class LFOstruct {
 public:
  al::ParameterMenu *shape = nullptr;
  al::ParameterMenu *polarity = nullptr;
  ecParameter *frequency = nullptr;
  al::Parameter *duty = nullptr;
  int mLFONumber;

  // variables for OSC control
  std::string mOscArgument = "";
  ImGuiInputTextCallback inputTextCallback;
  void *CallbackUserData;
  float mOscMin = 0.010;
  float mOscMax = 30.000;
  bool mOscCustomRange = 0;

  // constructor
  LFOstruct(int lfoNumber) {
    mLFONumber = lfoNumber;
    std::string menuName = "##LFOshape" + std::to_string(lfoNumber);
    std::string polarityName = "##Polarity" + std::to_string(lfoNumber);
    std::string freqName = "FreqLFOfrequency" + std::to_string(lfoNumber);
    std::string dutyName = "##LFOduty" + std::to_string(lfoNumber);

    mOscArgument = "/lfo" + std::to_string(lfoNumber + 1);

    shape = new al::ParameterMenu(menuName);
    polarity = new al::ParameterMenu(polarityName);
    frequency = new ecParameter(freqName, freqName, "", 1, 0.01, 30, 0.001, 10000, consts::LFO,
                                false, "%.3f Hz");
    duty = new al::Parameter(dutyName, "", 0.5, 0, 1);

    shape->setElements({"Sine", "Square", "Rise", "Fall", "Noise"});
    polarity->setElements({"BI", "UNI+", "UNI-"});
  }

  // Returns x offset for drawLFODuty
  int drawLFOControl(MIDILearnBool *isMIDILearn, KeyDown *k) {
    ImGuiIO &io = ImGui::GetIO();
    ImGui::Text("LFO%i", mLFONumber + 1);
    ImGui::SameLine();
    ImGui::PushItemWidth(70 * io.FontGlobalScale);
    al::ParameterGUI::drawMenu(shape);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushItemWidth(55 * io.FontGlobalScale);
    al::ParameterGUI::drawMenu(polarity);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    int x = ImGui::GetCursorPosX();
    frequency->drawRangeSlider(isMIDILearn, k);
    if (mOscCustomRange == 0) {
      mOscMin = frequency->getCurrentMin();
      mOscMax = frequency->getCurrentMax();
    }
    return x;
  }

  void drawLFODuty(MIDILearnBool *isMIDILearn, KeyDown *k, int x_offset) {
    if (*shape != 1) return;

    ImGuiIO &io = ImGui::GetIO();
    int sliderPos = x_offset;  // ImGui::GetCursorPosX();

    ImGui::SetCursorPosX(sliderPos - (35 * io.FontGlobalScale));
    ImGui::Text("Duty");
    ImGui::SameLine();
    ImGui::SetCursorPosX(sliderPos);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (35 * io.FontGlobalScale) + 8);
    al::ParameterGUI::drawParameter(duty);
    // MIDI LEARN Functionality
    isMIDILearn->mParamAdd = false;
    isMIDILearn->mParamDel = false;

    // Press m while hovering over a parameter to start midi learn.
    if (ImGui::IsItemHovered() && k->key.key() == static_cast<int>(consts::KEYBOARD_MIDI_LEARN) &&
        !k->key.shift() && k->readyToTrig) {
      isMIDILearn->mParamAdd = true;
    }

    // Press shift-m while hovering over a parameter to midi unlearn.
    if (ImGui::IsItemHovered() && k->key.key() == static_cast<int>(consts::KEYBOARD_MIDI_UNLEARN) &&
        k->key.shift() && k->readyToTrig) {
      isMIDILearn->mParamDel = true;
    }

    if ((ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))) {
      ImGui::OpenPopup(("rightClickSlider" + duty->getName()).c_str());
    }
    if (ImGui::BeginPopup(("rightClickSlider" + duty->getName()).c_str())) {
      if (ImGui::Selectable("MIDI Learn")) {
        isMIDILearn->mParamAdd = true;
      }
      if (ImGui::Selectable("MIDI Unlearn")) {
        isMIDILearn->mParamDel = true;
      }
      ImGui::EndPopup();
    }
    ImGui::PopItemWidth();
  }

  // destructor
  ~LFOstruct() {
    delete shape;
    delete frequency;
    delete polarity;
    delete duty;
  }
};

struct grainParameters {
  std::shared_ptr<ecParameter> transposition;
  float modTranspositionDepth;
  std::shared_ptr<ecParameter> filter;
  float modFilterDepth;
  std::shared_ptr<ecParameter> resonance;
  float modResonanceDepth;
  std::shared_ptr<ecParameter> grainDurationMs;
  float modGrainDurationDepth;
  std::shared_ptr<ecParameter> envelope;
  float modEnvelopeDepth;
  std::shared_ptr<ecParameter> pan;
  float modPanDepth;
  std::shared_ptr<ecParameter> volumeDB;
  float modVolumeDepth;
  std::shared_ptr<util::buffer<float>> source;
  int *activeVoices;
  float mCurrentIndex;
};

/**
 * Grain class containing an audio buffer and an envelope. Used as the voice for
 * the voiceScheduler class
 */
class Grain : public al::SynthVoice {
 public:
  Grain();
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
   *
   * @param[in] The global sampling rate.
   */
  void configureGrain(grainParameters &list, float samplingRate);

  /**
   * @brief Processing done at the audio rate.
   */
  virtual void onProcess(al::AudioIOData &io) override;

  /**
   * @brief This function will only be called once per trigger.
   */
  virtual void onTriggerOn() override;

  /**
   * @brief Sets duration of grain.
   *
   * @param[in] Set duration of grain in milliseconds.
   */
  void setDurationS(float dur) { mDurationS = dur; }

  float getDurationS() const { return mDurationS; }

  float getSourceIndex() const { return mSourceIndex; }

 private:
  std::shared_ptr<util::buffer<float>> source = nullptr;
  util::line<double> index;
  gam::Biquad<> bpf_1_l, bpf_1_r, bpf_2_l, bpf_2_r, bpf_3_l, bpf_3_r;
  grainEnvelope gEnv;
  bool bypassFilter = true;
  float currentSample, cascadeFilter = 0;
  int *mPActiveVoices;
  float envVal, sourceIndex, mSourceIndex, mDurationS, mLeft, mRight, mAmp;
  float PAN_CONST = std::sqrt(2) / 2;
  int iSourceIndex;
  float mSamplingRate = consts::SAMPLE_RATE;

  void configureIndex(const grainParameters &list);

  // Store value in mAmp;
  // Note that this is dependent on the active number of voices.
  void configureAmp(float dbIn);

  // Store value in mLeft and mRight.
  void configurePan(float inPan, float amp);

  void configureFilter(float freq, float resonance);

  float filterSample(float sample, float cascadeMix, bool isRight);

  void initEffects(float sr);

  // Temporary variables
  float before, after, dec;
  int prevSamplingRate = -1;
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
  voiceScheduler(double samplingRate = 48000) { mSamplingRate = samplingRate; }

  /**
   * @brief Set sampling rate.
   *
   * @param[in] The global audio sampling rate.
   */
  void setSamplingRate(double samplingRate) { mSamplingRate = samplingRate; }

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
  void setFrequency(double frequency) { configure(frequency, mAsync, mIntermittence); }

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
    // std::cout << mFrequency << std::endl;
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
  void setIntermittence(double intermittence) { configure(mFrequency, mAsync, intermittence); }

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

class Clipper : public al::AudioCallback {
  virtual void onAudioCB(al::AudioIOData &io) override {
    while (io()) {
      for (unsigned i = 0; i < io.channelsOut(); ++i) {
        if (io.out(i) > 1) io.sum(-1 * io.out(i) + 1, i);
        if (io.out(i) < -1) io.sum(-1 * io.out(i) - 1, i);
      }
    }
  }
};
#endif
