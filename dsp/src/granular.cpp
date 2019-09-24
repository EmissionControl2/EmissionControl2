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
  ecParameter grainRate {"grainRate", "", 1, "", 0.5, 1000, consts::SINE, 0};
  ecParameter modGrainRateWidth {"modGrainRateWidth", "", 0, "", 0, 1};
  ecParameter asynchronicity {"asynchronicity", "", 0.0, "", 0.0, 1.0, consts::SINE};
  ecParameter modAsynchronicityWidth {"modAsynchronicityWidth", "", 0, "", 0, 1};
  ecParameter intermittency {"intermittency", "", 0,"", 0, 1};
  ecParameter modIntermittencyWidth {"modIntermittencyWidth", "", 0, "", 0, 1};
  ParameterInt streams {"streams", "", 1,"", 1, 12};
  ecParameter modStreamsWidth {"modStreamsWidth", "", 0, "", 0, 1};

  ecParameter grainDurationMs {"grainDurationMs", "", 25, "", 0.01, 1000};
  ecParameter modGrainDurationWidth {"modGrainDurationWidth", "", 0, "", 0, 1};
  ecParameter envelope {"envelope", "", 0.5, "", 0, 1};
  ecParameter modEnvelopeWidth {"modEnvelopeWidth", "", 0, "", 0, 1};
  ecParameter tapeHead{"tapeHead", "", 0, "", 0, 1};
  ecParameter modTapeHeadWidth {"modTapeHeadWidth", "", 0, "", 0, 1};
  ecParameter playbackRate {"playbackRate", "", 1, "", -2, 2};
  ecParameter modPlaybackRateWidth {"modPlaybackRateWidth", "", 0, "", 0, 1};

  ecParameter volumedB {"volumedB", "", -6, "", -60, 6};
  ecParameter modVolumeWidth {"modVolumeWidth", "", 0, "", 0, 1};
  


  ecParameter modSineFrequency {"modSineFrequency", "",1, "", 0.01, 40};
  ecParameter modSinePhase {"modSinePhase", "", 0, "", 0, 1};
  ecParameter modSquareFrequency {"modSquareFrequency", "",1,"", 0.01, 40};
  ecParameter modSquareWidth {"modSquareWidth", "",1,"", 0, 1};
  ecParameter modSawFrequency {"modSawFrequency", "",1,"", 0.01, 40};
  ecParameter modSawWidth {"modSawWidth", "",1,"", 0, 1};
  
  //test
  ecModulator mod {consts::SINE, 1,1};
  //test

  ecModulator modSine {consts::SINE, 1, 1};
  ecModulator modSquare {consts::SQUARE};
  ecModulator modSaw {consts::SAW};
  ecModulator modNoise {consts::NOISE};

  //grainParameters list;
  float modSineValue, modSquareValue, modSawValue, modNoiseValue;


  virtual void init() override {


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

    modSquareFrequency.registerChangeCallback([&](float value) {
      modSquare.setFrequency(value);
    });

    modSquareWidth.registerChangeCallback([&](float value) {
      modSquare.setWidth(value);
    });

    modSawFrequency.registerChangeCallback([&](float value) {
      modSaw.setFrequency(value);
    });
    modSawWidth.registerChangeCallback([&](float value) {
      modSaw.setWidth(value);
    });

    grainSynth.allocatePolyphony<Grain>(1024);
    grainSynth.setDefaultUserData(this);
  }

  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      modSineValue = modSine(); // construct modulation value
      modSquareValue = modSquare();
      modSawValue = modSaw();
      modNoiseValue = modNoise();

      // THIS IS WHERE WE WILL MODULATE THE GRAIN SCHEDULER
      
      
      if(modGrainRateWidth.get() > 0)  // modulate the grain rate
        grainScheduler.setFrequency(grainRate.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
        modGrainRateWidth.get())); 
      else grainScheduler.setFrequency(grainRate.get());

      if(modAsynchronicityWidth.get() > 0) //modulate the asynchronicity 
        grainScheduler.setAsynchronicity(asynchronicity.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
        modAsynchronicityWidth.get()));
      else grainScheduler.setAsynchronicity(asynchronicity.get());

      if(modIntermittencyWidth.get() > 0)  //modulate the intermittency 
        grainScheduler.setIntermittence(intermittency.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue, 
        modIntermittencyWidth.get())); 
      else grainScheduler.setIntermittence(intermittency.get());
    

      if (grainScheduler.trigger()) {
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          grainParameters list = {
            grainDurationMs,
            modGrainDurationWidth.get(),
            envelope,
            modEnvelopeWidth.get(),
            tapeHead,
            modTapeHeadWidth.get(),
            playbackRate,
            modPlaybackRateWidth.get(),
            soundClip[0], 
            modSineValue,
            modSquareValue,
            modSawValue,
            modNoiseValue
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




