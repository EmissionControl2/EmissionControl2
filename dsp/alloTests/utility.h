#ifndef UTILITY_H 
#define UTILITY_H 

#include <chrono>
#include <cmath>
#include <cstdio>
#include "const.h"


struct Line {
  float value = 0, target = 0, seconds = 1 / SAMPLE_RATE, increment = 0;

  void set() {
    if (seconds <= 0) seconds = 1 / SAMPLE_RATE;
    // slope per sample
    increment = (target - value) / (seconds * SAMPLE_RATE);
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
};



struct Array {
  float* data = nullptr;
  unsigned size = 0;

  virtual ~Array() {
    printf("Array deleted.\n");
    fflush(stdout);
    if (data) delete[] data;
  }

  // deep-copy copy constructor
  // Array(const Array& other);
  // also do assignment

  float& operator[](unsigned index) { return data[index]; }
  float operator[](const float index) const { return get(index); }

  void resize(unsigned n) {
    size = n;
    if (data) delete[] data;  // or your have a memory leak
    if (n == 0) {
      data = nullptr;
    } else {
      data = new float[n];
      for (unsigned i = 0; i < n; ++i) data[i] = 0.0f;
    }
  }

  float get(float index) const {
    // allow for sloppy indexing (e.g., negative, huge) by fixing the index to
    // within the bounds of the array
    if (index < 0) index += size;  // -21221488559881683402437427200.000000
    if (index > size) index -= size;

    // defer to our method without bounds checking
    return raw(index);
  }

  float raw(const float index) const {
    const unsigned i = floor(index);
    const float x0 = data[i];
    const float x1 = data[(i == (size - 1)) ? 0 : i + 1];  // looping semantics
    const float t = index - i;
    return x1 * t + x0 * (1 - t);
  }

  void add(const float index, const float value) {
    const unsigned i = floor(index);
    const unsigned j = (i == (size - 1)) ? 0 : i + 1;  // looping semantics
    const float t = index - i;
    data[i] += value * (1 - t);
    data[j] += value * t;
  }
};



#endif