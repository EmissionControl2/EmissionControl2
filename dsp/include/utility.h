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


/**
 * Line class that moves from one point to another over a set period of time.
 */
class line {
 public:

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
   * @param[in]: The amount of time to go from the starting value to the target value.
   */
  void set(float v = 0, float t = 0, float s = 1 / consts::SAMPLE_RATE);

  /**
   * @brief Check if the line function is complete. 
   * 
   * @param[in] Return true if reached target, false otherwise.
   */
  bool const done() { return value == target; }

 private:
  float value = 0, target = 0, seconds = 1 / consts::SAMPLE_RATE, increment = 0;
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
   * @brief Used to set values to its original position.
   */
  void set();

  /**
   * Set parameters of envelope.
   *
   * @param[in] The duration of the envelope in seconds.
   * @param[in] If true, the envelope is reversed and becomes an exponential growth envelope. 
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
                     mThresholdY = 0.001;
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
  float value = 0, alpha = 0.6;
  int currentS = 0, totalS = 1;
};

/**
 * A Buffer struct that has multi-sound file loading functionalities. 
 * Inspired by Karl Yerkes.
 */
template <typename T>
class buffer {
public:
  T* data;
  unsigned size = 0;

  virtual ~buffer() {
    printf("Buffer deleted.\n");
    fflush(stdout);
    if (data) delete[] data;
  }

  T& operator[](unsigned index) { return data[index]; }
  T operator[](const T index) const { return get(index); }


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
  T get(float index) const {
    if (index < 0) index += size;
    if (index > size) index -= size;

    return raw(index);
  }

  T raw(const float index) const {
    const unsigned i = floor(index);
    const T x0 = data[i];
    const T x1 = data[(i == (size - 1)) ? 0 : i + 1];  // looping semantics
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
bool load(std::string fileName, std::vector<buffer<float>*>& buf);

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
