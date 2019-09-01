//C STD Includes
#include <math.h>

//AlloLib Includes 
#include "al_ext/soundfile/al_SoundfileBuffered.hpp"
#include "al_ext/soundfile/al_SoundfileBufferedRecord.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"
#include "al_ext/soundfile/al_SoundfileRecordGUI.hpp"

#include "Gamma/Oscillator.h"
#include "Gamma/Types.h"
#include "Gamma/Envelope.h"

#include "al/core.hpp"
#include "al/core/app/al_App.hpp"
#include "al/core/graphics/al_Shapes.hpp"
#include "al/util/ui/al_Parameter.hpp"
#include "al/util/scene/al_PolySynth.hpp"
#include "al/util/scene/al_SynthSequencer.hpp"
#include "al/util/ui/al_ControlGUI.hpp"
#include "al/core/math/al_Random.hpp"


//Local Includes
#include "../include/utility.h"
#include "../include/const.h"
#include "../include/emissionControl.h"

//Externals

using namespace al;
class Granular : public al::SynthVoice {
public:

  voiceScheduler grainScheduler{SAMPLE_RATE};
  Parameter grainTriggerFreq {"grainTriggerFreq", "", 1, "", 0.5, 40};
  Parameter grainTriggerDiv {"grainTriggerDiv", "", 0.0, "", 0.0, 1.0};
  Parameter grainDurationMs {"grainDurationMs", "", 1000.0, "", 0.01, 10000};
  Parameter skew {"skew", "", 0.5, "", 0, 1};
  Parameter attackTime {"attackTime", "", 0.3, "", 0, 1};
  Parameter decayTime {"decayTime", "", 0.8, "", 0.01, 1};
  Parameter releaseTime {"releaseTime", "", 2.0, "", 0.001, 10.0};
  Parameter volumedB {"volumedB", "", -6, "", -60, 6};
  Parameter position{"position", "", 0, "", 0, 1};
  Parameter playbackRate {"playbackRate", "", 1, "", -1, 1};
  Parameter positionModFreq {"positionModFreq", "mod", 1,"", 0.01, 30};
  Parameter intermittency {"intermittency", "", 0,"", 0, 1};
  //gam::LFO<> testLFO;
  //ecModulator test1{"SINE", 1,1};
  ecModulator positionMod {"TRI"};

  // ~Granular() {
  //   for(auto i = soundClip.begin(); i != soundClip.end(); i++) 
  //     delete[] *i;
  // }


  virtual void init() {
    //testLFO.set(1,0,0.5);
    //testLFO.mod(1);

    /// TESTING 
    ///////
    load("pluck.aiff", soundClip);

    *this << volumedB << skew <<  attackTime << decayTime << releaseTime << grainTriggerFreq << grainTriggerDiv << intermittency
    << grainDurationMs << position << playbackRate;
    

    grainScheduler.configure(grainTriggerFreq, 0.0, 0.0);
    grainTriggerFreq.registerChangeCallback([&](float value) {
      grainScheduler.setFrequency(value);
    });
    grainTriggerDiv.registerChangeCallback([&](float value) {
      grainScheduler.setAsynchronicity(value);
    });

    intermittency.registerChangeCallback([&](float value) {
      grainScheduler.setIntermittence(value);
    });

    positionModFreq.registerChangeCallback([&](float value) {
      positionMod.setFrequency(value);
    });

    grainDurationMs.registerChangeCallback([&](float value) {
      positionMod.setFrequency(value);
    });

    grainSynth.allocatePolyphony<Grain>(1024);
    grainSynth.setDefaultUserData(this);
  }

  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      //audio rate
      if (grainScheduler.tick()) {
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          //voice->mGrainEnv.freq(1000.0/grainDurationMs.get());
          voice->setDurationMs(grainDurationMs);
          voice->source = soundClip[0];
          voice->setSkew(skew.get());
          float startSample = voice->source->size * (position.get()); 
          float endSample = startSample  + (grainDurationMs.get()/1000) * SAMPLE_RATE * abs(playbackRate.get())/2;
          // std::cout << "Start Sample: " << startSample << "...End Sample: " << endSample << "...grainTIME: " 
          // << grainDurationMs.get()/1000 * 1/abs(playbackRate.get()) <<  std::endl;
          //voice->env.attack(attackTime);
          //voice->env.release(releaseTime);
          if(playbackRate.get() < 0) 
            voice->index.set(endSample,startSample, grainDurationMs.get()/1000); 
          else 
            voice->index.set(startSample,endSample, grainDurationMs.get()/1000); 
    
          grainSynth.triggerOn(voice, io.frame());

        } else {
          std::cout << "out of voices!" <<std::endl;
        }
      }
    }

    grainSynth.render(io);

    io.frame(0); 
    float amp = powf(10,volumedB.get()/20);
    while (io()) {
      io.out(0) *=  amp ; // this manipulates the entire stream on the channel level 
      io.out(1) *=  amp ; //* mEnv() 
      
    }
    //if (mEnv.done()) {free();}
  }

  virtual void onTriggerOn() override {
   grainScheduler.setFrequency(grainTriggerFreq);
   grainScheduler.setAsynchronicity(grainTriggerDiv);
   // std::cout << grainTriggerFreq.get() << " --- " << sustain.get() <<std::endl;
  }

  virtual void onTriggerOff() override {
  }

  void loadSoundFile(std::string fileName) {
    util::load(fileName, soundClip);
  }

private:
  PolySynth grainSynth {PolySynth::TIME_MASTER_AUDIO};
  std::vector<util::Buffer<float>*> soundClip;
};


//NEED to figure out how to have multiple GUI Windows
class Modulator : public SynthVoice {
public:
  Parameter positionModFreq {"positionModFreq", "mod", 1,"", 0.01, 30};
  ecModulator positionMod {"TRI"};

  virtual void init() {
    *this << positionModFreq;
  }
private:
};
//


