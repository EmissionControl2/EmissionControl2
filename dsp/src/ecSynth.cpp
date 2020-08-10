/**
 * ecSynth.cpp
 *
 * AUTHOR: Jack Kilgore
 */

/**** Emission Control LIB ****/
#include "ecSynth.h"
#include "utility.h"

/**** ALLOLIB ****/
#include "al/io/al_File.hpp"

using namespace al;

/**** ecSynth Implementation ****/

void ecSynth::setIO(al::AudioIOData *io) {
  mPrevSR = mGlobalSamplingRate;
  grainScheduler.setSamplingRate(io->fps());
  mGlobalSamplingRate = io->fps();
}

void ecSynth::mapParameters() {
  Parameters[GRAIN_RATE] = &grainRate;
  Parameters[GRAIN_RATE_WIDTH] = &modGrainRateWidth;
  Parameters[ASYNC] = &asynchronicity;
  Parameters[ASYNC_WIDTH] = &modAsynchronicityWidth;
  Parameters[INTERM] = &intermittency;
  Parameters[INTERM_WIDTH] = &modIntermittencyWidth;
  Parameters[GRAIN_DUR] = &grainDurationMs;
  Parameters[GRAIN_DUR_WIDTH] = &modGrainDurationWidth;
  Parameters[ENVELOPE] = &envelope;
  Parameters[ENVELOPE_WIDTH] = &modEnvelopeWidth;
  Parameters[TRANSPOSE] = &transposition;
  Parameters[TRANSPOSE_WIDTH] = &modTranspositionWidth;
  Parameters[FILTER] = &filter;
  Parameters[FILTER_WIDTH] = &modFilterWidth;
  Parameters[RESONANCE] = &resonance;
  Parameters[RESONANCE_WIDTH] = &modResonanceWidth;
  Parameters[TAPE_HEAD] = &tapeHead;
  Parameters[TAPE_HEAD_WIDTH] = &modTapeHeadWidth;
  Parameters[PAN] = &pan;
  Parameters[PAN_WIDTH] = &modPanWidth;
  Parameters[VOLUME] = &volumeDB;
  Parameters[VOLUME_WIDTH] = &modVolumeWidth;
}

void ecSynth::init(al::AudioIOData *io) {
  int index;

  for (index = 0; index < NUM_MODULATORS; index++) {
    Modulators.push_back(std::make_shared<ecModulator>());
  }

  for (index = 0; index < NUM_MODULATORS; index++) {
    LFOparameters.push_back(new LFOstruct{index});
  }

  mapParameters();

  mGlobalSamplingRate = io->fps();
  mPrevSR = io->fps();

  mPActiveVoices = &mActiveVoices;

  // MUST USE THIS ORDER
  grainRateLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  grainRate.setModulationSource(Modulators[0]); // Default
  grainRateLFO.registerChangeCallback(
      [&](int value) { grainRate.setModulationSource(Modulators[value]); });
  asyncLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  asynchronicity.setModulationSource(Modulators[0]);
  asyncLFO.registerChangeCallback([&](int value) {
    asynchronicity.setModulationSource(Modulators[value]);
  });
  intermittencyLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  intermittency.setModulationSource(Modulators[0]);
  intermittencyLFO.registerChangeCallback(
      [&](int value) { intermittency.setModulationSource(Modulators[value]); });
  streamsLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  streams.setModulationSource(Modulators[0]);
  streamsLFO.registerChangeCallback(
      [&](int value) { streams.setModulationSource(Modulators[value]); });
  grainDurationLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  grainDurationMs.setModulationSource(Modulators[0]);
  grainDurationLFO.registerChangeCallback([&](int value) {
    grainDurationMs.setModulationSource(Modulators[value]);
  });
  envelopeLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  envelope.setModulationSource(Modulators[0]);
  envelopeLFO.registerChangeCallback(
      [&](int value) { envelope.setModulationSource(Modulators[value]); });
  tapeHeadLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  tapeHead.setModulationSource(Modulators[0]);
  tapeHeadLFO.registerChangeCallback(
      [&](int value) { tapeHead.setModulationSource(Modulators[value]); });
  transpositionLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  transposition.setModulationSource(Modulators[0]);
  transpositionLFO.registerChangeCallback(
      [&](int value) { transposition.setModulationSource(Modulators[value]); });

  filterLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  filter.setModulationSource(Modulators[0]);
  filterLFO.registerChangeCallback(
      [&](int value) { filter.setModulationSource(Modulators[value]); });

  resonanceLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  resonance.setModulationSource(Modulators[0]);
  resonanceLFO.registerChangeCallback(
      [&](int value) { resonance.setModulationSource(Modulators[value]); });

  volumeLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  volumeDB.setModulationSource(Modulators[0]);
  volumeLFO.registerChangeCallback(
      [&](int value) { volumeDB.setModulationSource(Modulators[value]); });
  panLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  pan.setModulationSource(Modulators[0]);
  panLFO.registerChangeCallback(
      [&](int value) { pan.setModulationSource(Modulators[value]); });
  soundFileLFO.setElements({"LFO1", "LFO2", "LFO3", "LFO4"});
  soundFile.setModulationSource(Modulators[0]);
  soundFileLFO.registerChangeCallback(
      [&](int value) { soundFile.setModulationSource(Modulators[value]); });

  grainScheduler.configure(grainRate.getParam(), 0.0, 0.0);

  // FOR LOOP CAUSES CRASHES ???
  LFOparameters[0]->shape->registerChangeCallback(
      [&](int value) { Modulators[0]->setWaveform(value); });

  LFOparameters[0]->polarity->registerChangeCallback(
      [&](int value) { Modulators[0]->setPolarity(value); });

  LFOparameters[0]->frequency->mParameter->registerChangeCallback(
      [&](float value) { Modulators[0]->setFrequency(value); });

  LFOparameters[0]->duty->registerChangeCallback(
      [&](float value) { Modulators[0]->setWidth(value); });

  LFOparameters[1]->shape->registerChangeCallback(
      [&](int value) { Modulators[1]->setWaveform(value); });

  LFOparameters[1]->polarity->registerChangeCallback(
      [&](int value) { Modulators[1]->setPolarity(value); });

  LFOparameters[1]->frequency->mParameter->registerChangeCallback(
      [&](float value) { Modulators[1]->setFrequency(value); });

  LFOparameters[1]->duty->registerChangeCallback(
      [&](float value) { Modulators[1]->setWidth(value); });

  LFOparameters[2]->shape->registerChangeCallback(
      [&](int value) { Modulators[2]->setWaveform(value); });

  LFOparameters[2]->polarity->registerChangeCallback(
      [&](int value) { Modulators[2]->setPolarity(value); });

  LFOparameters[2]->frequency->mParameter->registerChangeCallback(
      [&](float value) { Modulators[2]->setFrequency(value); });

  LFOparameters[2]->duty->registerChangeCallback(
      [&](float value) { Modulators[2]->setWidth(value); });

  LFOparameters[3]->shape->registerChangeCallback(
      [&](int value) { Modulators[3]->setWaveform(value); });

  LFOparameters[3]->polarity->registerChangeCallback(
      [&](int value) { Modulators[3]->setPolarity(value); });

  LFOparameters[3]->frequency->mParameter->registerChangeCallback(
      [&](float value) { Modulators[3]->setFrequency(value); });

  LFOparameters[3]->duty->registerChangeCallback(
      [&](float value) { Modulators[3]->setWidth(value); });

  /**
   * WHY DOES THIS CRASH ??
   */
  // for (int index = 0; index < NUM_MODULATORS; ++index) {
  //   std::cout << "INDEX: " << index << std::endl;
  //   LFOparameters[index]->shape->registerChangeCallback(
  //       [&](int value) { Modulators[index]->setWaveform(value); });

  //   LFOparameters[index]->frequency->registerChangeCallback(
  //       [&](float value) { Modulators[index]->setFrequency(value); });

  //   LFOparameters[index]->duty->registerChangeCallback(
  //       [&](float value) { Modulators[index]->setWidth(value); });
  // }

  grainSynth.allocatePolyphony<Grain>(1024);
  grainSynth.setDefaultUserData(this);

  /**
   * Input correct number of files into parameters.
   */
  soundFile.mParameterInt->max(mClipNum);
  soundFile.mLowRange->max(mClipNum);
  soundFile.mHighRange->max(mClipNum);
  soundFile.mHighRange->set(mClipNum);
}

void ecSynth::onProcess(AudioIOData &io) {
  //        updateFromParameters();
  /* Manipulate on a Grain Level */

  while (io()) {
    for (int index = 0; index < NUM_MODULATORS; ++index)
      Modulators[index]->sampleAndStore();

    // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER

    // NOTE grainRate noise isnt very perceptible
    if (modGrainRateWidth.getParam() > 0) // modulate the grain rate
      grainScheduler.setFrequency(
          grainRate.getModParam(modGrainRateWidth.getParam()));
    else
      grainScheduler.setFrequency(grainRate.getParam());

    if (modAsynchronicityWidth.getParam() > 0) // modulate the asynchronicity
      grainScheduler.setAsynchronicity(
          asynchronicity.getModParam(modAsynchronicityWidth.getParam()));
    else
      grainScheduler.setAsynchronicity(asynchronicity.getParam());

    if (modIntermittencyWidth.getParam() > 0) // modulate the intermittency
      grainScheduler.setIntermittence(
          intermittency.getModParam(modIntermittencyWidth.getParam()));
    else
      grainScheduler.setIntermittence(intermittency.getParam());

    if (modStreamsWidth.getParam() >
        0) // Modulate the amount of streams playing.
      grainScheduler.setPolyStream(
          consts::synchronous, streams.getModParam(modStreamsWidth.getParam()));
    else
      grainScheduler.setPolyStream(consts::synchronous, streams.getParam());

    // CONTROL RATE LOOP (Executes every 4th sample)
    if (controlRateCounter == 4) {
      controlRateCounter = 0;
      mModClip = soundFile.getModParam(modSoundFileWidth.getParam()) - 1;
    }
    controlRateCounter++;
    /////

    // Grain by Grain Initilization
    if (grainScheduler.trigger()) {
      auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
      if (voice) {
        grainParameters list = {grainDurationMs,
                                modGrainDurationWidth.getParam(),
                                envelope,
                                modEnvelopeWidth.getParam(),
                                tapeHead,
                                modTapeHeadWidth.getParam(),
                                transposition,
                                modTranspositionWidth.getParam(),
                                filter,
                                modFilterWidth.getParam(),
                                resonance,
                                modResonanceWidth.getParam(),
                                volumeDB,
                                modVolumeWidth.getParam(),
                                pan,
                                modPanWidth.getParam(),
                                soundClip[mModClip],
                                mPActiveVoices};

        voice->configureGrain(list, mGlobalSamplingRate);

        mActiveVoices++;
        grainSynth.triggerOn(voice, io.frame());

      } else {
        std::cout << "out of voices!" << std::endl;
      }
    }
  }

  grainSynth.render(io);

  io.frame(0);
  // Manipulate on a stream level
  while (io()) {
    // Set clip vars to > 0 if clip is detected on any sample
    // 5 is the number of visual frames the meter will turn red for
    if (io.out(0) > 1.0f || io.out(0) < -1.0f)
      clipL = 5;
    if (io.out(1) > 1.0f || io.out(1) < -1.0f)
      clipR = 5;
    // add samples to oscilloscope ringbuffer
    oscBufferL.push_back(io.out(0));
    oscBufferR.push_back(io.out(1));
    // Add samples to VU ringbuffer (squared for RMS calculations)
    vuBufferL.push_back(pow(io.out(0), 2));
    vuBufferR.push_back(pow(io.out(1), 2));
  }
}

void ecSynth::onTriggerOn() {}

void ecSynth::onTriggerOff() {}

void ecSynth::loadSoundFile(std::string fileName) {
  if (std::find(soundClipFileName.begin(), soundClipFileName.end(), fileName) !=
      soundClipFileName.end())
    return;
  bool temp = util::load(fileName, soundClip, mGlobalSamplingRate, true);
  if (temp) {
    soundClipFileName.push_back(fileName);
    mClipNum++;
    soundFile.mParameterInt->max(mClipNum);
    soundFile.mLowRange->max(mClipNum);
    soundFile.mHighRange->max(mClipNum);
    soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out
  }
}

bool ecSynth::loadInitSoundFiles(std::string directory) {
  FileList initAudioFiles = fileListFromDir(directory);
  initAudioFiles.sort(util::compareFileNoCase);

  bool success = false;
  for (auto i = initAudioFiles.begin(); i != initAudioFiles.end(); i++) {
    if (i->file().substr(i->file().length() - 4) == ".wav" ||
        i->file().substr(i->file().length() - 4) == ".aif") {
      loadSoundFile(i->filepath());
      success = true;
    } else if (i->file().substr(i->file().length() - 5) == ".aiff") {
      loadSoundFile(i->filepath());
      success = true;
    }
  }
  return success;
}

bool ecSynth::removeSoundFile(int index) {
  if (mClipNum == 0)
    return false;
  soundClip.erase(soundClip.begin() + index);
  soundClipFileName.erase(soundClipFileName.begin() + index);
  mClipNum--;
  soundFile.mParameterInt->max(mClipNum);
  soundFile.mLowRange->max(mClipNum);
  soundFile.mHighRange->max(mClipNum);
  soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out

  if (soundFile.mParameterInt->get() >= index)
    soundFile.mParameterInt->set(soundFile.mParameterInt->get() - 1);
  return true;
}

bool ecSynth::removeCurrentSoundFile() {
  removeSoundFile(soundFile.mParameterInt->get() - 1);
}

void ecSynth::clearSoundFiles() {
  soundClip.clear();
  soundClipFileName.clear();

  mClipNum = 0;
  soundFile.mParameterInt->max(mClipNum);
  soundFile.mLowRange->max(mClipNum);
  soundFile.mHighRange->max(mClipNum);
  soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out
}

void ecSynth::resampleSoundFiles() {
  // If sampling rate is the same as before, no need for resampling.
  if (static_cast<int>(mPrevSR) == static_cast<int>(mGlobalSamplingRate))
    return;
  std::vector<std::string> filePaths;
  long unsigned i;
  // Collect filepaths of audio buffers.
  for (i = 0; i < soundClip.size(); i++) {
    filePaths.push_back(soundClip[i]->filePath);
  }

  clearSoundFiles();
  for (long unsigned i = 0; i < filePaths.size(); i++)
    loadSoundFile(filePaths[i]);
}

void ecSynth::hardClip(al::AudioIOData &io) {
  for (unsigned i = 0; i < io.channelsOut(); ++i) {
    if (io.out(i) > 1)
      io.sum(-1 * io.out(i) + 1, i);
    if (io.out(i) < -1)
      io.sum(-1 * io.out(i) - 1, i);
  }
}

/**
 * WIP -- runs at max 2/3 of full power
 * softClip(currentSample) :
 *    -2/3    if x < -1
 *     2/3    if x > 1
 *     currentSample - (currentSample)**3/3
 */
void ecSynth::softClip(al::AudioIOData &io) {
  for (unsigned int i = 0; i < io.channelsOut(); ++i) {
    float currentSample = io.out(i);
    if (currentSample > 1)
      io.sum(-1 * currentSample + (2.0f / 3), i);
    else if (currentSample < -1)
      io.sum(-1 * currentSample - (2.0f / 3), i);
    else
      io.sum(-1 * powf(currentSample, 3) / 3, i);
  }
}

/**** TO DO TO DO TO DO ****/
void ecSynth::throttle(float time, float ratio) {
  if (mCounter < time * mGlobalSamplingRate) {
    mCounter++;
    mAvgActiveVoices += mActiveVoices;
    return;
  } else {
    mCounter++;
    mAvgActiveVoices /= mCounter;
    mCounter = 0;
  }

  // float adaptThresh;

  // if (mPeakCPU > adaptThresh) {
  // }
  // if (mAvgCPU > adaptThresh) {
  // } else {
  // }
}
