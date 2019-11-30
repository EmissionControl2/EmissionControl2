//ecSynth.cpp

/**** Emission Control LIB ****/
#include "ecSynth.h"
#include "utility.h"

/**** ALLOLIB ****/
#include "al/io/al_File.hpp"

/**** C STANDARD ****/
#include <stdio.h>  /* defines FILENAME_MAX */
#ifdef WINDOWS
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

using namespace al;

/**** ecSynth Implementation ****/

void ecSynth::init() {

  mPActiveVoices = &mActiveVoices;

  //MUST USE THIS ORDER
  grainRateLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  grainRateLFO.registerChangeCallback([&](int value) {
    grainRate.setWaveformIndex(value);
  });
  asyncLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  asyncLFO.registerChangeCallback([&](int value) {
    asynchronicity.setWaveformIndex(value);
  });
  intermittencyLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  intermittencyLFO.registerChangeCallback([&](int value) {
    intermittency.setWaveformIndex(value);
  });
  streamsLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  streamsLFO.registerChangeCallback([&](int value) {
    streams.setWaveformIndex(value);
  });
  grainDurationLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  grainDurationLFO.registerChangeCallback([&](int value) {
    grainDurationMs.setWaveformIndex(value);
  });
  envelopeLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  envelopeLFO.registerChangeCallback([&](int value) {
    envelope.setWaveformIndex(value);
  });
  tapeHeadLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  tapeHeadLFO.registerChangeCallback([&](int value) {
    tapeHead.setWaveformIndex(value);
  });
  playbackRateLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  playbackRateLFO.registerChangeCallback([&](int value) {
    playbackRate.setWaveformIndex(value);
  });
  volumeLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  volumeLFO.registerChangeCallback([&](int value) {
    volumeDB.setWaveformIndex(value);
  });
  soundFileLFO.setElements({"Sine", "Square", "Saw", "Noise"});
  soundFileLFO.registerChangeCallback([&](int value) {
    soundFile.setWaveformIndex(value);
  });

  grainScheduler.configure(grainRate.getParam(), 0.0, 0.0);

  modSineFrequency.mParameter->registerChangeCallback([&](float value) {
    modSine.setFrequency(value);
  });

  modSinePhase.mParameter->registerChangeCallback([&](float value) {
    modSine.setPhase(value);
  });

  modSquareFrequency.mParameter->registerChangeCallback([&](float value) {
    modSquare.setFrequency(value);
  });

  modSquareWidth.mParameter->registerChangeCallback([&](float value) {
    modSquare.setWidth(value);
  });

  modSawFrequency.mParameter->registerChangeCallback([&](float value) {
    modSaw.setFrequency(value);
  });
  modSawWidth.mParameter->registerChangeCallback([&](float value) {
    modSaw.setWidth(value);
  });

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


void ecSynth::onProcess(AudioIOData& io) {
  //        updateFromParameters();
  while (io()) {
    modSineValue = modSine(); // construct modulation value
    modSquareValue = modSquare();
    modSawValue = modSaw();
    modNoiseValue = modNoise();

    // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER

    // NOTE grainRate noise isnt very perceptible 
    if(modGrainRateWidth.getParam() > 0)  // modulate the grain rate
      grainScheduler.setFrequency(grainRate.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
      modGrainRateWidth.getParam())); 
    else grainScheduler.setFrequency(grainRate.getParam());

    if(modAsynchronicityWidth.getParam() > 0) //modulate the asynchronicity 
      grainScheduler.setAsynchronicity(asynchronicity.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
      modAsynchronicityWidth.getParam()));
    else grainScheduler.setAsynchronicity(asynchronicity.getParam());

    if(modIntermittencyWidth.getParam() > 0)  //modulate the intermittency 
      grainScheduler.setIntermittence(intermittency.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
      modIntermittencyWidth.getParam())); 
    else grainScheduler.setIntermittence(intermittency.getParam());

    if(modStreamsWidth.getParam() > 0) //Modulate the amount of streams playing.
      grainScheduler.setPolyStream(consts::synchronous, streams.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue,
      modStreamsWidth.getParam()));
    else grainScheduler.setPolyStream(consts::synchronous, streams.getParam());

    //if(modSoundFileWidth.getParam() > 0)
    //  soundFile.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue,modSoundFileWidth.getParam());

    // CONTROL RATE LOOP (Executes every 4th sample)
    if(controlRateCounter == 4) {
      controlRateCounter = 0;
      mModClip = soundFile.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue,modSoundFileWidth.getParam())-1;

    }
    controlRateCounter++;
    /////
    
    //Grain by Grain Initilization 
    if (grainScheduler.trigger()) {
      auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
      if (voice) {
        grainParameters list = {
          grainDurationMs,
          modGrainDurationWidth.getParam(),
          envelope,
          modEnvelopeWidth.getParam(),
          tapeHead,
          modTapeHeadWidth.getParam(),
          playbackRate,
          modPlaybackRateWidth.getParam(),
          soundClip[mModClip], 
          modSineValue,
          modSquareValue,
          modSawValue,
          modNoiseValue,
          mPActiveVoices
        };

        voice->configureGrain(list);
        
        mActiveVoices++; 
        grainSynth.triggerOn(voice, io.frame());

      } else {
        std::cout << "out of voices!" <<std::endl;
      }
    }
  }

  grainSynth.render(io);

  io.frame(0); 
  float amp = powf(10,volumeDB.getParam()/20);
  while (io()) {
    io.out(0) *=  amp ; // this manipulates the entire stream on the channel level 
    io.out(1) *=  amp ; //* mEnv() 
    
  }
}

void ecSynth::onTriggerOn() {
}

void ecSynth::onTriggerOff() {

}

void ecSynth::loadSoundFile(std::string fileName) {
    bool temp = util::load(fileName, soundClip);
    if(temp) {
      mClipNum++;
      soundFile.mParameterInt->max(mClipNum);
      soundFile.mLowRange->max(mClipNum);
      soundFile.mHighRange->max(mClipNum);
      soundFile.mHighRange->set(mClipNum); // stylistic choice, might take out
    }
}

void ecSynth::loadInitSoundFiles() {
  std::string execPath = util::getExecutablePath();
  File f(execPath);
  std::string initDir = f.directory(execPath) + "samples/";
  FileList initAudioFiles = fileListFromDir(initDir);
  for(auto i = initAudioFiles.begin(); i != initAudioFiles.end(); i++) {
    if(i->file().substr(i->file().length() - 4) == ".wav" || i->file().substr(i->file().length() - 4) == ".aif" ) {
      loadSoundFile(i->filepath());
    } else if(i->file().substr(i->file().length() - 5) == ".aiff") {
      loadSoundFile(i->filepath());
    }
  }

}

/**** TO DO TO DO TO DO ****/
void ecSynth::throttle(float time, float ratio) {
  if(mCounter < time * consts::SAMPLE_RATE) {
    mCounter++;
    mAvgActiveVoices += mActiveVoices;
    return ;
  } else {
    mCounter++;
    mAvgActiveVoices /= mCounter;
    mCounter = 0; 
  }

  float adaptThresh;

  if(mPeakCPU > adaptThresh) {
    
  } 
  if(mAvgCPU > adaptThresh) {
    
  } else {
    
  }
}