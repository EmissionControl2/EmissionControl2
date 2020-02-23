#ifndef UTILITY_H
#define UTILITY_H

#include "Gamma/Gamma.h"
#include "al/io/al_File.hpp"
#include "const.h"
#include <cmath>

namespace util {

/**
 * Line class that moves from one point to another over a set period of time.
 */
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
  line(float samplingRate) : mSamplingRate(samplingRate) {}

  /**
   * @brief Generate line in real-time.
   *
   * @return Amplitude value at a point in time.
   */
  float operator()();

  /**
   * @brief Set paramaters of class.
   *
   * @param[in]: Starting value.
   * @param[in]: Target value.
   * @param[in]: The amount of time to go from the starting value to the target
   * value.
   */
  void set(float v = 0, float t = 0, float s = 1);

  void setSamplingRate(float samplingRate) { mSamplingRate = samplingRate; }

  float getSamplingRate() const { return mSamplingRate; }

  /**
   * @brief Check if the line function is complete.
   *
   * @param[in] Return true if reached target, false otherwise.
   */
  bool const done() { return value == target; }

private:
  float value = 0, target = 0, seconds = 1, increment = 0;
  float mSamplingRate;
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
  float mIncrementX, mX = 0, mY = 0.001, mThresholdX = -1 * std::log(0.001),
                     mThresholdY = 0.001, mSamplingRate = consts::SAMPLE_RATE;
  bool mReverse = 0;
  int mTotalS = 1;
};

/**
 * Envelope generator for creating a tukey envelope.
 * A tukey window is like a fatter Hann envelope.
 */
class tukey {
public:
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
};

/**
 * A Buffer struct that has multi-sound file loading functionalities.
 * Inspired by Karl Yerkes.
 */
template <typename T> class buffer {
public:
  T *data;
  unsigned size = 0;
  int channels;
  std::string filePath;

  virtual ~buffer() {
    printf("Buffer deleted.\n");
    fflush(stdout);
    if (data)
      delete[] data;
  }

  void deleteBuffer() {
    fflush(stdout);
    if (data)
      delete[] data;
  }

  T &operator[](unsigned index) { return data[index]; }
  T operator[](const T index) const { return get(index); }

  /**
   * @brief Resize buffer.
   *
   * @param[in] New size.
   */
  void resize(unsigned n) {
    size = n;
    if (data)
      delete[] data; // or your have a memory leak
    if (n == 0) {
      data = nullptr;
    } else {
      data = new T[n];
      for (unsigned i = 0; i < n; ++i)
        data[i] = 0.0f;
    }
  }

  /**
   * @brief Index buffer.
   *
   * @param[in] Index in buffer where value desired is stored.
   *
   * @return Value at given index.
   */
  T get(float index) const {
    if (index < 0)
      index += size;
    if (index > size)
      index -= size;

    return raw(index);
  }

  T raw(const float index) const {
    const unsigned i = floor(index);
    const T x0 = data[i];
    const T x1 = data[(i == (size - 1)) ? 0 : i + 1]; // looping semantics
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
    const unsigned j = (i == (size - 1)) ? 0 : i + 1; // looping semantics
    const float t = index - i;
    data[i] += value * (1 - t);
    data[j] += value * t;
  }
};

/**
 * @brief Load soundfile into a buffer in memory.
 *
 * @param[in] The filename. An absolute filename is preferred.
 * @param[out] A vector holding the audio buffers.
 */
bool load(std::string fileName, std::vector<buffer<float> *> &buf,
          float sampleRate = consts::SAMPLE_RATE, bool resample = 1);

/**
 * @brief Get absolute path of executable.
 */
std::string getExecutablePath();

/**
 * @brief a comparator function for sorting the filePaths (case insensitive).
 */
bool compareFileNoCase(al::FilePath s1, al::FilePath s2);

} // namespace util

#endif
