/**
 * emissionControl.cpp
 *
 * AUTHOR: Jack Kilgore
 */

/**** Emission Control LIB ****/
#include "emissionControl.h"

/**** AlloLib LIB ****/
#include "al/io/al_Imgui.hpp"

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
  if (mEnvelope < 0 || mEnvelope > 1) // exponential envelope case
    mEnvelope = 0;

  if (mEnvelope < 0.5) { // exponetial and turkey envelope interpolation
    mRExpoEnv.increment();
    return ((mExpoEnv() * (1 - mEnvelope * 2)) +
            (mTurkeyEnv() * mEnvelope * 2));
  } else if (mEnvelope == 0.5) { // turkey envelope case
    mRExpoEnv.increment();
    mExpoEnv.increment();
    return mTurkeyEnv();
  } else if (mEnvelope <=
             1) { // turkey and reverse exponential envelope interpolation
    mExpoEnv.increment();
    return ((mTurkeyEnv() * (1 - (mEnvelope - 0.5) * 2)) +
            (mRExpoEnv() * (mEnvelope - 0.5) * 2));
  } else { // fails silently but gracefully
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
    if (mPolarity == consts::BI)
      result = mLFO.stair();
    else
      result = mLFO.stairU() * mSign;
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
    mSign = 1; // THIS IS A DONT CARE
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

void ecModulator::setWidth(float width) { mLFO.mod(width); }

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

ecParameter::ecParameter(std::string parameterName, std::string displayName,
                         float defaultValue, float defaultMin, float defaultMax,
                         float absMin, float absMax, bool independentMod) {
  mParameter =
      new Parameter{parameterName, defaultValue, defaultMin, defaultMax};
  mDisplayName = displayName;
  mParameter->displayName("##" + parameterName);
  mLowRange = new Parameter{("##" + parameterName + "Low").c_str(), defaultMin,
                            absMin, absMax};
  mHighRange = new Parameter{("##" + parameterName + "High").c_str(),
                             defaultMax, absMin, absMax};
  mMin = defaultMin;
  mMax = defaultMax;
  mIndependentMod = independentMod;
  if (mIndependentMod) // if true, this parameter will have its own modulator
    mModulator = new ecModulator();
}

ecParameter::ecParameter(std::string parameterName, std::string displayName,
                         std::string Group, float defaultValue,
                         std::string prefix, float defaultMin, float defaultMax,
                         float absMin, float absMax, bool independentMod) {
  mParameter = new Parameter{parameterName, Group,      defaultValue,
                             prefix,        defaultMin, defaultMax};
  mDisplayName = displayName;
  mParameter->displayName("##" + parameterName);
  mLowRange = new Parameter{("##" + parameterName + "Low").c_str(),
                            Group,
                            defaultMin,
                            prefix,
                            absMin,
                            absMax};
  mHighRange = new Parameter{("##" + parameterName + "High").c_str(),
                             Group,
                             defaultMax,
                             prefix,
                             absMin,
                             absMax};
  mMin = defaultMin;
  mMax = defaultMax;
  mIndependentMod = independentMod;
  if (mIndependentMod) // if true, this parameter will have its own modulator
    mModulator = new ecModulator();
}

ecParameter::~ecParameter() {
  delete mParameter;
  delete mLowRange;
  delete mHighRange;
  if (mIndependentMod)
    delete mModulator;
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
    temp = mParameter->get() + (mModSource->getCurrentSample() * modWidth *
                                (mHighRange->get() - mLowRange->get()));
  else if (mIndependentMod)
    temp = mParameter->get() + ((*mModulator)() * modWidth *
                                (mHighRange->get() - mLowRange->get()));
  else {
    std::cerr << "No Valid Modulation source for ecParameter instance: "
              << mParameter->displayName() << std::endl;
    std::exit(1);
  }
  if (temp > mHighRange->get())
    return mHighRange->get();
  else if (temp < mLowRange->get())
    return mLowRange->get();
  else
    return temp;
}

void ecParameter::addToPresetHandler(al::PresetHandler &presetHandler) {
  presetHandler.registerParameter(*mParameter);
  presetHandler.registerParameter(*mLowRange);
  presetHandler.registerParameter(*mHighRange);
}

void ecParameter::drawRangeSlider(consts::sliderType slideType) {
  float valueSlider, valueLow, valueHigh;
  bool changed;

  ImGui::PushItemWidth(70);
  valueLow = mLowRange->get();
  changed = ImGui::DragFloat((mLowRange->displayName()).c_str(), &valueLow, 0.1,
                             mLowRange->min(), mLowRange->max(), "%.3f");
  ImGui::SameLine();
  if (changed)
    mLowRange->set(valueLow);
  mParameter->min(valueLow);

  ImGui::PopItemWidth();
  ImGui::SameLine();
  if (slideType == consts::LFO)
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 110);
  else if (slideType == consts::MOD)
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 90);
  else if (slideType == consts::PARAM)
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 210);
  valueSlider = mParameter->get();
  changed =
      ImGui::SliderFloat((mParameter->displayName()).c_str(), &valueSlider,
                         mParameter->min(), mParameter->max(), "%0.3f");
  if (changed)
    mParameter->set(valueSlider);
  ImGui::PopItemWidth();

  ImGui::SameLine();
  ImGui::PushItemWidth(70);
  valueHigh = mHighRange->get();
  changed = ImGui::DragFloat((mHighRange->displayName()).c_str(), &valueHigh,
                             0.1, mHighRange->min(), mHighRange->max());
  if (changed)
    mHighRange->set(valueHigh);
  mParameter->max(valueHigh);

  ImGui::PopItemWidth();

  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
  if (slideType == consts::LFO)
    ImGui::Text("Hz");
  else if (slideType == consts::MOD)
    ImGui::Text("");
  else if (slideType == consts::PARAM)
    ImGui::Text((getDisplayName()).c_str());

  ImGui::PopItemWidth();
}

/******* ecParameterInt *******/

ecParameterInt::ecParameterInt(std::string parameterName,
                               std::string displayName, std::string Group,
                               int defaultValue, std::string prefix,
                               int defaultMin, int defaultMax, int absMin,
                               int absMax, bool independentMod) {
  mParameterInt = new ParameterInt{parameterName, Group,      defaultValue,
                                   prefix,        defaultMin, defaultMax};
  mDisplayName = displayName;
  mParameterInt->displayName("##" + parameterName);
  mLowRange = new ParameterInt{("##" + parameterName + "Low").c_str(),
                               Group,
                               defaultMin,
                               prefix,
                               absMin,
                               absMax};
  mHighRange = new ParameterInt{("##" + parameterName + "High").c_str(),
                                Group,
                                defaultMax,
                                prefix,
                                absMin,
                                absMax};
  mMin = defaultMin;
  mMax = defaultMax;
  mIndependentMod = independentMod;
  if (mIndependentMod) // if true, this parameter will have its own modulator
    mModulator = new ecModulator();
}

ecParameterInt::~ecParameterInt() {
  delete mParameterInt;
  delete mLowRange;
  delete mHighRange;
  if (mIndependentMod)
    delete mModulator;
}

void ecParameterInt::setIndependentMod(bool independentMod) {
  mIndependentMod = independentMod;
  if (mIndependentMod && mModulator == nullptr)
    mModulator = new ecModulator();
  else
    delete mModulator;
}

int ecParameterInt::getModParam(float modWidth) {
  int temp;
  if (!mIndependentMod && mModSource.get() != nullptr)
    temp = mParameterInt->get() + (mModSource->getCurrentSample() * modWidth *
                                   (mHighRange->get() - mLowRange->get()));
  else if (mIndependentMod)
    temp = mParameterInt->get() + ((*mModulator)() * modWidth *
                                   (mHighRange->get() - mLowRange->get()));
  else {
    std::cerr << "No Valid Modulation source for ecParameterInt instance: "
              << mParameterInt->displayName() << std::endl;
    std::exit(1);
  }
  if (temp > mHighRange->get())
    return mHighRange->get();
  else if (temp < mLowRange->get())
    return mLowRange->get();
  else
    return temp;
}

void ecParameterInt::addToPresetHandler(al::PresetHandler &presetHandler) {
  presetHandler.registerParameter(*mParameterInt);
  presetHandler.registerParameter(*mLowRange);
  presetHandler.registerParameter(*mHighRange);
}

void ecParameterInt::drawRangeSlider() {
  int valueSlider, valueLow, valueHigh;
  bool changed;
  ImGui::PushItemWidth(70);
  valueLow = mLowRange->get();
  changed = ImGui::DragInt((mLowRange->displayName()).c_str(), &valueLow, 0.1,
                           mLowRange->min(), mLowRange->max());
  ImGui::SameLine();
  if (changed)
    mLowRange->set(valueLow);
  mParameterInt->min(valueLow);

  // if(valueLow > mHighRange->get()) mParameter->min(mMin);

  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() - 210);
  valueSlider = mParameterInt->get();
  changed =
      ImGui::SliderInt((mParameterInt->displayName()).c_str(), &valueSlider,
                       mParameterInt->min(), mParameterInt->max());
  if (changed)
    mParameterInt->set(valueSlider);
  ImGui::PopItemWidth();

  ImGui::SameLine();
  ImGui::PushItemWidth(70);
  valueHigh = mHighRange->get();
  changed = ImGui::DragInt((mHighRange->displayName()).c_str(), &valueHigh, 0.1,
                           mHighRange->min(), mHighRange->max());

  if (changed)
    mHighRange->set(valueHigh);

  mParameterInt->max(valueHigh);

  ImGui::PopItemWidth();

  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
  ImGui::Text((getDisplayName()).c_str());
  ImGui::PopItemWidth();
}

/******* Grain Class *******/

Grain::Grain() {}

void Grain::init() {
  gEnv.reset();
  bpf_1_r.type(gam::BAND_PASS);
  bpf_2_r.type(gam::BAND_PASS);
  bpf_3_r.type(gam::LOW_PASS);
  bpf_1_l.type(gam::BAND_PASS);
  bpf_2_l.type(gam::BAND_PASS);
  bpf_3_l.type(gam::LOW_PASS);
}

void Grain::configureGrain(grainParameters &list, float samplingRate) {
  float startSample, endSample;

  mPActiveVoices = list.activeVoices;

  // Set Duration
  if (list.modGrainDurationDepth > 0)
    setDurationMs(list.grainDurationMs.getModParam(list.modGrainDurationDepth));
  else
    setDurationMs(list.grainDurationMs.getParam());

  // Set Envelope
  gEnv.setSamplingRate(samplingRate);
  if (list.modEnvelopeDepth > 0)
    gEnv.set(mDurationMs / 1000,
             list.envelope.getModParam(list.modEnvelopeDepth));
  else
    gEnv.set(mDurationMs / 1000, list.envelope.getParam());

  // Set sample
  this->source = list.source;

  // Set where in the buffer to play.
  index.setSamplingRate(samplingRate);
  if (list.modTapeHeadDepth > 0)
    // NOTE: the tape head wraps around to the beginning of the buffer when
    // it exceeds its buffer size.
    startSample = floor(source->size / source->channels *
                        (list.tapeHead.getModParam(list.modTapeHeadDepth)));
  else
    startSample =
        floor(source->size / source->channels * list.tapeHead.getParam());

  if (list.modTranspositionDepth > 0)
    endSample = floor(startSample + ((mDurationMs / 1000) * samplingRate *
                                     abs(list.transposition.getModParam(
                                         list.modTranspositionDepth))));
  else
    endSample = floor(startSample + ((mDurationMs / 1000) * samplingRate *
                                     abs(list.transposition.getParam())));

  if (list.transposition.getParam() < 0)
    index.set(endSample, startSample, mDurationMs / 1000);
  else
    index.set(startSample, endSample, mDurationMs / 1000);

  if (list.modVolumeDepth > 0)
    configureAmp(list.volumeDB.getModParam(list.modVolumeDepth));
  else
    configureAmp(list.volumeDB.getParam());

  // Store modulated pan value of grain IF it is being modulated.
  if (list.modPanDepth > 0)
    configurePan(list.pan.getModParam(list.modPanDepth));
  else
    configurePan(list.pan.getParam());

  configureFilter(list.filter.getModParam(list.modFilterDepth),
                  list.resonance.getModParam(list.modResonanceDepth));
}

void Grain::onProcess(al::AudioIOData &io) {
  while (io()) {
    envVal = gEnv();
    sourceIndex = index();

    if (sourceIndex > source->size)
      sourceIndex -= source->size;

    if (source->channels == 1) {
      currentSample = source->get(sourceIndex);
      currentSample = filterSample(currentSample, bypassFilter, cascadeFilter,
                                   freqMakeup, 0);
      io.out(0) += currentSample * envVal * mLeft * mAmp;
      io.out(1) += currentSample * envVal * mRight * mAmp;
    } else if (source->channels == 2) {

      before = source->data[(int)floor(sourceIndex) * 2];
      after = source->data[(int)floor(sourceIndex) * 2 + 2];
      dec = sourceIndex - floor(sourceIndex);
      currentSample = before * (1 - dec) + after * dec;
      currentSample = filterSample(currentSample, bypassFilter, cascadeFilter,
                                   freqMakeup, 0);
      io.out(0) += currentSample * envVal * mLeft * mAmp;

      before = source->get(floor(sourceIndex + 1) * 2.0);
      after = source->get(floor(sourceIndex + 1) * 2.0 + 2);
      dec = (sourceIndex + 1) - floor(sourceIndex + 1);
      currentSample = before * (1 - dec) + after * dec;
      currentSample = filterSample(currentSample, bypassFilter, cascadeFilter,
                                   freqMakeup, 1);
      io.out(1) += currentSample * envVal * mRight * mAmp;
    }

    if (gEnv.done()) {
      *mPActiveVoices -= 1; // This will remove a grain from the active list.
      free();
      break;
    }
  }
}

void Grain::onTriggerOn() {}

void Grain::configureAmp(float dbIn) {
  // Convert volume from db to amplitude
  mAmp = powf(10, dbIn / 20);
  mAmp =
      mAmp *
      powf(*mPActiveVoices + 1,
           -0.367877); //  1/e PERFECT FOR grain overlap gain compensation
}

/* PAN PROCESS
In radians ---
LeftPan = 2√2(cos𝜃-sin𝜃)
RightPan = 2√2(cos𝜃+sin𝜃)
Where 𝜃 is in the range -pi/4 to pi/4
*/
void Grain::configurePan(float inPan) {
  float pan = inPan * (M_PI) / 4;
  float process_1 = std::cos(pan);
  float process_2 = std::sin(pan);
  mLeft = PAN_CONST * (process_1 - process_2);
  mRight = PAN_CONST * (process_1 + process_2);
}

void Grain::configureFilter(float freq, float resonance) {

  if (resonance >= 0 && resonance < 0.00001)
    bypassFilter = true;
  else
    bypassFilter = false;

  float res_process;
  res_process = powf(13,2.9 * (resonance-0.5)); //10^{3.25\cdot\left(x-0.5\right)}
  cascadeFilter = resonance;

  freqMakeup = (96000000 / (freq * freq / 2) + 3.6) * resonance; //(res_process/41.2304);

  if (freqMakeup > 750 && freq < 120)
    freqMakeup = 750;

  std::cout << "ORIG: " << resonance << " PROCeSS: " << res_process << std::endl;

  std::cout << freqMaekup << std::endl;

  bpf_1_l.freq(freq);
  bpf_2_l.freq(freq);
  bpf_3_l.freq(freq);

  bpf_1_l.res(res_process);
  bpf_2_l.res(res_process);
  bpf_3_l.res(res_process);
  if (source->channels == 2) {
    bpf_1_r.freq(freq);
    bpf_2_r.freq(freq);
    bpf_3_r.freq(freq);

    bpf_1_r.res(res_process);
    bpf_2_r.res(res_process);
    bpf_3_r.res(res_process);
  }
}

float Grain::filterSample(float sample, bool isBypass, float cascadeMix,
                          float freqMakeupCoeff, bool isRight) {
  if (isBypass)
    return sample;

  float solo, cascade;
  if (!isRight) {
    solo = bpf_1_l.nextBP(sample);
    cascade = bpf_2_l.nextBP(bpf_3_l.nextBP(bpf_1_l.nextBP(sample)));
  } else {
    solo = bpf_1_r.nextBP(sample);
    cascade = bpf_2_r.nextBP(bpf_3_r.nextBP(bpf_1_r.nextBP(sample)));
  }

  return (solo * (1 - cascadeMix)) + (cascade * cascadeMix * freqMakeup);
}

/******* voiceScheduler *******/

void voiceScheduler::configure(double frequency, double async,
                               double intermittence) {
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
    if (rand.uniform() < mIntermittence)
      return false;
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

/******* flowControl *******/

bool flowControl::throttle(float time, float ratio, int activeVoices) {
  // if (mCounter < time * mSamplingRate) {
  //   mCounter++;
  //   mAvgActiveVoices += activeVoices;
  //   return false;
  // } else {
  //   mCounter++;
  //   mAvgActiveVoices /= mCounter;
  //   mCounter = 0;
  // }
  // return true;
  // float adaptThresh;

  // if (getPeakCPU() > adaptThresh) {
  //   return true;
  // }
  // if (getAvgCPU() > adaptThresh) {
  //   return true;
  // } else {
  //   return false;
  // }
  return false;
}
