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



class Granular : public SynthVoice {
public:

  StochasticCannon mCannon{SAMPLE_RATE};
  Parameter grainTriggerFreq {"grainTriggerFreq", "", 1, "", 0.2, 4000.0};
  Parameter grainTriggerDiv {"grainTriggerDiv", "", 0.0, "", 0.0, 1.0};
  Parameter grainDurationMs {"grainDurationMs", "", 1000.0, "", 0.01, 10000};
  Parameter attackTime {"attackTime", "", 0.3, "", 0, 1};
  Parameter sustain {"sustain", "", 0.8, "", 0.01, 10.0};
  Parameter releaseTime {"releaseTime", "", 2.0, "", 0.001, 10.0};
  Parameter volumedB {"volumedB", "", -6, "", -60, 6};
  Parameter position{"position", "", 0, "", 0, 1};
  Parameter playbackRate {"playbackRate", "", 1, "", -1, 1};
  Parameter positionModFreq {"positionModFreq", "mod", 1,"", 0.01, 30};

  gam::ADSR<> mEnv{0.3, 0.3, 0.8, 2.0};

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
    load("costco-list.wav", soundClip);

    *this << volumedB <<  attackTime << sustain << releaseTime << grainTriggerFreq << grainTriggerDiv 
    << grainDurationMs << position << playbackRate;
    

    mCannon.configure(grainTriggerFreq, 0.0);
    grainTriggerFreq.registerChangeCallback([&](float value) {
      mCannon.setFrequency(value);
    });
    grainTriggerDiv.registerChangeCallback([&](float value) {
      mCannon.setDivergence(value);
    });

    attackTime.registerChangeCallback([&](float value) {
      mEnv.attack(value*grainDurationMs.get());
      mEnv.decay(value*grainDurationMs.get());
    });
    sustain.registerChangeCallback([&](float value) {
      mEnv.sustain(value);
    });
    releaseTime.registerChangeCallback([&](float value) {
      mEnv.release(value);
    });

    positionModFreq.registerChangeCallback([&](float value) {
      positionMod.setFrequency(value);
    });

    grainSynth.allocatePolyphony<Grain>(1024);
    grainSynth.setDefaultUserData(this);
  }

  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      //audio rate
      if (mCannon.tick()) {
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          voice->mGrainEnv.freq(1000.0/grainDurationMs.get());
          voice->source = soundClip[0];
          float startSample = voice->source->size * (position.get()); 
          float endSample = startSample  
            + (grainDurationMs.get()/1000) * SAMPLE_RATE * 1/powf(2.0, 1); //
          std::cout << "Start Sample: " << startSample << "...End Sample: " << endSample << "...grainTIME: " 
          << grainDurationMs.get()/1000 * 1/abs(playbackRate.get()) <<  std::endl;
          voice->env.attack(attackTime);
          voice->env.release(releaseTime);
          if(playbackRate.get() < 0) 
            voice->index.set(endSample,startSample, grainDurationMs.get()/1000); 
          else 
            voice->index.set(startSample,endSample, 1); 
    
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
    if (mEnv.done()) {free();}
  }

  virtual void onTriggerOn() override {
   mCannon.setFrequency(grainTriggerFreq);
   mCannon.setDivergence(grainTriggerDiv);
   mEnv.attack(attackTime);
   mEnv.decay(attackTime);
   mEnv.sustain(sustain);
   mEnv.release(releaseTime);
   std::cout << grainTriggerFreq.get() << " --- " << sustain.get() <<std::endl;
    mEnv.reset();

  }

  virtual void onTriggerOff() override {
    mEnv.triggerRelease();
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


// We make an app.
class MyApp : public App
{
public:


  // GUI manager for OscEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<Granular> synthManager {"GranularVoices"};
  SynthGUIManager<Modulator> synthManagerMod {"mods"};



  OutputRecorder mRecorder;

  virtual void onInit( ) override {

//    mRecorder.start("output.wav", audioIO().framesPerSecond(), 2);
    // Append recorder to audio IO object. This will run the recorder
    // after running the onSound() function below
    audioIO().append(mRecorder);
    synthManager.setCurrentTab(2); // Run constant synth note.
    synthManagerMod.setCurrentTab(2); // Run constant synth note

    synthManager.triggerOn();
    synthManagerMod.triggerOn();
  }

  virtual void onCreate() override {
    ParameterGUI::initialize();

    // Play example sequence. Comment this line to start from scratch
    //        synthManager.synthSequencer().playSequence("synth2.synthSequence");
    synthManager.synthRecorder().verbose(true);
    //        std::cout << " ----- " << std::endl;

  }

  virtual void onSound(AudioIOData &io) override {
    synthManager.render(io); // Render audio
  }

  virtual void onDraw(Graphics &g) override {
    g.clear();
    synthManager.render(g);
    synthManagerMod.render(g);

    // Draw GUI
    ParameterGUI::beginDraw();
    ParameterGUI::beginPanel(synthManager.name(),600,-1);
    synthManager.drawFields();
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Presets",400,200);
    synthManager.drawPresets();
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("recorder");
    SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
    ParameterGUI::endPanel();




    
    ParameterGUI::beginPanel(synthManagerMod.name(),-1,-1);
    synthManagerMod.drawSynthWidgets();
    ParameterGUI::endPanel();
    ParameterGUI::endDraw();

  }

  virtual void onKeyDown(Keyboard const& k) override {
    if (ParameterGUI::usingKeyboard()) { //Ignore keys if GUI is using them
      return;
    }
    if (k.shift()) {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    } else {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());

      synthManager.voice()->grainTriggerFreq.set( ::powf (2.0f, (midiNote - 69.f)/12.f) * 432.f);
      if (midiNote > 0) {
        synthManager.triggerOn(midiNote);
      }
    }
  }

  virtual void onKeyUp(Keyboard const& k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.synth().triggerOff(midiNote);
    }
  }

  void onExit() override {
    // mRecorder.stop();
    ParameterGUI::cleanup();
    

  }

};