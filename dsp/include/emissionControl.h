#ifndef EMISSIONCONTROL_H 
#define EMISSIONCONTROL_H 

#include "Gamma/Oscillator.h"
#include "const.h"
#include <iostream>


class Grain : public al::SynthVoice {
public:
  // Unit generators
  //Grain();
  //gam::Osc<gam::real, gam::ipl::Linear, gam::phsInc::OneShot> mGrainEnv{1.0, 0.0, 512};
  util::Buffer<float> *source = nullptr;
  util::Line index;

  // Initialize voice. This function will nly be called once per voice
  virtual void init() {
    //gam::tbl::hann(mGrainEnv.elems(), mGrainEnv.size());
    //mGrainEnv.freq(10);
  }
  virtual void onProcess(al::AudioIOData& io) override {
    //        updateFromParameters();
    while (io()) {
      // std::cout << env() << std::endl;
      io.out(0) += source->get(index())  * env(); //this manipulates on grain level
      io.out(1) += source->get(index())  * env();
      if (env.done()) {
        std::cout << "Made it\n";
        free();
        break;
      }
    }
  }
  virtual void onTriggerOn() override {
    //mGrainEnv.reset();
    std::cout << env.attack() << std::endl;
    env.reset();
    //      mOsc.reset();
  }

  //value between 0 and 1 
  void setSkew(float value) {
    if(value < 0) value++;
    if(value > 1) value--;
    
    env.sustain(durationMs/1000 * 0.5);
    env.attack(value * durationMs/1000 * 0.5);
    env.release(durationMs/1000 - env.sustain() - env.attack());
  }

  float getDurationMs() const {return durationMs;}

  void setDurationMs(float dur) {durationMs = dur;}

private:
  gam::ADSR<> env{0.001,0,1,0.01,1,-4};
  float durationMs;
  float startPosition;
};

class ecModulator {
  public:
   ecModulator(std::string waveform = "SINE", float frequency = 1, float width = 1) : frequency(frequency), width(width) {
        std::cout << "ecModulator Constructor\n";
        this->setWaveform(waveform);
        LFO.set(frequency, 0, 0.5); 
    }

    float operator()() {
        if(waveform == "SINE") {
            return LFO.cos() * width;
        } else if(waveform == "TRI") {
            return LFO.tri() * width; 
        } else if (waveform == "SQUARE") {
            return LFO.sqr() * width;
        } else {
            return LFO.cos() * width;
        }
    }

    std::string getWaveform() {return waveform;}
    float getFrequency() {return frequency;}
    float getWidth() {return width;}

    void setWaveform(std::string waveform) {
      if(waveform != "SINE" && waveform != "TRI" && waveform != "SQUARE") {
            std::cerr << "invalid waveform" << std::endl;
            return;
        }
      this->waveform = waveform;
    }

    void setFrequency(float frequency) {
      this->frequency= frequency;
      LFO.set(frequency,0,0.5);
    }

    void setWidth(float width) {
      this->width = width;
    }

    private: 
    gam::LFO<> LFO;;
    std::string waveform;
    float frequency;
    float width;
  
};



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
  al::rnd::Random<> rand;

  double mCounter {1.0};
  double mSamplingRate;
  double mDivergence {0.0};
  double mFrequency {1.0};
  double mIncrement {0.0};
};

#endif