//C STD Includes
#include <math.h>

//AlloLib Includes 
#include "al_ext/soundfile/al_SoundfileBuffered.hpp"
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
#include "../external/r8brain-free-src/CDSPResampler.h"

using namespace al;


class Grain : public SynthVoice {
public:
  // Unit generators
  Buffer<double> *source = nullptr;
  gam::Osc<gam::real, gam::ipl::Linear, gam::phsInc::OneShot> mGrainEnv{1.0, 0.0, 512};
  gam::ADSR<> env{0.01,0,1,0.01,1,1};
  Line index;

  // Initialize voice. This function will nly be called once per voice
  virtual void init() {
    gam::tbl::hann(mGrainEnv.elems(), mGrainEnv.size());
    mGrainEnv.freq(10);
  }
  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      io.out(0) += source->get(index())  * mGrainEnv(); //this manipulates on grain level
      io.out(1) += source->get(index())  * mGrainEnv();
      if (mGrainEnv.done()) {
        free();
        break;
      }
    }
  }

  virtual void onTriggerOn() override {
    mGrainEnv.reset();
    //      mOsc.reset();
  }
};

class Granular : public SynthVoice {
public:

  StochasticCannon mCannon{SAMPLE_RATE};
  Parameter grainTriggerFreq {"grainTriggerFreq", "", 10.0, "", 0.2, 4000.0};
  Parameter grainTriggerDiv {"grainTriggerDiv", "", 0.0, "", 0.0, 1.0};
  Parameter grainDurationMs {"grainDurationMs", "", 10.0, "", 0.01, 10000};
  Parameter attackTime {"attackTime", "", 0.3, "", 0, 1};
  Parameter sustain {"sustain", "", 0.8, "", 0.01, 10.0};
  Parameter releaseTime {"releaseTime", "", 2.0, "", 0.001, 10.0};
  Parameter volumedB {"volumedB", "", -6, "", -60, 6};
  Parameter position{"position", "", 0.4, "", 0, 1};
  Parameter playbackRate {"playbackRate", "", 0, "", -1, 1};
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

    load("pluck.aiff");

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
    int count = 0;
  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      //audio rate
      float posMod = positionMod();
      if (mCannon.tick()) {
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          voice->mGrainEnv.freq(1000.0/grainDurationMs.get());
          rnd::Random<> rng;
          voice->source = soundClip[0];
          float startSample = (voice->source->size * (position.get() * posMod)); //* posMod
          float endSample = startSample  // + 24000; why is half the sampling rate the move? 
            + (grainDurationMs.get()/1000) * SAMPLE_RATE * abs(playbackRate.get())/2; //
          //std::cout << "Start Sample: " << startSample << "...End Sample: " << endSample << "...grainTIME: " << grainDurationMs.get() <<  std::endl;
          voice->env.attack(attackTime);
          voice->env.release(releaseTime);
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

  //JKilg
  void load(std::string fileName) {
      SearchPaths searchPaths;
      // searchPaths.addSearchPath("../../samples");
      searchPaths.addSearchPath("/Users/jkilgore/Applications/allo/EmissionControlPort/samples");
      //searchPaths.print();
  
      std::string filePath = searchPaths.find(fileName).filepath(); //JKilg currently debugging.
      gam::SoundFile soundFile;
      soundFile.path(filePath);

      if (!soundFile.openRead()) {
        std::cout << "We could not read " << fileName << "!" << std::endl;
        exit(1);
      }
      if (soundFile.channels() != 1) {
        std::cout << fileName << " is not a mono file" << std::endl;
        exit(1);
      }

      Buffer<double>* a = new Buffer<double>();
      a->size = soundFile.frames();
      a->data = new double[a->size];
      soundFile.read(a->data, a->size);
      
      // STILL NEED TO TEST IF THIS WORKS AND NEED TO LINK IT
      if(soundFile.frameRate() != SAMPLE_RATE) {
        // Buffer<double>* b = new Buffer<double>();
        // b->data = new double[a->size/soundFile.frameRate() * SAMPLE_RATE];
        // r8b::CDSPResampler convertSampleRate(soundFile.frameRate(),double(SAMPLE_RATE), a->size);
        // convertSampleRate.process(a->data,a->size,b->data);
        // soundClip.push_back(b);
        // delete[] a->data;

      } else soundClip.push_back(a);

    soundFile.close();
  }


private:
  PolySynth grainSynth {PolySynth::TIME_MASTER_AUDIO};
  std::vector<Buffer<double>*> soundClip;
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
  PolySynth grainSynth {PolySynth::TIME_MASTER_AUDIO};
};
//


// We make an app.
class MyApp : public App
{
public:


  // GUI manager for OscEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<Granular> synthManager {"granular"};


  OutputRecorder mRecorder;

  virtual void onInit( ) override {

//    mRecorder.start("output.wav", audioIO().framesPerSecond(), 2);
    // Append recorder to audio IO object. This will run the recorder
    // after running the onSound() function below
    audioIO().append(mRecorder);
    synthManager.setCurrentTab(2); // Run constant synth note.

    synthManager.triggerOn();
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

    // Draw GUI
    ParameterGUI::beginDraw();
    ParameterGUI::beginPanel(synthManager.name());

    synthManager.drawSynthWidgets();
    ParameterGUI::endPanel();
    ParameterGUI::beginPanel("recorder");
    SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
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


int main(){    // Create app instance

  //    app.navControl().active(false); // Disable navigation via keyboard, since we will be using keyboard for note triggering

  MyApp app;
  // Set up audio
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, AUDIO_OUTS, DEVICE_NUM);
  // Set sampling rate for Gamma objects from app's audio
  gam::sampleRate(app.audioIO().framesPerSecond());
  app.audioIO().print();

  app.start();
  return 0;
}
