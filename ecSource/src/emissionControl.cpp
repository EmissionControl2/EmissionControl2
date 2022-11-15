/**
 * emissionControl.cpp
 *
 * AUTHOR: Jack Kilgore
 */

/**** Emission Control LIB ****/
#include "emissionControl.h"

/**** CSTD LIB ****/
#include <iostream>

using namespace al;

/******* grainEnvelope *******/

void grainEnvelope::setSamplingRate(float samplingRate) {
  mSamplingRate = samplingRate;
  mExpoEnv.setSamplingRate(mSamplingRate);
  mTurkeyEnv.setSamplingRate(mSamplingRate);
  mRExpoEnv.setSamplingRate(mSamplingRate);
}

float grainEnvelope::operator()() {
  if (mEnvelope < 0 || mEnvelope > 1)  // exponential envelope case
    mEnvelope = 0;

  if (mEnvelope < 0.5) {  // exponetial and turkey envelope interpolation
    mRExpoEnv.increment();
    return ((mExpoEnv() * (1 - mEnvelope * 2)) + (mTurkeyEnv() * mEnvelope * 2));
  } else if (mEnvelope == 0.5) {  // turkey envelope case
    mRExpoEnv.increment();
    mExpoEnv.increment();
    return mTurkeyEnv();
  } else if (mEnvelope <= 1) {  // turkey and reverse exponential envelope interpolation
    mExpoEnv.increment();
    return ((mTurkeyEnv() * (1 - (mEnvelope - 0.5) * 2)) + (mRExpoEnv() * (mEnvelope - 0.5) * 2));
  } else {  // fails silently but gracefully
    mRExpoEnv.increment();
    mExpoEnv.increment();
    return mTurkeyEnv();
  }
}

void grainEnvelope::reset() {
  mExpoEnv.set();
  mRExpoEnv.set();
  mTurkeyEnv.set();
}

void grainEnvelope::setDuration(float duration) {
  if (duration <= 0) {
    mDuration = 1;
  } else
    mDuration = duration;
  mTurkeyEnv.set(duration);
  mExpoEnv.set(duration, 0);
  mRExpoEnv.set(duration, 1);
}

void grainEnvelope::setEnvelope(float envelope) {
  if (envelope > 1)
    mEnvelope = 1;
  else if (envelope < 0)
    mEnvelope = 0;
  else
    mEnvelope = envelope;
}

bool grainEnvelope::done() { return mTurkeyEnv.done(); }

/******* ecModulator *******/
float ecModulator::operator()() {
  float result;
  if (mModWaveform == consts::SINE) {
    if (mPolarity == consts::BI)
      result = mLFO.cos();
    else {
      result = mLFO.cosU() * mSign;
    }
  } else if (mModWaveform == consts::SQUARE) {
    if (mPolarity == consts::BI) {
      result = mLFO.tri();
      if (result >= -2 * (mWidth - 0.5))
        result = 1.0;
      else
        result = -1.0;
    } else {
      result = mLFO.tri();
      if (result >= -1 * mSign * 2 * (mWidth - 0.5))
        result = 1.0 * mSign;
      else
        result = 0;
    }
  } else if (mModWaveform == consts::ASCEND) {
    if (mPolarity == consts::BI)
      result = mLFO.upU();
    else
      result = mLFO.upU() * mSign;
  } else if (mModWaveform == consts::DESCEND) {
    if (mPolarity == consts::BI)
      result = mLFO.downU();
    else
      result = mLFO.downU() * mSign;
  } else if (mModWaveform == consts::NOISE) {
    if (mPolarity == consts::BI)
      result = sampleAndHoldUniform(-1.0f, 1.0f);
    else
      result = mSign * sampleAndHoldUniform(0.f, 1.0f);
  } else {
    result = mLFO.cos();
  }
  return result;
}

void ecModulator::setWaveform(consts::waveform modWaveform) {
  if (modWaveform != consts::SINE && modWaveform != consts::ASCEND &&
      modWaveform != consts::DESCEND && modWaveform != consts::SQUARE &&
      modWaveform != consts::NOISE) {
    std::cerr << "invalid waveform" << std::endl;
    return;
  }
  mModWaveform = modWaveform;
}

void ecModulator::setWaveform(unsigned modWaveformIndex) {
  if (modWaveformIndex > 4) {
    std::cerr << "invalid waveform" << std::endl;
    return;
  }

  switch (modWaveformIndex) {
    case 0:
      mModWaveform = consts::SINE;
      break;
    case 1:
      mModWaveform = consts::SQUARE;
      break;
    case 2:
      mModWaveform = consts::ASCEND;
      break;
    case 3:
      mModWaveform = consts::DESCEND;
      break;
    case 4:
      mModWaveform = consts::NOISE;
      break;
  }
}

void ecModulator::setPolarity(consts::polarity modPolarity) {
  if (modPolarity != consts::BI && modPolarity != consts::UNI_NEG &&
      modPolarity != consts::UNI_POS) {
    std::cerr << "invalid polarity" << std::endl;
    return;
  }
  if (modPolarity == consts::UNI_POS) {
    mPolarity = consts::UNI;
    mSign = 1;
  } else if (modPolarity == consts::UNI_NEG) {
    mPolarity = consts::UNI;
    mSign = -1;
  } else {
    mPolarity = modPolarity;
    mSign = 1;  // THIS IS A DONT CARE
  }
}

void ecModulator::setPolarity(unsigned modPolarityIndex) {
  if (modPolarityIndex > 2) {
    std::cerr << "invalid waveform" << std::endl;
    return;
  }

  switch (modPolarityIndex) {
    case 0:
      setPolarity(consts::BI);
      break;
    case 1:
      setPolarity(consts::UNI_POS);
      break;
    case 2:
      setPolarity(consts::UNI_NEG);
      break;
    default:
      setPolarity(consts::BI);
      break;
  }
}

void ecModulator::setFrequency(float frequency) {
  mLFO.freq(frequency);
  mFrequency = frequency;
}

void ecModulator::setWidth(float width) {
  mLFO.mod(width);
  mWidth = width;
}

void ecModulator::setPhase(float phase) { mLFO.phase(phase); }

float ecModulator::sampleAndHoldUniform(float low, float high) {
  mLFO.nextPhase();
  lastPhase = currentPhase;
  currentPhase = mLFO.phaseI();
  if (lastPhase > currentPhase) {
    mHoldNoiseSample = rand.uniform(low, high);
    return mHoldNoiseSample;
  } else
    return mHoldNoiseSample;
}

/******* ecParameter *******/

ecParameter::ecParameter(std::string parameterName, std::string displayName, float defaultValue,
                         float defaultMin, float defaultMax, float absMin, float absMax,
                         consts::sliderType slideType, bool isLog, std::string sliderText,
                         bool independentMod) {
  mParameter = new Parameter{parameterName, defaultValue, defaultMin, defaultMax};
  mDisplayName = displayName;
  mParameter->displayName("##" + parameterName);
  mLowRange = new Parameter{("##" + parameterName + "Low").c_str(), defaultMin, absMin, absMax};
  mHighRange = new Parameter{("##" + parameterName + "High").c_str(), defaultMax, absMin, absMax};
  mLowRange->set(defaultMin);
  mHighRange->set(defaultMax);
  if (mOscCustomRange == 0) {
    mOscMin = defaultMin;
    mOscMax = defaultMax;
  }
  std::string tempArgument = parameterName;
  tempArgument.insert(tempArgument.begin(), '/');
  mOscArgument = tempArgument;
  mSliderType = slideType;
  mIsLog = isLog;
  mSliderText = sliderText;
  mIndependentMod = independentMod;
  if (mIndependentMod)  // if true, this parameter will have its own modulator
    mModulator = new ecModulator();
}

ecParameter::ecParameter(std::string parameterName, std::string displayName, std::string Group,
                         float defaultValue, float defaultMin, float defaultMax, float absMin,
                         float absMax, consts::sliderType slideType, bool isLog,
                         std::string sliderText, bool independentMod) {
  mParameter = new Parameter{parameterName, Group, defaultValue, defaultMin, defaultMax};
  mDisplayName = displayName;
  mParameter->displayName("##" + parameterName);
  mLowRange =
    new Parameter{("##" + parameterName + "Low").c_str(), Group, defaultMin, absMin, absMax};
  mHighRange =
    new Parameter{("##" + parameterName + "High").c_str(), Group, defaultMax, absMin, absMax};
  mLowRange->set(defaultMin);
  mHighRange->set(defaultMax);
  if (mOscCustomRange == 0) {
    mOscMin = defaultMin;
    mOscMax = defaultMax;
  }
  mSliderType = slideType;
  mIsLog = isLog;
  mSliderText = sliderText;
  mIndependentMod = independentMod;
  if (mIndependentMod)  // if true, this parameter will have its own modulator
    mModulator = new ecModulator();
}

ecParameter::~ecParameter() {
  delete mParameter;
  delete mLowRange;
  delete mHighRange;
  if (mIndependentMod) delete mModulator;
}

void ecParameter::setIndependentMod(bool independentMod) {
  mIndependentMod = independentMod;
  if (mIndependentMod && mModulator == nullptr)
    mModulator = new ecModulator();
  else
    delete mModulator;
}

float ecParameter::getModParam(float modWidth) {
  float temp;
  if (!mIndependentMod && mModSource.get() != nullptr)
    temp = mParameter->get() +
           (mModSource->getCurrentSample() * modWidth * (mHighRange->get() - mLowRange->get()));
  else if (mIndependentMod)
    temp =
      mParameter->get() + ((*mModulator)() * modWidth * (mHighRange->get() - mLowRange->get()));
  else {
    std::cerr << "No Valid Modulation source for ecParameter instance: "
              << mParameter->displayName() << std::endl;
    std::exit(1);
  }
  if (temp > mHighRange->get()) {
    return mHighRange->get();
  } else if (temp < mLowRange->get()) {
    return mLowRange->get();
  } else {
    return temp;
  }
}

void ecParameter::addToPresetHandler(al::PresetHandler &presetHandler) {
  presetHandler.registerParameter(*mParameter);
  presetHandler.registerParameter(*mLowRange);
  presetHandler.registerParameter(*mHighRange);
}

void ecParameter::skipParamPresetHandler(al::PresetHandler &presetHandler, bool skip) {
  presetHandler.skipParameter(mParameter->getFullAddress(), skip);
  presetHandler.skipParameter(mLowRange->getFullAddress(), skip);
  presetHandler.skipParameter(mHighRange->getFullAddress(), skip);
}

void ecParameter::drawRangeSlider(MIDILearnBool *isMIDILearn, KeyDown *k) {
  float valueSliderf, valueLowf, valueHighf;
  int valueSlideri, valueLowi, valueHighi;
  bool changed = false, isInt = isIntVal();
  ImGuiIO &io = ImGui::GetIO();
  ImGuiItemFlags slider_flags = (ImGuiItemFlags)NULL;

  // Draw left most range box.
  slider_flags = ImGuiSliderFlags_ClampOnInput;
  ImGui::PushItemWidth(50 * io.FontGlobalScale);
  if (isInt) {
    valueLowi = static_cast<int>(mLowRange->get());
    changed = ImGui::DragInt((mLowRange->displayName()).c_str(), &valueLowi, 0.1,
                             (int)mLowRange->min(), (int)mHighRange->get(), "%d", slider_flags);
  } else {
    valueLowf = mLowRange->get();
    changed = ImGui::DragFloat((mLowRange->displayName()).c_str(), &valueLowf, 0.1,
                               mLowRange->min(), mHighRange->get(), "%.3f", slider_flags);
  }
  ImGui::SameLine();
  if (changed && isInt) {
    setCurrentMin(valueLowi);
  } else if (changed && !isInt) {
    setCurrentMin(valueLowf);
  }
  if (isInt)
    mParameter->min(valueLowi);
  else
    mParameter->min(valueLowf);

  ImGui::PopItemWidth();

  // DRAW MAIN SLIDER
  ImGui::SameLine();
  if (mSliderType == consts::LFO || mSliderType == consts::INT_LFO) {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (85 * io.FontGlobalScale));
  } else if (mSliderType == consts::MOD || mSliderType == consts::INT_MOD) {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (58 * io.FontGlobalScale));
  } else if (mSliderType == consts::PARAM || mSliderType == consts::INT_PARAM) {
    if (ImGui::GetContentRegionAvail().x > (250 * io.FontGlobalScale)) {
      ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (175 * io.FontGlobalScale));
    } else {
      ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (45 * io.FontGlobalScale));
    }
  }

  if (isInt) {  // Draw int slider.
    valueSlideri = static_cast<int>(mParameter->get());
    if (mSliderText != "") {
      changed =
        ImGui::SliderInt((mParameter->displayName()).c_str(), &valueSlideri, mParameter->min(),
                         mParameter->max(), (mSliderText).c_str(), slider_flags);
    } else {
      changed = ImGui::SliderInt((mParameter->displayName()).c_str(), &valueSlideri,
                                 mParameter->min(), mParameter->max(), "%d", slider_flags);
    }
  } else {  // Draw float slider.

    // Funny way to visualize log versus linear.
    std::string lin_log_mod;
    if (isLog()) {
      slider_flags = ImGuiSliderFlags_ClampOnInput | ImGuiSliderFlags_Logarithmic;
      lin_log_mod = "";  // (
    } else
      lin_log_mod = "";  // /
    valueSliderf = mParameter->get();
    if (mSliderText != "") {
      changed =
        ImGui::SliderFloat((mParameter->displayName()).c_str(), &valueSliderf, mParameter->min(),
                           mParameter->max(), (lin_log_mod + mSliderText).c_str(), slider_flags);
    } else {
      changed =
        ImGui::SliderFloat((mParameter->displayName()).c_str(), &valueSliderf, mParameter->min(),
                           mParameter->max(), (lin_log_mod + "%0.3f").c_str(), slider_flags);
    }
  }

  if (mSliderType == consts::MOD || mSliderType == consts::INT_MOD ||
      mSliderType == consts::PARAM || mSliderType == consts::INT_PARAM) {
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", (getDisplayName()).c_str());
    ImGui::PopStyleVar();
  }

  // Protect Parameter Val While Editing with Ctrl-Click
  if (io.KeyCtrl && ImGui::IsItemClicked() && editing == false) {
    editing = true;
  }
  if (editing) {
    if (ImGui::IsItemDeactivatedAfterEdit() &&
        (ImGui::IsMouseDown(0) || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter)))) {
      changed = true;
      editing = false;
    } else if (ImGui::IsItemDeactivated() &&
               (ImGui::IsMouseDown(0) || ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Enter)))) {
      changed = false;
      editing = false;
    } else {
      changed = false;
    }
  }

  if (changed && isInt)
    mParameter->set(valueSlideri);
  else if (changed && !isInt)
    mParameter->set(valueSliderf);

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
    ImGui::OpenPopup(("rightClickSlider" + mParameter->getName()).c_str());
  }
  if (ImGui::BeginPopup(("rightClickSlider" + mParameter->getName()).c_str())) {
    if (ImGui::Selectable("MIDI Learn")) {
      isMIDILearn->mParamAdd = true;
    }
    if (ImGui::Selectable("MIDI Unlearn")) {
      isMIDILearn->mParamDel = true;
    }
    ImGui::Separator();
    if (ImGui::Selectable("Logarithmic")) {
      mIsLog ? setLog(false) : setLog(true);
    }
    ImGui::SameLine();
    ImGui::Checkbox("##logIndicator", &mIsLog);
    ImGui::EndPopup();
  }

  // Logarithm Stuff
  if (ImGui::IsItemHovered() &&
      k->key.key() == static_cast<int>(consts::KEYBOARD_PARAM_LOG_TOGGLE) && k->readyToTrig) {
    mIsLog ? setLog(false) : setLog(true);
  }

  ImGui::PopItemWidth();

  // Draw right most range box.
  ImGui::SameLine();
  slider_flags = ImGuiSliderFlags_ClampOnInput;
  ImGui::PushItemWidth(50 * io.FontGlobalScale);
  if (isInt) {
    valueHighi = static_cast<int>(mHighRange->get());
    changed = ImGui::DragInt((mHighRange->displayName()).c_str(), &valueHighi, 0.1,
                             (int)mLowRange->get(), (int)mHighRange->max(), "%d", slider_flags);
  } else {
    valueHighf = mHighRange->get();
    changed = ImGui::DragFloat((mHighRange->displayName()).c_str(), &valueHighf, 0.1,
                               mLowRange->get(), mHighRange->max(), "%.3f", slider_flags);
  }
  ImGui::SameLine();
  if (changed && isInt) {
    setCurrentMax(valueHighi);
  } else if (changed && !isInt) {
    setCurrentMax(valueHighf);
  }
  if (isInt)
    mParameter->max(valueHighi);
  else
    mParameter->max(valueHighf);

  ImGui::PopItemWidth();

  // Display name if necessary.
  ImGui::SameLine();
  if (mSliderType == consts::LFO || mSliderType == consts::INT_LFO)
    ImGui::Text("");
  else if (mSliderType == consts::MOD || mSliderType == consts::INT_MOD)
    ImGui::Text("");
  else if (mSliderType == consts::PARAM || mSliderType == consts::INT_PARAM)
    ImGui::Text("%s", getDisplayName().c_str());

  // A bit hacky, but...you know, ok.
  // If last parameter, allow readyToTrig to be false.
  if (k->lastParamCheck) {
    k->readyToTrig = false;
    k->lastParamCheck = false;
  }
}

/******* Grain Class *******/

Grain::Grain() {}

void Grain::init() { gEnv.reset(); }

void Grain::initEffects(float sr) {
  bpf_1_r.onDomainChange(1);
  bpf_2_r.onDomainChange(1);
  bpf_3_r.onDomainChange(1);
  bpf_1_l.onDomainChange(1);
  bpf_2_l.onDomainChange(1);
  bpf_3_l.onDomainChange(1);

  bpf_1_r.set(440, 1, gam::BAND_PASS);
  bpf_2_r.set(440, 1, gam::RESONANT);
  bpf_3_r.set(440, 1, gam::BAND_PASS);
  bpf_1_l.set(440, 1, gam::BAND_PASS);
  bpf_2_l.set(440, 1, gam::RESONANT);
  bpf_3_l.set(440, 1, gam::BAND_PASS);

  bpf_1_l.zero();
  bpf_2_l.zero();
  bpf_3_l.zero();
  bpf_1_r.zero();
  bpf_2_r.zero();
  bpf_3_r.zero();
}

void Grain::configureGrain(grainParameters &list, float samplingRate) {
  mPActiveVoices = list.activeVoices;
  this->source = list.source;

  if (static_cast<int>(samplingRate) != prevSamplingRate) {
    prevSamplingRate = samplingRate;
    mSamplingRate = samplingRate;
    initEffects(mSamplingRate);
  }

  // Set Duration
  if (list.modGrainDurationDepth > 0)
    setDurationS(list.grainDurationMs->getModParam(list.modGrainDurationDepth) / 1000);
  else
    setDurationS(list.grainDurationMs->getParam() / 1000);

  // Set Envelope
  gEnv.setSamplingRate(mSamplingRate);
  if (list.modEnvelopeDepth > 0)
    gEnv.set(mDurationS, list.envelope->getModParam(list.modEnvelopeDepth));
  else
    gEnv.set(mDurationS, list.envelope->getParam());

  // Configure what part of the buffer the grain will play;
  configureIndex(list);

  if (list.modVolumeDepth > 0)
    configureAmp(list.volumeDB->getModParam(list.modVolumeDepth));
  else
    configureAmp(list.volumeDB->getParam());

  // Store modulated pan value of grain IF it is being modulated.
  if (list.modPanDepth > 0)
    configurePan(list.pan->getModParam(list.modPanDepth), mAmp);
  else
    configurePan(list.pan->getParam(), mAmp);

  configureFilter(list.filter->getModParam(list.modFilterDepth),
                  list.resonance->getModParam(list.modResonanceDepth));
}

void Grain::configureIndex(const grainParameters &list) {
  float startSample, endSample;

  // Set where in the buffer to play.
  index.setSamplingRate(mSamplingRate);
  startSample = list.mCurrentIndex;
  endSample = startSample + (mDurationS * mSamplingRate *
                             list.transposition->getModParam(list.modTranspositionDepth));
  index.set(startSample, endSample, mDurationS);
}

void Grain::configureAmp(float dbIn) {
  // Convert volume from db to amplitude
  mAmp = powf(10, dbIn / 20);
  mAmp = mAmp * powf(*mPActiveVoices + 1,
                     -0.367877);  //  1/e PERFECT FOR grain overlap gain compensation
}

/* PAN PROCESS
In radians ---
LeftPan = 2√2(cos𝜃-sin𝜃)
RightPan = 2√2(cos𝜃+sin𝜃)
Where 𝜃 is in the range -pi/4 to pi/4
*/
void Grain::configurePan(float inPan, float amp) {
  float pan = inPan * (M_PI) / 4;
  float process_1 = std::cos(pan);
  float process_2 = std::sin(pan);
  mLeft = PAN_CONST * (process_1 - process_2) * amp;
  mRight = PAN_CONST * (process_1 + process_2) * amp;
}

void Grain::configureFilter(float freq, float resonance) {
  if (resonance >= 0 && resonance < 0.00001)
    bypassFilter = true;
  else
    bypassFilter = false;

  float res_process;
  res_process = powf(13, 2.9 * (resonance - 0.5));  // 13^{2.9\cdot\left(x-0.5\right)}
  cascadeFilter = res_process / 41.2304;            // Normalize by max resonance.

  bpf_1_l.freq(freq);
  bpf_2_l.freq(freq);
  bpf_3_l.freq(freq);

  bpf_1_l.res(res_process);
  bpf_2_l.res(log(res_process + 1));
  bpf_3_l.res(res_process);
  if (source->channels == 2) {
    bpf_1_r.freq(freq);
    bpf_2_r.freq(freq);
    bpf_3_r.freq(freq);

    bpf_1_r.res(res_process);
    bpf_2_r.res(log(res_process + 1));
    bpf_3_r.res(res_process);
  }
}

float Grain::filterSample(float sample, float cascadeMix, bool isRight) {
  float solo, cascade;
  if (!isRight) {
    solo = bpf_1_l.nextBP(sample);
    cascade = bpf_3_l.nextBP(bpf_2_l(solo));
  } else {
    solo = bpf_1_r.nextBP(sample);
    cascade = bpf_3_r.nextBP(bpf_2_r(solo));
  }

  return (solo * (1 - cascadeMix)) + (cascade * cascadeMix);
}

void Grain::onProcess(al::AudioIOData &io) {
  while (io()) {
    envVal = gEnv();
    sourceIndex = index();
    iSourceIndex = (int)(sourceIndex);
    if (iSourceIndex >= source->frames - source->channels || iSourceIndex < 0) {
      sourceIndex = fmod(sourceIndex, (float)(source->frames - source->channels));
      iSourceIndex = (int)(sourceIndex);
      if (iSourceIndex < 0) {
        sourceIndex += (source->frames - source->channels);
        iSourceIndex += (source->frames - source->channels);
      }
    }

    if (source->channels == 1) {
      before = source->data[iSourceIndex];
      after = source->data[iSourceIndex + 1];
      dec = sourceIndex - iSourceIndex;
      currentSample = before * (1 - dec) + after * dec;
      if (!bypassFilter) currentSample = filterSample(currentSample, cascadeFilter, 0);
      io.out(0, io.frame()) += currentSample * envVal * mLeft;
      io.out(1, io.frame()) += currentSample * envVal * mRight;

    } else if (source->channels == 2) {
      before = source->data[iSourceIndex * 2];
      after = source->data[iSourceIndex * 2 + 2];
      dec = sourceIndex - iSourceIndex;
      currentSample = before * (1 - dec) + after * dec;
      if (!bypassFilter) currentSample = filterSample(currentSample, cascadeFilter, 0);
      io.out(0, io.frame()) += currentSample * envVal * mLeft;

      before = source->get(iSourceIndex * 2 + 1);
      after = source->get((iSourceIndex * 2 + 3));
      dec = (sourceIndex + 1) - (iSourceIndex + 1);
      currentSample = before * (1 - dec) + after * dec;
      if (!bypassFilter) currentSample = filterSample(currentSample, cascadeFilter, 0);
      io.out(1, io.frame()) += currentSample * envVal * mRight;
    }
    mSourceIndex = sourceIndex;

    if (gEnv.done()) {
      *mPActiveVoices -= 1;  // This will remove a grain from the active list.
      free();
      break;
    }
  }
}

void Grain::onTriggerOn() {}

/******* voiceScheduler *******/

void voiceScheduler::configure(double frequency, double async, double intermittence) {
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

bool voiceScheduler::trigger() {
  if (mCounter >= 1.0) {
    mCounter -= 1.0;
    if (rand.uniform() < mIntermittence) return false;
    mCounter += rand.uniform(-mAsync, mAsync);
    mCounter += mIncrement;
    return true;
  }
  mCounter += mIncrement;
  return false;
}

void voiceScheduler::setPolyStream(consts::streamType type, int numStreams) {
  if (type == consts::synchronous) {
    setFrequency(static_cast<double>(mFrequency * numStreams));
  } else {
    std::cerr << "Not implemented yet, please try again later.\n";
  }
}
