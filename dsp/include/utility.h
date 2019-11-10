#ifndef UTILITY_H
#define UTILITY_H

#include <chrono>
#include <cmath>
#include <cstdio>
#include "../external/libsamplerate/src/samplerate.h"
#include "Gamma/Gamma.h"
#include "Gamma/SoundFile.h"
#include "al/io/al_File.hpp"
#include "const.h"

namespace util {

struct line {
 public:
  void set() {
    if (seconds <= 0) seconds = 1 / consts::SAMPLE_RATE;
    // slope per sample
    increment = (target - value) / (seconds * consts::SAMPLE_RATE);
  }
  void set(float v, float t, float s) {
    value = v;
    target = t;
    seconds = s;
    set();
  }
  void set(float t, float s) {
    target = t;
    seconds = s;
    set();
  }
  void set(float t) {
    target = t;
    set();
  }

  bool done() { return value == target; }

  float operator()() {
    if (value != target) {
      value += increment;
      if ((increment < 0) ? (value < target) : (value > target)) value = target;
    }
    return value;
  }

 private:
  float value = 0, target = 0, seconds = 1 / consts::SAMPLE_RATE, increment = 0;
};

/**
 * Envelope generator for creating exponetial decay and exponential growth.
 */
class expo {
 public:
  /**
   * Used to reset values to original position.
   */
  void set() {
    if (mTotalS <= 0) mTotalS = 1;
    if (mThresholdY <= 0)
      mThresholdY = 0.001, mThresholdX = -1 * std::log(0.001);
    mX = 0;
    mY = mThresholdY;
    mIncrementX = (mThresholdX / mTotalS);
  }

  /**
   * Set parameters of envelope.
   *
   * param[in] The duration of the envelope in seconds.
   * param[in] If true, the envelope is reversed and becomes an exponential
   * growth envelope. param[in] Sets the amplitude threshold for when to release
   * the envelope.
   */
  void set(float seconds, bool reverse, float threshold) {
    mTotalS = seconds * consts::SAMPLE_RATE;
    mReverse = reverse;
    mThresholdY = threshold;
    mThresholdX = -1 * std::log(threshold);
    set();
  }

  void set(float seconds, bool reverse) {
    mTotalS = seconds * consts::SAMPLE_RATE;
    mReverse = reverse;
    set();
  }

  void set(float seconds) {
    mTotalS = seconds * consts::SAMPLE_RATE;
    set();
  }

  /**
   * Returns true if the envelope is finished. False otherwise.
   */
  bool done() { return mX == 0; }

  void increment() { mX += mIncrementX; }

  /**
   * Envelope generated at the audio rate.
   */
  float operator()() {
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

 private:
  float mIncrementX, mX = 0, mY = 0.001, mThresholdX = -1 * std::log(0.001),
                     mThresholdY = 0.001;
  bool mReverse = 0;
  int mTotalS = 1;
  int tempCounter = 0;
};

/**
 * Envelope generator for creating a tukey envelope.
 * A tukey window is like a fatter Hann envelope.
 */
class tukey {
 public:
  /**
   * Used to reset values to original position.
   */
  void set() {
    if (totalS <= 0) totalS = 1;
    currentS = 0;
    value = 0;
  }

  /**
   * Set parameters of envelope.
   *
   * param[in] The duration of the envelope in seconds.
   * param[in] FROM 0 to 1. A coefficient that determines the ratio of the
   * sustain to the attack and release. 0 being a rectangular envelope and 1
   * being a Hann envelope.
   *  https://www.mathworks.com/help/signal/ref/tukeywin.html
   */
  void set(float seconds, float alpha) {
    this->alpha = alpha;
    totalS = seconds * consts::SAMPLE_RATE;
    set();
  }
  void set(float seconds) {
    totalS = seconds * consts::SAMPLE_RATE;
    set();
  }

  /**
   * Return true if envelope is finished. False otherwise.
   */
  bool done() { return totalS == currentS; }

  void increment() { currentS++; }

  /**
   * Envelope generated at the audio rate.
   */
  float operator()() {
    if (currentS < (alpha * totalS) / 2) {
      value =
          0.5 * (1 + std::cos(M_PI * (2 * currentS / (alpha * totalS) - 1)));
      currentS++;
    } else if (currentS <= totalS * (1 - alpha / 2)) {
      value = 1;
      currentS++;
    } else if (currentS <= totalS) {
      value = 0.5 * (1 + std::cos(M_PI * (2 * currentS / (alpha * totalS) -
                                          (2 / alpha) + 1)));
      currentS++;
    } else
      currentS = 0;
    return value;
  }

 private:
  float value = 0, alpha = 0.6;
  int currentS = 0, totalS = 1;
};

/**
 * A Buffer struct that has multi-sound file loading functionalities.
 */
template <typename T>
struct Buffer {
  T* data;
  unsigned size = 0;

  virtual ~Buffer() {
    printf("Buffer deleted.\n");
    fflush(stdout);
    if (data) delete[] data;
  }

  // deep-copy copy constructor
  // Array(const Array& other);
  // also do assignment

  T& operator[](unsigned index) { return data[index]; }
  T operator[](const T index) const { return get(index); }

  void resize(unsigned n) {
    size = n;
    if (data) delete[] data;  // or your have a memory leak
    if (n == 0) {
      data = nullptr;
    } else {
      data = new T[n];
      for (unsigned i = 0; i < n; ++i) data[i] = 0.0f;
    }
  }

  T get(float index) const {
    // allow for sloppy indexing (e.g., negative, huge) by fixing the index to
    // within the bounds of the array
    if (index < 0) index += size;
    if (index > size) index -= size;

    // defer to our method without bounds checking
    return raw(index);
  }

  T raw(const float index) const {
    const unsigned i = floor(index);
    const T x0 = data[i];
    const T x1 = data[(i == (size - 1)) ? 0 : i + 1];  // looping semantics
    const T t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  void add(const float index, const T value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size - 1)) ? 0 : i + 1;  // looping semantics
    const float t = index - i;
    data[i] += value * (1 - t);
    data[j] += value * t;
  }
};

/**
 * Load soundfile into a buffer in memory.
 *
 * param[in] The filename. An absolute filename is preferred.
 * param[out] A vector holding the audio buffers.
 */
void load(std::string fileName, std::vector<Buffer<float>*>& buf);

}  // namespace util

// typedef struct
// {
//   float  *data_in, *data_out ;

//   long   input_frames, output_frames ;
//   long   input_frames_used, output_frames_gen ;

//   int    end_of_input ;

//   double src_ratio ;
// } SRC_DATA ;

#endif
