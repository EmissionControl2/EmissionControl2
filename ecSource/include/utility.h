#ifndef UTILITY_H
#define UTILITY_H

#include <cmath>
#include <memory>
#include <mutex>

#include "Gamma/Filter.h"
#include "Gamma/Gamma.h"
#include "Gamma/Oscillator.h"
#include "al/io/al_File.hpp"
#include "const.h"

namespace util {

class FastTrig {
 public:
  void buildTrigTable();
  float get_cos_implied_pi_factor(float x);

 private:
  static const int CIRCLE = 1024;
  static const int MASK_CIRCLE = CIRCLE - 1;
  static const int HALF_CIRCLE = CIRCLE / 2;
  static const int QUARTER_CIRCLE = CIRCLE / 4;
  float COS_TABLE[CIRCLE];
};

/**
 * Line class that moves from one point to another over a set period of time.
 */
template <typename T>
class line {
 public:
  /**
   * @brief Default constructor.
   */
  line() { mSamplingRate = consts::SAMPLE_RATE; }

  /**
   * @brief line constructor.
   *
   * @param[in] The sample rate needed for operator()
   */
  line(T samplingRate) : mSamplingRate(samplingRate) {}

  /**
   * @brief Generate line in real-time.
   *
   * @return Amplitude value at a point in time.
   */
  T operator()() {
    if (value != target) {
      value += increment;
      if ((increment < 0) ? (value < target) : (value > target)) value = target;
    }
    return value;
  }

  /**
   * @brief Set paramaters of class.
   *
   * @param[in]: Starting value.
   * @param[in]: Target value.
   * @param[in]: The amount of time to go from the starting value to the target
   * value.
   */
  void set(T v = 0, T t = 0, T s = 1) {
    value = v;
    start = v;
    target = t;
    seconds = s;
    if (seconds <= 0) seconds = 1 / mSamplingRate;
    increment = (target - value) / (seconds * mSamplingRate);
  }

  void setSamplingRate(T samplingRate) { mSamplingRate = samplingRate; }

  T getSamplingRate() const { return mSamplingRate; }

  T getIncrement() const { return increment; }

  T getStart() const { return start; }

  T getValue() const { return value; }

  T getTarget() const { return target; }

  /**
   * @brief Check if the line function is complete.
   *
   * @param[in] Return true if reached target, false otherwise.
   */
  bool const done() { return value == target; }

 private:
  T value = 0, start = 0, target = 0, seconds = 1, increment = 0, mSamplingRate;
};

/**
 * Envelope generator for creating exponetial decay and exponential growth.
 */
class expo {
 public:
  /**
   * @brief Generate exponential envelope in real-time.
   *
   * @return Amplitude value at a point in time.
   */
  float operator()();

  /**
   * @brief Getter and setter for sampling rate of envelope.
   */
  void setSamplingRate(float samplingRate) { mSamplingRate = samplingRate; }
  float getSamplingRate() const { return mSamplingRate; }

  /**
   * @brief Used to set values to its original position.
   */
  void set();

  /**
   * Set parameters of envelope.
   *
   * @param[in] The duration of the envelope in seconds.
   * @param[in] If true, the envelope is reversed and becomes an exponential
   * growth envelope.
   * @param[in] Sets the amplitude threshold for when to release the envelope.
   */
  void set(float seconds, bool reverse, float threshold);
  void set(float seconds, bool reverse);
  void set(float seconds);

  /**
   * @brief Check if the line function is complete.
   *
   * @param[in] Return true if reached target, false otherwise.
   */
  bool done() const { return mX == 0; }

  /**
   * @brief Increment in the time axis/
   */
  void increment() { mX += mIncrementX; }

 private:
  float mIncrementX, mX = 0, mY = 0.001, mThresholdX = -1 * std::log(0.001), mThresholdY = 0.001,
                     mSamplingRate = consts::SAMPLE_RATE;
  bool mReverse = 0;
  int mTotalS = 1;
};

/**
 * Envelope generator for creating a tukey envelope.
 * A tukey window is like a fatter Hann envelope.
 */
class tukey {
 public:
  tukey() { fast_trig.buildTrigTable(); }
  /**
   * @brief Generate tukey envelope in real-time.
   *
   * @return Amplitude value at a point in time.
   */
  float operator()();

  /**
   * @brief Getter and setter for sampling rate of envelope.
   */
  void setSamplingRate(float samplingRate) { mSamplingRate = samplingRate; }
  float getSamplingRate() const { return mSamplingRate; }

  /**
   * Used to reset values to original position.
   */
  void set();

  /**
   * Set parameters of envelope.
   *
   * @param[in] The duration of the envelope in seconds.
   * @param[in] FROM 0 to 1. A coefficient that determines the ratio of the
   * sustain to the attack and release. 0 being a rectangular envelope and 1
   * being a Hann envelope.
   *  https://www.mathworks.com/help/signal/ref/tukeywin.html
   */

  void set(float seconds, float alpha);
  void set(float seconds);

  /**
   * @brief Check if the line function is complete.
   *
   * @param[in] Return true if reached target, false otherwise.
   */
  bool done() const { return totalS == currentS; }

  /**
   * @brief Increment in the time axis/
   */
  void increment() { currentS++; }

 private:
  float value = 0, alpha = 0.6, mSamplingRate = consts::SAMPLE_RATE;
  int currentS = 0, totalS = 1;
  float alpha_totalS;
  FastTrig fast_trig;
};

/**
 * A Buffer struct that has multi-sound file loading functionalities.
 * Inspired by Karl Yerkes.
 */
template <typename T>
class buffer {
 public:
  T *data;
  unsigned size = 0;
  int channels;
  unsigned frames;
  std::string filePath;

  virtual ~buffer() {
    fflush(stdout);
    if (data) delete[] data;
  }

  void deleteBuffer() {
    fflush(stdout);
    if (data) delete[] data;
  }

  T &operator[](unsigned index) { return data[index]; }
  T operator[](const T index) const { return get(index); }

  T get(unsigned index) const { return data[index]; }

  /**
   * @brief Resize buffer.
   *
   * @param[in] New size.
   */
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

  /**
   * @brief Index buffer.
   *
   * @param[in] Index in buffer where value desired is stored.
   *
   * @return Value at given index.
   */
  T getInterpolate(float index) const {
    if (index < 0) index += size;
    if (index > size) index -= size;

    return raw(index);
  }

  T raw(const float index) const {
    const unsigned i = floor(index);
    const T x0 = data[i];
    const T x1 = data[(i >= (size - channels)) ? 0 : i + channels];  // looping semantics
    const T t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  /**
   * @brief Add value to index in buffer.
   *
   * @param[in] Index of buffer where value will be stored.
   * @param[in] Value to be stored.
   *
   * @return Value at given index.
   */
  void add(const float index, const T value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size - 1)) ? 0 : i + channels;  // looping semantics
    const float t = index - i;
    data[i] += value * (1 - t);
    data[j] += value * t;
  }
};

class RingBuffer {
 public:
  RingBuffer(unsigned maxSize) : mMaxSize(maxSize) {
    mBuffer.resize(mMaxSize);
    mTail = -1;
    mPrevSample = 0;
  }

  unsigned getMaxSize() const { return mMaxSize; }

  void resize(unsigned maxSize) {
    mMaxSize = maxSize;
    mBuffer.resize(mMaxSize);
  }

  void push_back(float value) {
    mMutex.lock();
    mTail = (mTail + 1) % mMaxSize;
    mBuffer[mTail] = value;
    mMutex.unlock();
  }

  int getTail() const { return mTail; }

  float at(unsigned index) {
    if (index >= mMaxSize) {
      std::cerr << "RingBuffer index out of range." << std::endl;
      index = index % mMaxSize;
    }
    if (mMutex.try_lock()) {
      mPrevSample = mBuffer.at(index);
      mMutex.unlock();
    }
    return mPrevSample;
  }

  float operator[](unsigned index) { return this->at(index); }

  const float *data() { return mBuffer.data(); }

  float getRMS(unsigned lookBackLength) {
    int start = mTail - lookBackLength;
    if (start < 0) start = mMaxSize + start;

    float val = 0.0;
    for (unsigned i = 0; i < lookBackLength; i++) {
      val += pow(mBuffer[(start + i) % mMaxSize], 2);
    }
    return sqrt(val / lookBackLength);
  }

  void print() const {
    for (auto i = mBuffer.begin(); i != mBuffer.end(); ++i) std::cout << *i << " ";
    std::cout << "\n";
  }

 private:
  std::vector<float> mBuffer;
  unsigned mMaxSize;
  int mTail;
  float mPrevSample;

  std::mutex mMutex;
};

struct Plot_RingBufferGetterData {
  const float *Values;
  int Stride;
  int RingOffset;
  int MaxRingSize;

  Plot_RingBufferGetterData(const float *values, int stride, int ring_offset, int max_ring_size) {
    Values = values;
    Stride = stride;
    RingOffset = ring_offset;
    MaxRingSize = max_ring_size;
  }
};

static float Plot_RingBufferGetter(void *data, int idx) {
  Plot_RingBufferGetterData *plot_data = (Plot_RingBufferGetterData *)data;
  const float v = *(const float *)(const void *)((const unsigned char *)plot_data->Values +
                                                 ((size_t)((idx + plot_data->RingOffset) %
                                                           plot_data->MaxRingSize)) *
                                                   plot_data->Stride);
  return v;
}

/**
 * @brief Load soundfile into a buffer in memory.
 *
 * @param[in] The filename. An absolute filename is preferred.
 * @param[out] A vector holding the audio buffers.
 *
 */
bool load(std::string fileName, std::vector<std::shared_ptr<buffer<float>>> &buf,
          float sampleRate = consts::SAMPLE_RATE, bool resample = 1, int max_samples = 256000000);

/**
 * @brief Get absolute path of executable.
 */
std::string getExecutablePath();

/**
 * @brief Get users home folder.
 */
std::string getUserHomePath();

/**
 * @brief If you are using an apple package, you might went the directory of the
 * Content folder rather than the actual executable.
 * param[in] The directory containing the ACTUAL UNIX EXECUTABLE.
 */
std::string getContentPath_OSX(std::string s);

/**
 * @brief Return a value between min and max based on val.
 *
 * @param[in] val: A value between [0, 1].
 * @param[in] min: Some minimum value.
 * @param[in] min: Some maximum value.
 * @param[in] isLog : If logarithmic, returns a value scaled logarithmically between min and max.
 *                    Linear otherwise.
 * @param[in] precision: Corresponds to the amount of places after the decimal point in base 10.
 */
float outputValInRange(float val, float min, float max, bool isLog = false,
                       unsigned int precision = 5);

}  // namespace util

#endif