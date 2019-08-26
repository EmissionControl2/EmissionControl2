#ifndef EMISSIONCONTROL_H 
#define EMISSIONCONTROL_H 

#include "Gamma/Oscillator.h"
#include "const.h"
#include <iostream>

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