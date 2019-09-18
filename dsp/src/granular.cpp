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

  voiceScheduler grainScheduler{consts::SAMPLE_RATE};
  Parameter grainRate {"grainRate", "", 1, "", 0.5, 100};
  Parameter asynchronicity {"asynchronicity", "", 0.0, "", 0.0, 1.0};
  Parameter grainDurationMs {"grainDurationMs", "", 25, "", 0.01, 1000};
  Parameter envelope {"envelope", "", 0, "", 0, 1};
  Parameter volumedB {"volumedB", "", -6, "", -60, 6};
  Parameter tapeHead{"tapeHead", "", 0, "", 0, 1};
  Parameter playbackRate {"playbackRate", "", 1, "", -1, 1};
  Parameter intermittency {"intermittency", "", 0,"", 0, 1};
  ParameterInt streams {"streams", "", 1,"", 1, 12};
  Parameter modSineFrequency {"modSineFrequency", "",1, "", 0.01, 40};
  Parameter modSinePhase {"modSinePhase", "", 0, "", 0, 1};
  Parameter modGrainRateWidth {"modGrainRateWidth", "", 0, "", 0, 1};
  Parameter modAsynchronicityWidth {"modAsynchronicityWidth", "", 0, "", 0, 1};
  Parameter modIntermittencyWidth {"modIntermittencyWidth", "", 0, "", 0, 1};
  //test
  ecModulator mod {consts::SINE, 1,1};
  //test

  ecModulator modSine {consts::SINE,2};
  ecModulator modSquare {consts::SQUARE};
  ecModulator modSaw {consts::SAW};
  ecModulator modNoise {consts::NOISE};

  grainParameters list;
  float sineModValue;

  //testLFO.set(2,0,0.5);
  // ~Granular() {
  //   for(auto i = soundClip.begin(); i != soundClip.end(); i++) 
  //     delete[] *i;
  // }


  virtual void init() {

    /// TESTING 
    ///////
    load("noise.aiff", soundClip);

    *this << volumedB << streams << grainRate << asynchronicity << intermittency
    << envelope << grainDurationMs << tapeHead << playbackRate << modSineFrequency << modSinePhase 
    << modGrainRateWidth << modAsynchronicityWidth << modIntermittencyWidth;
    

    grainScheduler.configure(grainRate, 0.0, 0.0);
    grainRate.registerChangeCallback([&](float value) {
      grainScheduler.setFrequency(value);
    });
    asynchronicity.registerChangeCallback([&](float value) {
      grainScheduler.setAsynchronicity(value);
    });

    intermittency.registerChangeCallback([&](float value) {
      grainScheduler.setIntermittence(value);
    });

    streams.registerChangeCallback([&](float value) {
      grainScheduler.polyStream(consts::synchronous, value);
    });

    modSineFrequency.registerChangeCallback([&](float value) {
      modSine.setFrequency(value);
    });

    modSinePhase.registerChangeCallback([&](float value) {
      modSine.setPhase(value);
    });

    grainSynth.allocatePolyphony<Grain>(1024);
    grainSynth.setDefaultUserData(this);
  }

  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      sineModValue = modSine(); // construct sine modulation value

      // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER

      if(modGrainRateWidth.get() > 0)  // modulate the grain rate
        grainScheduler.setFrequency(grainRate.get() * ((sineModValue * modGrainRateWidth.get()) + 1) ); 
      else grainScheduler.setFrequency(grainRate.get());

      if(modAsynchronicityWidth.get() > 0) //modulate the asynchronicity 
        grainScheduler.setAsynchronicity(asynchronicity.get() * ((sineModValue * modAsynchronicityWidth.get()) + 1) );
      else grainScheduler.setAsynchronicity(asynchronicity.get());

      if(modIntermittencyWidth.get() > 0)  //modulate the intermittency 
        grainScheduler.setIntermittence(intermittency.get() * ((sineModValue * modIntermittencyWidth.get())  + 1 ) ); //still figuring out math 
      else grainScheduler.setIntermittence(intermittency.get());
    

      if (grainScheduler.trigger()) {
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          list = {
            grainDurationMs.get(),
            envelope.get(),
            tapeHead.get(),
            playbackRate.get(),
            soundClip[0], 
            sineModValue,
          };

          voice->configureGrain(list);
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
    // (mEnv.done()) {free();}
  }

  virtual void onTriggerOn() override {
   //grainScheduler.setFrequency(grainRate);
   //grainScheduler.setAsynchronicity(asynchronicity);
   // std::cout << grainRate.get() << " --- " << sustain.get() <<std::endl;
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
  Parameter tapeHeadFreq {"tapeHeadFreq", "mod", 1,"", 0.01, 30};
  ecModulator tapeHead {consts::SAW};

  virtual void init() {
    *this << tapeHeadFreq;
  }
private:
};



