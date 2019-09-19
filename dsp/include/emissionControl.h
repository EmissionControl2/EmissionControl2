#ifndef EMISSIONCONTROL_H 
#define EMISSIONCONTROL_H 

#include "Gamma/Oscillator.h"
#include "const.h"
#include <iostream>

using namespace al;


/**
 * Wrapper class of three envelopes:
 *  expo,  
 *  reverese expo, 
 *  and tukey.
 * 
 * Allows for a single value [0,1] to interpolate between all three envelopes. 
 */
class grainEnvelope {
public:
  
  /**
   * Set grainEnvelope parameters. 
   *  
   * @param[in] sets the duration of the envelope in SECONDS, usually equal to duration of grain.
   * @param[in] FROM 0 to 1, where 0 to 0.5 interplates between expo and tukey and 0.5 to 1 interpolated between tukey and reverse expo.
   */ 
  grainEnvelope(float duration = 1, float envelope = 0) {
    this->setDuration(duration);
    this->setEnvelope(envelope);
  }

  /**
   * Run in audio callback loop.
   */
  float operator()() {
  if(mEnvelope < 0) { //exponential envelope case 
  mEnvelope = 0;
  } else if (mEnvelope < 0.5) { //exponetial and turkey envelope interpolation
  mRExpoEnv.increment();
  return ((mExpoEnv() * (1 - mEnvelope*2)) + (mTurkeyEnv() * mEnvelope*2) );
  } else if (mEnvelope == 0.5) { //turkey envelope case 
  mRExpoEnv.increment();
  mExpoEnv.increment();
  return mTurkeyEnv();
  } else if (mEnvelope <= 1) { // turkey and reverse exponential envelope interpolation
  mExpoEnv.increment();
  return ((mTurkeyEnv() * (1 - (mEnvelope-0.5) * 2)) + (mRExpoEnv() * (mEnvelope - 0.5) * 2) );
  } 
} 

  void set(float duration, float envelope) {
    this->setDuration(duration);
    this->setEnvelope(envelope);
  }

  void setEnvelope(float envelope) {
    if(envelope > 1 ) 
      mEnvelope = 1;
    else if (envelope < 0) 
      mEnvelope = 0;
    else mEnvelope = envelope;
  }

  //  in seconds
  void setDuration(float duration) {
    if(duration <= 0 ) {
      mDuration = 1;
    } else mDuration = duration;
    mTurkeyEnv.set(duration);
    mExpoEnv.set(duration, 0);
    mRExpoEnv.set(duration, 1);
  }

  /**
   * Mark envelope as done.
   */
  bool done() {
    return mTurkeyEnv.done();
  }

  /**
   * Call to reset envelope parameters to original starting position. 
   */
  void reset() { 
    mExpoEnv.set();
    mRExpoEnv.set();
    mTurkeyEnv.set();
  }

  float getEnvelope() const {
    return mEnvelope;
  }

  float getDuration() const {
    return mDuration;
  }

private: 
  util::expo mExpoEnv;
  util::tukey mTurkeyEnv;
  util::expo mRExpoEnv;
  float mEnvelope; //assumes between 0 and 1
  float mDuration; //in seconds
};

struct grainParameters{
  float grainDurationMs;
  float envelope;
  float tapeHead;
  float playbackRate;
  util::Buffer<float>* source;
  float modValue;
};

struct modParameters{
  
};

/**
 * Grain class containing an audio buffer and an envelope. Used as the voice for the voiceScheduler class
 */
class Grain : public al::SynthVoice {
public:
  //Grain();
  int counter = 0; //USED for debugging
  
  // Initialize voice. This function will nly be called once per voice
  virtual void init() {
  }

  /**
   * Processing done at the audio rate. 
   */
  virtual void onProcess(al::AudioIOData& io) override {
    while (io()) {
      envVal = gEnv();
      sourceIndex = index();
      if(sourceIndex > source->size) 
        sourceIndex -= source->size;
      //if(counter%12 == 0)
        //std::cout << envVal << std::endl;
      io.out(0) += source->get(sourceIndex)  * envVal; 
      io.out(1) += source->get(sourceIndex)  * envVal;
      if (gEnv.done()) { 
        free();
        counter = 0;
        break;
      }
    }
  }
  
   virtual void onTriggerOn() override {
    gEnv.reset();
  }

  /**
   * Configure grain parameters before being sent to scheduler. 
   * param[in] A struct containing a list of all grain parameters that need to be set. 
   *  - Note: see struct grainParameters for details.
   */

  void configureGrain(grainParameters& list) {
    setDurationMs(list.grainDurationMs);
    gEnv.set(list.grainDurationMs/1000, list.envelope);
    this->source = list.source;

    float startSample = list.source->size * (list.tapeHead * (list.modValue + 1)/2); 
    float endSample = startSample  + (list.grainDurationMs/1000) * consts::SAMPLE_RATE * abs(list.playbackRate)/2;
    if(list.playbackRate < 0) 
      index.set(endSample,startSample, list.grainDurationMs/1000 ); 
    else 
      index.set(startSample,endSample, list.grainDurationMs/1000); 

  }

  float getDurationMs() const {return durationMs;}

  void setDurationMs(float dur) {durationMs = dur;}

private:
  util::Buffer<float> *source = nullptr;
  util::line index;
  grainEnvelope gEnv;
  float envVal, sourceIndex, tapeHead, durationMs;
};


/**
 * Wrapper class containing all unit generators needed to modulate the grain/voiceScheduler parameters/
 */
class ecModulator {
  public:

  /**
   * Constructor for ecModulator. 
   * 
   * param[in] An enum type denoting the modulator source. 
   * param[in] The frequency of the modulator. 
   * param[in] The width of the modulator.
   */
   ecModulator(consts::waveform modWaveform = consts::SINE, float frequency = 2, float width = 1) : mFrequency(frequency), mWidth(width) {
        std::cout << "ecModulator Constructor\n";
        this->setWaveform(modWaveform);
        mLFO.set(frequency, 0, 0.5); 
    }

    /**
     * Processing done at the audio rate. 
     */
    float operator()() {
        if(mModWaveform == consts::SINE) {
            return mLFO.cos() * mWidth;
        } else if(mModWaveform == consts::SAW) {
            return mLFO.tri() * mWidth; 
        } else if (mModWaveform == consts::SQUARE) {
            return mLFO.sqr() * mWidth;
        } else if (mModWaveform == consts::NOISE) {
            return rand.uniform(-1.0,1.0);
        } else {
            return mLFO.cos() * mWidth;
        }
    }

    consts::waveform getWaveform() {return mModWaveform;}
    float getFrequency() {return mFrequency;}
    float getWidth() {return mWidth;}

    void setWaveform(consts::waveform modWaveform) {
      if(modWaveform != consts::SINE && modWaveform != consts::SAW && modWaveform != consts::SQUARE && modWaveform != consts::NOISE) {
            std::cerr << "invalid waveform" << std::endl;
            return;
        }
      mModWaveform = modWaveform;
    }

    void setFrequency(float frequency) {
      mFrequency = frequency;
      mLFO.set(frequency, 0, 0.5);
    }

    void setWidth(float width) {
      mWidth = width;
    }

    void setPhase(float phase) {
      mLFO.phase(phase);
    }

    private: 
    gam::LFO<> mLFO{};
    al::rnd::Random<> rand;
    consts::waveform mModWaveform;
    float mFrequency;
    float mWidth;
  
};


/**
 * Class used to schedule the emission of an arbitrary voice. 
 */
class voiceScheduler
{
public:

  /**
   * Constructor of the voice scheduler. 
   * 
   * param[in] The audio samplingRate. 
   */
  voiceScheduler(double samplingRate) {
    mSamplingRate = samplingRate;
  }

  /** 
   * Used to configure all parameters necessary for voiceScheduler. 
   * 
   * param[in] The frequency in which voices are emitted. 
   * param[in] FROM 0 to 1; The value of the asynchronicity parameter. 
   * param[in] FROM 0 to 1; The value of the intermittence parameter.
   */
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

  /**
   * When return true, trigger a voice.
   */
  bool trigger() {
    if(mCounter >= 1.0) {
      //std::cout << "made it\n";
      mCounter -= 1.0;
      if(rand.uniform() < mIntermittence) return false;
      mCounter += rand.uniform(-mAsync, mAsync);
      mCounter += mIncrement;
      return true;
    } 
    mCounter += mIncrement;
    return false;
  }

  /** 
   * Sets frequency in which voices are emitted.
   */
  void setFrequency(double frequency) {
    configure(frequency, mAsync, mIntermittence);
  }

  /**
   * Sets the asynchronicity parameter.
   * This randomly triggers a voice between its scheduled trigger point and a percentage of one period of the scheduler.
   * 
   * param[in] FROM 0 to 1. This determines the width of the random trigger. 
   *  0 being completely synchronous and 1 being completely asynchronous.
   * 
   * DOES NOT AFFECT DENSITY OF EMISSION.
   */
  void setAsynchronicity(double async) {
    configure(mFrequency, async, mIntermittence);
  }

  /**
   * Set the intermittence parameter. 
   * This randomly drops a voice from being triggered. 
   * 
   * param[in] FROM 0 to 1. Probability of dropping voice is determined by this value.
   *  0 being a 0% chance of dropping the voice and 1 being a 100% chance of dropping the voice. 
   * 
   * AFFECTS DENSITY OF EMISSION.
   */
  void setIntermittence(double intermittence) {
    configure(mFrequency, mAsync, intermittence);
  }

  /**
   * Set the amount of voices running in parallel.
   * 
   * IN PROGRESS
   */
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