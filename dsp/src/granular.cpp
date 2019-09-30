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
  ecParameter grainRate {"grainRate", "", 1, "", 0.5, 100, consts::SINE, 0};
  ParameterMenu grainRateLFO {""}; 
  ecParameter modGrainRateWidth {"modGrainRateWidth", "", 0, "", 0, 1};
  ecParameter asynchronicity {"asynchronicity", "", 0.0, "", 0.0, 1.0, consts::SINE};
  ParameterMenu asyncLFO {" "}; // Using spaces so the ParameterGUI::drawMenu behaves correctly/
  ecParameter modAsynchronicityWidth {"modAsynchronicityWidth", "", 0, "", 0, 1};
  ecParameter intermittency {"intermittency", "", 0,"", 0, 1};
  ParameterMenu intermittencyLFO {"  "}; 
  ecParameter modIntermittencyWidth {"modIntermittencyWidth", "", 0, "", 0, 1};
  ecParameterInt streams {"streams", "", 1,"", 1, 12};
  ParameterMenu streamsLFO {"   "}; 
  ecParameter modStreamsWidth {"modStreamsWidth", "", 0, "", 0, 1};

  ecParameter grainDurationMs {"grainDurationMs", "", 25, "", 0.01, 1000};
  ParameterMenu grainDurationLFO {"    "}; 
  ecParameter modGrainDurationWidth {"modGrainDurationWidth", "", 0, "", 0, 1};
  ecParameter envelope {"envelope", "", 0.5, "", 0, 1};
  ParameterMenu envelopeLFO {"     "}; 
  ecParameter modEnvelopeWidth {"modEnvelopeWidth", "", 0, "", 0, 1};
  ecParameter tapeHead{"tapeHead", "", 0, "", 0, 1};
  ParameterMenu tapeHeadLFO {"      "}; 
  ecParameter modTapeHeadWidth {"modTapeHeadWidth", "", 0, "", 0, 1};
  ecParameter playbackRate {"playbackRate", "       ", 1, "", -2, 2};
  ParameterMenu playbackRateLFO {"        "}; 
  ecParameter modPlaybackRateWidth {"modPlaybackRateWidth", "", 0, "", 0, 1};

  ecParameter volumeDB {"volumeDB", "", -6, "", -60, 6};
  ParameterMenu volumeLFO {"         "}; 
  ecParameter modVolumeWidth {"modVolumeWidth", "", 0, "", 0, 1};
  


  ecParameter modSineFrequency {"modSineFrequency", "",1, "", 0.01, 40};
  ecParameter modSinePhase {"modSinePhase", "", 0, "", 0, 1};
  ecParameter modSquareFrequency {"modSquareFrequency", "",1,"", 0.01, 40};
  ecParameter modSquareWidth {"modSquareWidth", "",0.5,"", 0, 1};
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

  int controlRateCounter = 0;


  virtual void init() override {

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
    grainScheduler.configure(grainRate, 0.0, 0.0);

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

      // NOTE grainRate noise isnt very perceptible 
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

      if(modStreamsWidth.get() > 0) //Modulate the amount of streams playing.
        grainScheduler.setPolyStream(consts::synchronous, streams.getModParam(modSineValue, modSquareValue, modSawValue, modNoiseValue,
        modStreamsWidth.get()));
      else grainScheduler.setPolyStream(consts::synchronous, streams.get());

      // CONTROL RATE LOOP (Executes every 4th sample)
      if(controlRateCounter == 4) {
        controlRateCounter = 0;
      }
      controlRateCounter++;
      /////
      
      //Grain by Grain Initilization o
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
    float amp = powf(10,volumeDB.get()/20);
    while (io()) {
      io.out(0) *=  amp ; // this manipulates the entire stream on the channel level 
      io.out(1) *=  amp ; //* mEnv() 
      
    }
  }

  virtual void onTriggerOn() override {
  }

  virtual void onTriggerOff() override {
  }

  void loadSoundFile(std::string fileName) {
    util::load(fileName, soundClip);
  }

  void verbose(bool toggle) {
    grainSynth.verbose(toggle);
  }

  int getActiveVoices() {
    return mActiveVoices;
  }

private:
  int mActiveVoices = 0;
  int *mPActiveVoices = nullptr;
  PolySynth grainSynth {PolySynth::TIME_MASTER_AUDIO};
  std::vector<util::Buffer<float>*> soundClip;
};




