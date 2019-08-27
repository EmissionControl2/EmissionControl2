#ifndef UTILITY_H 
#define UTILITY_H 

#include <chrono>
#include <cmath>
#include <cstdio>
#include "const.h"
#include "al_ext/soundfile/al_SoundfileBuffered.hpp"

namespace util {


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


template<typename T>
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
    if (index < 0) index += size;  // -21221488559881683402437427200.000000
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

void load(std::string fileName, std::vector<Buffer<double>*>& buf) {
  al::SearchPaths searchPaths;
  // searchPaths.addSearchPath("../../samples");
  searchPaths.addSearchPath("/Users/jkilgore/Applications/allo/EmissionControlPort/samples");
  //searchPaths.print();

  std::string filePath = searchPaths.find(fileName).filepath(); //JKilg currently debugging.
  gam::SoundFile soundFile;
  soundFile.path(filePath);

  if (!soundFile.openRead()) {
    std::cout << "We could not read " << fileName << "!" << std::endl;
    exit(1);
  }
  if (soundFile.channels() != 1) {
    std::cout << fileName << " is not a mono file" << std::endl;
    exit(1);
  }

  Buffer<double>* a = new Buffer<double>();
  a->size = soundFile.frames();
  a->data = new double[a->size];
  soundFile.read(a->data, a->size);
  
  // STILL NEED TO TEST IF THIS WORKS AND NEED TO LINK IT
  if(soundFile.frameRate() != SAMPLE_RATE) {
    // Buffer<double>* b = new Buffer<double>();
    // b->data = new double[a->size/soundFile.frameRate() * SAMPLE_RATE];
    // r8b::CDSPResampler convertSampleRate(soundFile.frameRate(),double(SAMPLE_RATE), a->size);
    // convertSampleRate.process(a->data,a->size,b->data);
    // soundClip.push_back(b);
    // delete[] a->data;

  } else buf.push_back(a);

  soundFile.close();
}

}

#endif