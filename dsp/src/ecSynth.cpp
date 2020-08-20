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

void ecSynth::initialize(al::AudioIOData *io) {
  initParameters();

  for (int index = 0; index < consts::NUM_LFOS; index++) {
    Modulators.push_back(std::make_shared<ecModulator>());
    LFOparameters.push_back(std::make_shared<LFOstruct>(index));
  }

  mGlobalSamplingRate = io->fps();
  mPrevSR = io->fps();

  mPActiveVoices = &mActiveVoices;

  mScanner.setSamplingRate(mGlobalSamplingRate);

  // MUST USE THIS ORDER
  std::vector<std::string> lfo_names{"LFO1", "LFO2", "LFO3", "LFO4"};
  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    ECParameters[index]->setModulationSource(Modulators[0]);
    ECModParameters[index]->setMenuElements(lfo_names);
    ECModParameters[index]->registerMenuChangeCallback(
        [this, index](int value) {
          ECParameters[index]->setModulationSource(Modulators[value]);
        });
  }

  for (unsigned index = 0; index < consts::NUM_LFOS; index++) {
    LFOparameters[index]->shape->registerChangeCallback(
        [this, index](int value) { Modulators[index]->setWaveform(value); });

    LFOparameters[index]->polarity->registerChangeCallback(
        [this, index](int value) { Modulators[index]->setPolarity(value); });

    LFOparameters[index]->frequency->mParameter->registerChangeCallback(
        [this, index](float value) { Modulators[index]->setFrequency(value); });

    LFOparameters[index]->duty->registerChangeCallback(
        [this, index](float value) { Modulators[index]->setWidth(value); });
  }

  grainScheduler.configure(ECParameters[consts::GRAIN_RATE]->getParam(), 0.0,
                           0.0);
  mScanner.set(ECParameters[consts::SCAN_POS]->getParam() *
                   soundClip[0]->frames,
               soundClip[0]->frames, mGlobalSamplingRate);

  grainSynth.allocatePolyphony<Grain>(2048);
  grainSynth.setDefaultUserData(this);
}

void ecSynth::onProcess(AudioIOData &io) {
  //        updateFromParameters();
  /* Manipulate on a Grain Level */

  while (io()) {
    for (int index = 0; index < NUM_MODULATORS; ++index)
      Modulators[index]->sampleAndStore();

    // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER
    width = ECModParameters[consts::GRAIN_RATE]->getWidthParam();
    if (width > 0)
      grainScheduler.setFrequency(
          ECParameters[consts::GRAIN_RATE]->getModParam(width));
    else
      grainScheduler.setFrequency(ECParameters[consts::GRAIN_RATE]->getParam());

    width = ECModParameters[consts::ASYNC]->getWidthParam();
    if (width > 0) // modulate the asynchronicity
      grainScheduler.setAsynchronicity(
          ECParameters[consts::ASYNC]->getModParam(width));
    else
      grainScheduler.setAsynchronicity(ECParameters[consts::ASYNC]->getParam());

    width = ECModParameters[consts::INTERM]->getWidthParam();
    if (width > 0) // modulate the intermittency
      grainScheduler.setIntermittence(
          ECParameters[consts::INTERM]->getModParam(width));
    else
      grainScheduler.setIntermittence(ECParameters[consts::INTERM]->getParam());

    width = ECModParameters[consts::STREAMS]->getWidthParam();
    if (width > 0) // Modulate the amount of streams playing.
      grainScheduler.setPolyStream(
          consts::synchronous,
          static_cast<int>(ECParameters[consts::STREAMS]->getModParam(width)));
    else
      grainScheduler.setPolyStream(
          consts::synchronous,
          static_cast<int>(ECParameters[consts::STREAMS]->getParam()));

    mCurrentIndex = mScanner();
    // CONTROL RATE LOOP (Executes every 4th sample)
    if (controlRateCounter == 4) {
      controlRateCounter = 0;
      mPrevModClip = mModClip;
      mModClip = static_cast<int>(ECParameters[consts::SOUND_FILE]->getModParam(
                     ECModParameters[consts::SOUND_FILE]->getWidthParam())) -
                 1;
    }
    controlRateCounter++;
    /////

    // Grain by Grain Initilization
    if (grainScheduler.trigger()) {
      prevTapeHeadVal = nowTapeHeadVal;
      nowTapeHeadVal = ECParameters[consts::SCAN_POS]->getModParam(
          ECModParameters[consts::SCAN_POS]->getWidthParam());
      prev_scan_speed = scan_speed;
      scan_speed = ECParameters[consts::SCAN_SPEED]->getModParam(
          ECModParameters[consts::SCAN_SPEED]->getWidthParam());
      prev_scan_width = scan_width;
      scan_width = ECParameters[consts::SCAN_WIDTH]->getModParam(
          ECModParameters[consts::SCAN_WIDTH]->getWidthParam());
      float frames = soundClip[mModClip]->frames;
      float start, end;

      // Case where the scanning head is given a hard reset.
      if (mPrevModClip != mModClip || mCurrentIndex == mScanner.getTarget() ||
          prevTapeHeadVal != nowTapeHeadVal) {
        start = nowTapeHeadVal * frames;
        end = start + (frames * scan_width);
        mScanner.set(start, end,
                     (end - start) / (mGlobalSamplingRate * abs(scan_speed)));
      }

      // On the fly adjustments.
      if (scan_width != prev_scan_width || scan_speed != prev_scan_speed) {
        start = mScanner.getValue();
        end = (nowTapeHeadVal * frames) + (frames * scan_width);
        mScanner.set(start, end,
                     (end - start) / (mGlobalSamplingRate * abs(scan_speed)));
      }

      // Logic for dealing with reversed buffer playthrough.
      // Note that this is caused by negative scan speed.
      if (scan_speed < 0) {
        mCurrentIndex = 2 * (nowTapeHeadVal * frames) - mCurrentIndex;
        // Wrapping logic.
        if (mCurrentIndex < 0) {
          mCurrentIndex = frames + mCurrentIndex;
        }
      }
      // Wrapping logic for when scan speed is positive.
      else {
        if (mCurrentIndex >= frames)
          mCurrentIndex = mCurrentIndex - frames;
      }


      /* Experiments For when Tape Head Changes

      if (prevTapeHeadVal != nowTapeHeadVal) {
        start = mScanner.getValue();
        end = (nowTapeHeadVal * frames) +
              ((frames - (nowTapeHeadVal * frames)) * scan_width);
        mScanner.set(start, end,
                     (end - start) / (mGlobalSamplingRate * scan_speed));
      }

      */

      /* Experiment: Quick attack to new tapehead position.
      if (prevTapeHeadVal != nowTapeHeadVal) {
        if (mScanner.getValue() < nowTapeHeadVal * frames) {
          start = mScanner.getValue();
          end = nowTapeHeadVal * frames;
          mScanner.set(start, end,
                       abs(end - start) / (mGlobalSamplingRate * scan_speed) /
                           (scan_speed * 16));
        } else {
          start = nowTapeHeadVal * frames;
          end = mScanner.getValue();
          mScanner.set(start, end,
                       abs(start - end) / (mGlobalSamplingRate * scan_speed) /
                           (scan_speed * 16));
        }
      }
      */

      auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
      if (voice) {
        grainParameters list = {
            ECParameters[consts::GRAIN_DUR],
            ECModParameters[consts::GRAIN_DUR]->getWidthParam(),
            ECParameters[consts::ENVELOPE],
            ECModParameters[consts::ENVELOPE]->getWidthParam(),
            ECParameters[consts::PITCH_SHIFT],
            ECModParameters[consts::PITCH_SHIFT]->getWidthParam(),
            ECParameters[consts::FILTER_CENTER],
            ECModParameters[consts::FILTER_CENTER]->getWidthParam(),
            ECParameters[consts::RESONANCE],
            ECModParameters[consts::RESONANCE]->getWidthParam(),
            ECParameters[consts::VOLUME],
            ECModParameters[consts::VOLUME]->getWidthParam(),
            ECParameters[consts::PAN],
            ECModParameters[consts::PAN]->getWidthParam(),
            soundClip[mModClip],
            mPActiveVoices,
            mCurrentIndex,
        };

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
    // get recent peak
    if (abs(io.out(0)) > peakL) {
      peakL = abs(io.out(0));
      peakResetL = 0;
    };
    if (abs(io.out(1)) > peakR) {
      peakR = abs(io.out(1));
      peakResetR = 0;
    };
    peakResetL += 1;
    peakResetR += 1;
    if (peakResetL > io.framesPerSecond() * 1)
      peakL = 0;
    if (peakResetR > io.framesPerSecond() * 1)
      peakR = 0;

    // Set clip vars to > 0 if clip is detected on any sample
    // 5 is the number of visual frames the meter will turn red for
    if (abs(io.out(0)) > 1.0f)
      clipL = 5;
    if (abs(io.out(1)) > 1.0f)
      clipR = 5;
    // add samples to oscilloscope ringbuffer
    oscBufferL.push_back(io.out(0));
    oscBufferR.push_back(io.out(1));
    // Add samples to VU ringbuffer (squared in advance for RMS calculations)
    vuBufferL.push_back(pow(io.out(0), 2));
    vuBufferR.push_back(pow(io.out(1), 2));
  }
}

void ecSynth::onTriggerOn() {}

void ecSynth::onTriggerOff() {}

void ecSynth::loadSoundFileRT(std::string fileName) {
  if (std::find(soundClipFileName.begin(), soundClipFileName.end(), fileName) !=
      soundClipFileName.end())
    return;
  bool temp = util::load(fileName, soundClip, mGlobalSamplingRate, true);
  if (temp) {
    soundClipFileName.push_back(fileName);
    mClipNum++;

    ECParameters[consts::SOUND_FILE]->mParameter->max(mClipNum);
    ECParameters[consts::SOUND_FILE]->mLowRange->max(mClipNum);
    ECParameters[consts::SOUND_FILE]->mHighRange->max(mClipNum);
    ECParameters[consts::SOUND_FILE]->mHighRange->set(
        mClipNum); // stylistic choice, might take out
  }
}

void ecSynth::loadSoundFileOffline(std::string fileName) {
  if (std::find(soundClipFileName.begin(), soundClipFileName.end(), fileName) !=
      soundClipFileName.end())
    return;
  bool temp = util::load(fileName, soundClip, mGlobalSamplingRate, true);
  if (temp) {
    soundClipFileName.push_back(fileName);
    mClipNum++;
  }
}

bool ecSynth::loadInitSoundFiles(std::string directory) {
  FileList initAudioFiles = fileListFromDir(directory);
  initAudioFiles.sort(util::compareFileNoCase);

  bool success = false;
  for (auto i = initAudioFiles.begin(); i != initAudioFiles.end(); i++) {
    if (i->file().substr(i->file().length() - 4) == ".wav" ||
        i->file().substr(i->file().length() - 4) == ".aif") {
      loadSoundFileOffline(i->filepath());
      success = true;
    } else if (i->file().substr(i->file().length() - 5) == ".aiff") {
      loadSoundFileOffline(i->filepath());
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
  ECParameters[consts::SOUND_FILE]->mParameter->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mLowRange->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mHighRange->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mHighRange->set(
      mClipNum); // stylistic choice, might take out

  if (static_cast<int>(ECParameters[consts::SOUND_FILE]->mParameter->get()) >=
      index)
    ECParameters[consts::SOUND_FILE]->mParameter->set(
        ECParameters[consts::SOUND_FILE]->mParameter->get() - 1);
  return true;
}

bool ecSynth::removeCurrentSoundFile() {
  removeSoundFile(ECParameters[consts::SOUND_FILE]->mParameter->get() - 1);
  return true;
}

void ecSynth::clearSoundFiles() {
  soundClip.clear();
  soundClipFileName.clear();

  mClipNum = 0;
  ECParameters[consts::SOUND_FILE]->mParameter->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mLowRange->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mHighRange->max(mClipNum);
  ECParameters[consts::SOUND_FILE]->mHighRange->set(
      mClipNum); // stylistic choice, might take out
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
    loadSoundFileRT(filePaths[i]);
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