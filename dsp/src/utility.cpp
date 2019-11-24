#include "utility.h"

using namespace util;

/**** line Class Implementation ****/

float line::operator()() {
  if (value != target) {
    value += increment;
    if ((increment < 0) ? (value < target) : (value > target)) value = target;
  }
  return value;
}

void line::set(float v, float t, float s) {
  value = v;
  target = t;
  seconds = s;
  if (seconds <= 0) seconds = 1 / consts::SAMPLE_RATE;
  increment = (target - value) / (seconds * consts::SAMPLE_RATE);
}

/**** expo Class Implementation ****/
float expo::operator()() {
  if (!mReverse) {
    if (mX < mThresholdX * 0.01) {  // ratio of initial ramp up to 1
      mY = powf(M_E, 100 * mX - mThresholdX);  // bias needed to reach that
                                                // value in time (SEE DESMOS)
      mX += mIncrementX;
    } else if (mX < mThresholdX) {
      mY = powf(M_E,
                -1 * mX + (mThresholdX *
                            0.01));  // this compensates for initial ramp up
      mX += mIncrementX;
    } else {
      mY = mThresholdY;
      mX = 0;
    }
  } else {  // reversed Logic
    if (mX < mThresholdX * 0.92761758634) {
      mY = powf(M_E,
                0.9 * (mX - mThresholdX +
                        0.5));  // (mx - thresh + bias ) where bias determines
                                // the ratio of envelope (mThresholdX * ratio)
      mX += mIncrementX;
    } else if (mX <
                mThresholdX * 0.95) {  // small sustain to makeup for percieved
                                      // volume loss (in relation to expodec)
      mY = 1;
      mX += mIncrementX;
    } else if (mX < mThresholdX) {  // quickly bring envelope down to zero
                                    // before marking as done.
      mY = powf(M_E, -20 * ((mX) - (mThresholdX * 0.95)));
      mX += mIncrementX;
    } else {
      mY = mThresholdY;
      mX = 0;
    }
  }
  return mY;
}

void expo::set() {
  if (mTotalS <= 0) mTotalS = 1;
  if (mThresholdY <= 0)
    mThresholdY = 0.001, mThresholdX = -1 * std::log(0.001);
  mX = 0;
  mY = mThresholdY;
  mIncrementX = (mThresholdX / mTotalS);
}

void expo::set(float seconds, bool reverse, float threshold) {
  mTotalS = seconds * consts::SAMPLE_RATE;
  mReverse = reverse;
  mThresholdY = threshold;
  mThresholdX = -1 * std::log(threshold);
  set();
}

void expo::set(float seconds, bool reverse) {
  mTotalS = seconds * consts::SAMPLE_RATE;
  mReverse = reverse;
  set();
}

void expo::set(float seconds) {
  mTotalS = seconds * consts::SAMPLE_RATE;
  set();
}

/**** tukey Class Implementation ****/

void tukey::set() {
  if (totalS <= 0) totalS = 1;
  currentS = 0;
  value = 0;
}

void tukey::set(float seconds, float alpha) {
  this->alpha = alpha;
  totalS = seconds * consts::SAMPLE_RATE;
  set();
}

void tukey::set(float seconds) {
  totalS = seconds * consts::SAMPLE_RATE;
  set();
}

/**** Load Soundfile into Memory ****/
bool util::load(
    std::string fileName,
    std::vector<buffer<float> *> &buf) {  // only works on mono files for now

  al::SearchPaths searchPaths;
  // searchPaths.addSearchPath("../../samples");
  // searchPaths.addSearchPath("..");
  //searchPaths.addSearchPath("/Users/jkilgore/Projects/EmissionControlPort/samples");

  // searchPaths.addSearchPath(
  //   "../../samples");

  // searchPaths.addSearchPath("../../samples");
  //searchPaths.print();

  std::string filePath = searchPaths.find(fileName).filepath();
  gam::SoundFile soundFile;
  soundFile.path(fileName);

  if (!soundFile.openRead()) {
    std::cout << "We could not read " << fileName << "!" << std::endl;
    //exit(1); 
    return 0;
  }
  if (soundFile.channels() != 1) {
    std::cout << fileName << " is not a mono file" << std::endl;
    //exit(1);
    return 0;
  }

  buffer<float> *a = new buffer<float>();
  a->size = soundFile.samples();
  a->data = new float[a->size];
  soundFile.read(a->data, a->size);

  // Not working correctly :(
  // if(soundFile.frameRate() != consts::SAMPLE_RATE) {
  //   Buffer<float>* b = new Buffer<float>();
  //   b->size = a->size/soundFile.frameRate() * consts::SAMPLE_RATE;
  //   b->data = new float[b->size];
  //   SRC_DATA *conversion = new SRC_DATA{a->data, b->data, a->size, b->size};
  //   conversion->src_ratio = soundFile.frameRate()/consts::SAMPLE_RATE;
  //   src_simple(conversion, 0, soundFile.channels());
  //   buf.push_back(b);
  //   std::cout<< "b->size: " << b->size << " a->size: " << a->size <<
  //   std::endl; delete[] a->data; delete conversion;

  // } else buf.push_back(a);

  buf.push_back(a);

  soundFile.close();
  return 1;
}
