/*    Gamma - Generic processing library
    See COPYRIGHT file for authors and license information

    Example:
    Description:
*/

#include <math.h>

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

#include "utility.h"

using namespace al;

#define SAMPLE_RATE 48000;

class StochasticCannon
{
public:

  StochasticCannon(double samplingRate) {
    mSamplingRate = samplingRate;
  }

  void setFrequency(double frequency) {
    configure(frequency, mDivergence);
  }

  void setDivergence(double divergence) {
    configure(mFrequency, divergence);
  }

  void configure(double frequency, double divergence) {
    if (divergence > 1.0) {
      divergence = 1.0;
    } else if (divergence < 0.0) {
      divergence = 0.0;
    }
    mDivergence = divergence;
    mFrequency = frequency;
    mIncrement = mFrequency/mSamplingRate;
  }

  bool tick() {
    if (mCounter >= 1.0) {
      mCounter -= 1.0;
      mCounter += rand.uniform(-mDivergence, mDivergence);
      mCounter += mIncrement;
      return true;
    }
    mCounter += mIncrement;
    return false;
  }
private:
  gam::LFO<> mPulse;
  rnd::Random<> rand;

  double mCounter {1.0};
  double mSamplingRate;
  double mDivergence {0.0};
  double mFrequency {1.0};
  double mIncrement {0.0};
};

class Grain : public SynthVoice {
public:
  // Unit generators
  gam::Sine<> mOsc;
  Array *source = nullptr;
  gam::Osc<gam::real, gam::ipl::Linear, gam::phsInc::OneShot> mGrainEnv{1.0, 0.0, 512};
  Line index;

  // Initialize voice. This function will nly be called once per voice
  virtual void init() {
    gam::tbl::hann(mGrainEnv.elems(), mGrainEnv.size());
    mGrainEnv.freq(10);
    mOsc.freq(440);
    mOsc.amp(1.0);
//    std::cout << " init grain" <<std::endl;
  }
  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      io.out(0) = source->get(index()) * mGrainEnv(); // JKilg: hard coded number as a placeholder 
      io.out(1) = source->get(index()) * mGrainEnv();
      //io.out(1) = source->get(index()) * mGrainEnv();
      //io.out(0) += mOsc() * mGrainEnv(); 
      //io.out(1) += mOsc() * mGrainEnv(); 
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

  StochasticCannon mCannon{48000};
  Parameter grainTriggerFreq {"grainTriggerFreq", "", 10.0, "", 1.0, 4000.0};
  Parameter grainTriggerDiv {"grainTriggerDiv", "", 0.0, "", 0.0, 1.0};
  Parameter grainDurationMs {"grainDurationMs", "", 10.0, "", 0.01, 1000};
  Parameter grainInternalFreq {"grainInternalFreq", "", 440.0, "", 5, 2000.0};
  Parameter attackTime {"attackTime", "", 0.3, "", 0.01, 10.0};
  Parameter sustain {"sustain", "", 0.8, "", 0.01, 10.0};
  Parameter releaseTime {"releaseTime", "", 2.0, "", 0.01, 10.0};
  Parameter amplitude {"amplitude", "", 0.4, "", 0.01, 2.0};
  Parameter position{"position", "", 0.4, "", 0, 1};
  Parameter playbackRate {"playbackRate", "", 1, "", 0.1, 2};

  gam::ADSR<> mEnv{0.3, 0.3, 0.8, 2.0};

  gam::LFO<> testLFO;

  virtual void init() {
    
    testLFO.set(1000,0.2,0.9);
    //testLFO.mod(1);

    load("pluck.aiff");

    *this << amplitude <<  attackTime << sustain << releaseTime << grainTriggerFreq << grainTriggerDiv 
    << grainDurationMs << grainInternalFreq << position << playbackRate;
    mCannon.configure(grainTriggerFreq, 0.0);
    grainTriggerFreq.registerChangeCallback([&](float value) {
      mCannon.setFrequency(value);
    });
    grainTriggerDiv.registerChangeCallback([&](float value) {
      mCannon.setDivergence(value);
    });

    attackTime.registerChangeCallback([&](float value) {
      mEnv.attack(value);
      mEnv.decay(value);
    });
    sustain.registerChangeCallback([&](float value) {
      mEnv.sustain(value);
    });
    releaseTime.registerChangeCallback([&](float value) {
      mEnv.release(value);
    });

    grainSynth.allocatePolyphony<Grain>(1024);
    grainSynth.setDefaultUserData(this);

  }
    int count = 0;
  virtual void onProcess(AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      if (mCannon.tick()) {
//        std::cout << "trigger " << io.frame() << std::endl;
        auto *voice = static_cast<Grain *>(grainSynth.getFreeVoice());
        if (voice) {
          voice->mGrainEnv.freq(1000.0/grainDurationMs.get());
          //voice->mOsc.freq(grainInternalFreq.get());
          //voice->mOsc.phase(0);
          //voice->mOsc.amp(1.0);
          rnd::Random<> rng;
          voice->source = soundClip[0];
          float modTEST = testLFO.cos();
          //std::cout << "modTEST: " << modTEST << std::endl;
          float startTime = (voice->source->size * (position.get() * modTEST));
          float endTime = startTime + (grainDurationMs.get()) * powf(2.0,playbackRate.get() * modTEST) *  SAMPLE_RATE;
          voice->index.set(startTime,endTime, grainDurationMs.get());
          grainSynth.triggerOn(voice, io.frame());
        } else {
          std::cout << "out of voices!" <<std::endl;
        }
      }
    }

    grainSynth.render(io);

    io.frame(0); 
    float amp = amplitude.get();
    while (io()) {
      //io.out(0) *= mEnv() * amp;
      //io.out(1) *= mEnv() * amp;
      
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

      size_t test;
      Array* a = new Array();
      a->size = soundFile.frames();
      a->data = new float[a->size];
      test = soundFile.read(a->data, a->size);
      soundClip.push_back(a);

      soundFile.close();
  }


private:
  PolySynth grainSynth {PolySynth::TIME_MASTER_AUDIO};
  std::vector<Array*> soundClip;
};


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
//    mRecorder.stop();
    ParameterGUI::cleanup();

  }

};


int main(){    // Create app instance

  //    app.navControl().active(false); // Disable navigation via keyboard, since we will be using keyboard for note triggering

//  std::cout << " ---***-- " << std::endl;
  MyApp app;
  // Set up audio
  app.initAudio(48000, 128, 2, 0);
  // Set sampling rate for Gamma objects from app's audio
  gam::sampleRate(app.audioIO().framesPerSecond());
  app.audioIO().print();

  app.start();
  return 0;
}
