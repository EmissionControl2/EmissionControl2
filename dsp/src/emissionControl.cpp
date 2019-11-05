#include "../include/emissionControl.h"
#include "../include/utility.h"

//grainEnvelope

  /**
   * Run in audio callback loop.
   */
float grainEnvelope::operator()() {
  if(mEnvelope < 0 || mEnvelope > 1)  //exponential envelope case 
    mEnvelope = 0; 
  
  if (mEnvelope < 0.5) { //exponetial and turkey envelope interpolation
  mRExpoEnv.increment();
  return ((mExpoEnv() * (1 - mEnvelope*2)) + (mTurkeyEnv() * mEnvelope*2) );
  } else if (mEnvelope == 0.5) { //turkey envelope case 
  mRExpoEnv.increment();
  mExpoEnv.increment();
  return mTurkeyEnv();
  } else if (mEnvelope <= 1) { // turkey and reverse exponential envelope interpolation
  mExpoEnv.increment();
  return ((mTurkeyEnv() * (1 - (mEnvelope-0.5) * 2)) + (mRExpoEnv() * (mEnvelope - 0.5) * 2) );
  } else { //fails silently but gracefully
    mRExpoEnv.increment();
    mExpoEnv.increment();
    return mTurkeyEnv();
  }
} 