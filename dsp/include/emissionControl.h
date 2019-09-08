#ifndef EMISSIONCONTROL_H 
#define EMISSIONCONTROL_H 

#include "Gamma/Oscillator.h"
#include "const.h"
#include <iostream>

using namespace al;

struct grainParameters{
  float grainDurationMs;
  float envelope;
  float tapeHead;
  float playbackRate;
  util::Buffer<float>* source;
  float modValue;
};

class Grain : public al::SynthVoice {
public:
  // Unit generators
  //Grain();
  util::Buffer<float> *source = nullptr;
  util::Line index;
  int counter = 0;
  float envVal;

  // Initialize voice. This function will nly be called once per voice
  virtual void init() {
  }
  virtual void onProcess(al::AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      counter++;
      envVal = testExp();
      //if(counter%12 == 1)
      //  std::cout << envVal << std::endl;
      io.out(0) += source->get(index())  * envVal; // * env();
      io.out(1) += source->get(index())  * envVal; // * env();
      if (testExp.done()) { //counter == static_cast<int>(consts::SAMPLE_RATE * durationMs/1000)
        free();
        counter = 0;
        break;
      }
    }
  }
  virtual void onTriggerOn() override {
    //mGrainEnv.reset();
    env.sustainDisable();
    env.reset();
    testExp.set();
    //      mOsc.reset();
  }

  //value between 0 and 1 
  void setEnvelope(float value) {
    if(value < 0) value++;
    if(value > 1) value--;
    
    env.sustain(1);
    env.decay(durationMs/1000 * 0.4);
    env.attack(value * durationMs/1000 * 0.6);
    env.release(durationMs/1000 - env.decay() - env.attack());
  }

  float getDurationMs() const {return durationMs;}

  void setDurationMs(float dur) {durationMs = dur;}

  void configureGrain(grainParameters& list) {
    setDurationMs(list.grainDurationMs);
    setEnvelope(list.envelope);
    this->source = list.source;

    float startSample = list.source->size * (list.tapeHead * (list.modValue + 1)/2); 
    float endSample = startSample  + (list.grainDurationMs/1000) * consts::SAMPLE_RATE * abs(list.playbackRate)/2;
    if(list.playbackRate < 0) 
      index.set(endSample,startSample, list.grainDurationMs/1000 ); 
    else 
      index.set(startSample,endSample, list.grainDurationMs/1000); 

    testEnv.set(list.grainDurationMs/1000,list.envelope);
    testExp.set(list.grainDurationMs/1000,0);
  }

private:
  gam::ADSR<> env{0.001,0,1,0.01,1,-4};
  util::turkey testEnv;
  util::expo testExp;
  float durationMs;
  float tapeHead;
};






class ecModulator {
  public:
   ecModulator(consts::waveform modWaveform = consts::SINE, float frequency = 1, float width = 1) : frequency(frequency), width(width) {
        std::cout << "ecModulator Constructor\n";
        this->setWaveform(modWaveform);
        LFO.set(frequency, 0, 0.5); 
    }

    float operator()() {
        if(modWaveform == consts::SINE) {
            return LFO.cos() * width;
        } else if(modWaveform == consts::SAW) {
            return LFO.tri() * width; 
        } else if (modWaveform == consts::SQUARE) {
            return LFO.sqr() * width;
        } else if (modWaveform == consts::SQUARE) {
            return -1020020209200;
        }
        else {
            return LFO.cos() * width;
        }
    }

    consts::waveform getWaveform() {return modWaveform;}
    float getFrequency() {return frequency;}
    float getWidth() {return width;}

    void setWaveform(consts::waveform modWaveform) {
      if(modWaveform != consts::SINE && modWaveform != consts::SAW && modWaveform != consts::SQUARE && modWaveform != consts::SQUARE) {
            std::cerr << "invalid waveform" << std::endl;
            return;
        }
      this->modWaveform = modWaveform;
    }

    void setFrequency(float frequency) {
      this->frequency= frequency;
      LFO.set(frequency,0,0.5);
    }

    void setWidth(float width) {
      this->width = width;
    }

    private: 
    gam::LFO<> LFO;
    consts::waveform modWaveform;
    float frequency;
    float width;
  
};



class voiceScheduler
{
public:

  voiceScheduler(double samplingRate) {
    mSamplingRate = samplingRate;
  }

  void setFrequency(double frequency) {
    configure(frequency, mAsync, mIntermittence);
  }

  void setAsynchronicity(double async) {
    configure(mFrequency, async, mIntermittence);
  }

  void setIntermittence(double intermittence) {
    configure(mFrequency, mAsync, intermittence);
  }

  void configure(double frequency, double async, double intermittence) {
    if (async > 1.0) {
      async = 1.0;
    } else if (async < 0.0) {
      async = 0.0;
    }
    if (intermittence > 1.0) {
      intermittence = 1.0;
    } else if (intermittence < 0.0) {
      intermittence = 0.0;
    }
    mAsync = async;
    mFrequency = frequency;
    mIntermittence = intermittence;
    mIncrement = mFrequency/mSamplingRate;
  }

  bool trigger() {
    if(mCounter >= 1.0) {
      //std::cout << "made it\n";
      mCounter -= 1.0;
      if(!intermittence()) return false;
      mCounter += rand.uniform(-mAsync, mAsync);
      mCounter += mIncrement;
      return true;
    } 
    mCounter += mIncrement;
    return false;
  }

  /*
  @brief Returns true if random number generator is withing bounds 
  @param lower bound coundition
  */
  bool intermittence() {
    if(rand.uniform() > mIntermittence) return true; 
    else return false;
  }

  void polyStream(consts::streamType type, int numStreams) {
    if(type == consts::synchronous) {
      setFrequency(mFrequency * numStreams);
    } else {
      std::cerr << "Not implemented yet, please try again later.\n";
    }
  }

private:
  gam::LFO<> mPulse;
  al::rnd::Random<> rand;

  double mCounter {1.0};
  double mSamplingRate;
  double mAsync {0.0};
  double mFrequency {1.0};
  double mIncrement {0.0};
  double mIntermittence {0.0};
};

#endif