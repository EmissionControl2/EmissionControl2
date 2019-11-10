//emissionControl.cpp

/**** Emission Control LIB ****/
#include "emissionControl.h"

/**** AlloLib LIB ****/
#include "al/io/al_Imgui.hpp"

/**** CSTD LIB ****/
#include <iostream>

using namespace al;

/******* grainEnvelope *******/

float grainEnvelope::operator()() {
  if(mEnvelope < 0 || mEnvelope > 1)  //exponential envelope case 
    mEnvelope = 0; 
  
  if (mEnvelope < 0.5) { //exponetial and turkey envelope interpolation
  mRExpoEnv.increment();
  return ((mExpoEnv() * (1 - mEnvelope*2)) + (mTurkeyEnv() * mEnvelope*2) );
  } else if (mEnvelope == 0.5) { //turkey envelope case 
  mRExpoEnv.increment();
  mExpoEnv.increment();
  return mTurkeyEnv();
  } else if (mEnvelope <= 1) { // turkey and reverse exponential envelope interpolation
  mExpoEnv.increment();
  return ((mTurkeyEnv() * (1 - (mEnvelope-0.5) * 2)) + (mRExpoEnv() * (mEnvelope - 0.5) * 2) );
  } else { //fails silently but gracefully
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

void ecModulator::setWaveform(consts::waveform modWaveform) {
  if (modWaveform != consts::SINE && modWaveform != consts::SAW &&
      modWaveform != consts::SQUARE && modWaveform != consts::NOISE) {
    std::cerr << "invalid waveform" << std::endl;
    return;
  }
  mModWaveform = modWaveform;
}

void ecModulator::setFrequency(float frequency) {
  mLFO.freq(frequency);
}

void ecModulator::setWidth(float width) {
    mWidth = width;
    mLFO.mod(width);
}

void ecModulator::setPhase(float phase) { 
  mLFO.phase(phase); 
}

/******* ecParameter *******/

ecParameter::ecParameter(std::string parameterName, float defaultValue,
            float defaultMin, float defaultMax ,
            float absMin, float absMax,
            consts::waveform modWaveform,
            bool independentMod) {
  mParameter = new Parameter{parameterName, defaultValue, defaultMin, defaultMax};
  mLowRange =
      new Parameter{(parameterName + "Low").c_str(), defaultMin, absMin, absMax};
  mHighRange =
      new Parameter{(parameterName + "High").c_str(), defaultMax, absMin, absMax};
  mMin = defaultMin;
  mMax = defaultMax;
  mModWaveform = modWaveform;
  mIndependentMod = independentMod;
  if (mIndependentMod)  // if true, this parameter will have its own modulator
    mModulator = new ecModulator{mModWaveform, 1, 1};
}


ecParameter::ecParameter(std::string parameterName, std::string Group,
            float defaultValue, std::string prefix,
            float defaultMin, float defaultMax,
            float absMin, float absMax,
            consts::waveform modWaveform,
            bool independentMod) {
  mParameter =
      new Parameter{parameterName, Group, defaultValue, prefix, defaultMin, defaultMax};
  mLowRange = new Parameter{
      (parameterName + "Low").c_str(), Group, defaultMin, prefix, absMin, absMax};
  mHighRange = new Parameter{
      (parameterName + "High").c_str(), Group, defaultMax, prefix, absMin, absMax};
  mMin = defaultMin;
  mMax = defaultMax;
  mModWaveform = modWaveform;
  mIndependentMod = independentMod;
  if (mIndependentMod)  // if true, this parameter will have its own modulator
    mModulator = new ecModulator{mModWaveform, 1, 1};
}

ecParameter::~ecParameter() {
    delete mParameter;
    if (mIndependentMod) delete mModulator;
}

void ecParameter::setWaveformIndex(int index) {
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

void ecParameter::setIndependentMod(bool independentMod) {
  mIndependentMod = independentMod;
  if (mIndependentMod && mModulator == nullptr)
    mModulator = new ecModulator{mModWaveform, 1, 1};
  else
    delete mModulator;
}

float ecParameter::getModParam(float modSineValue, float modSquareValue, float modSawValue,
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

float ecParameter::getModParam(float modWidth) {
  if (!mIndependentMod) {
    std::cerr << "PARAMETER must have independence set to true if you want "
                  "to use this getModParam function\n";
    return -9999999999;
  }
  return mParameter->get() * (((*mModulator)() * modWidth) + 1);
}

void ecParameter::draw() {
  float valueSlider, valueLow, valueHigh;
  bool changed;
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
  valueLow = mLowRange->get();
  changed = ImGui::DragFloat((mLowRange->displayName()).c_str(), &valueLow,
                              0.1, mLowRange->min(), mLowRange->max());
  ImGui::SameLine();
  if (changed) mLowRange->set(valueLow);
  // if(valueLow > mHighRange->get()) mParameter->min(mMin);
  mParameter->min(valueLow);

  ImGui::PopItemWidth();
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


/******* ecParameterInt *******/

ecParameterInt::ecParameterInt(std::string parameterName, std::string Group,
                 int defaultValue, std::string prefix, int min,
                 int max, consts::waveform modWaveform,
                 bool independentMod) {
  mParameterInt =
      new ParameterInt{parameterName, Group, defaultValue, prefix, min, max};
  mModWaveform = modWaveform;
  mIndependentMod = independentMod;
  if (mIndependentMod)  // if true, this parameter will have its own modulator
    mModulator = new ecModulator{mModWaveform, 1, 1};
}

ecParameterInt::~ecParameterInt() {
  delete mParameterInt;
  if (mIndependentMod) delete mModulator;
}

void ecParameterInt::setIndependentMod(bool independentMod) {
  mIndependentMod = independentMod;
  if (mIndependentMod && mModulator == nullptr)
    mModulator = new ecModulator{mModWaveform, 1, 1};
  else
    delete mModulator;
}

void ecParameterInt::setWaveformIndex(int index) {
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

int ecParameterInt::getModParam(float modSineValue, float modSquareValue, float modSawValue,
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

int ecParameterInt::getModParam(float modWidth) {
  if (!mIndependentMod) {
    std::cerr << "PARAMETER must have independence set to true if you want "
                  "to use this getModParam function\n";
    return -99999;
  }
  return mParameterInt->get() * (((*mModulator)() * modWidth) + 1);
}

void draw() {
  /**** TO DO ****/
  return;
}

/******* Grain Class *******/

void Grain::init() {
  gEnv.reset();
}

void Grain::configureGrain(grainParameters& list) {
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

void Grain::onProcess(al::AudioIOData& io) {
  while (io()) {
    envVal = gEnv();
    sourceIndex = index();
    if (sourceIndex > source->size) sourceIndex -= source->size;
    io.out(0) += source->get(sourceIndex) * envVal;
    io.out(1) += source->get(sourceIndex) * envVal;
    if (gEnv.done()) {
      *mPActiveVoices -= 1;  // This will remove a grain from the active list.
      free();
      break;
    }
  }
}

void Grain::onTriggerOn() {

}

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
    // std::cout << numStreams << std::endl;
    setFrequency(static_cast<double>(mFrequency * numStreams));
  } else {
    std::cerr << "Not implemented yet, please try again later.\n";
  }
}


/******* flowControl *******/

bool flowControl::throttle(float time, float ratio, int activeVoices) {
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