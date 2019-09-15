#ifndef UTILITY_H 
#define UTILITY_H 

#include <chrono>
#include <cmath>
#include <cstdio>
#include "const.h"
#include "al_ext/soundfile/al_SoundfileBuffered.hpp"
#include "al_ext/soundfile/al_SoundfileBufferedRecord.hpp"
#include "../external/libsamplerate/src/samplerate.h"

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


//envelope generator for creating exponetial decay and exponential growth.
class expo {
public:
  void set() {
    if (mTotalS <= 0) mTotalS = 1;
    if (mThresholdY <= 0) mThresholdY = 0.001, mThresholdX = -1 * std::log(0.001);
    mX = 0;
    if(mReverse) mY = mThresholdY;
    else mY =1;
    mIncrementX = (mThresholdX / mTotalS); // -ln(0.001) 
  }                                         // where 0.001 is our threshold for when we release envelope
  void set(float seconds, bool reverse,float threshold, float alpha) {
    mTotalS = seconds * consts::SAMPLE_RATE;
    mReverse = reverse;
    mThresholdY = threshold;
    mThresholdX = -1 * std::log(threshold);
    mAlpha = alpha;
    set();
  }

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

  bool done() { 
    return mX == 0;
  }

  void increment() {mX += mIncrementX;}

  float operator()() { 
  if(!mReverse) {
    if(mX < mThresholdX * 0.01) { // ratio of initial ramp up to 1
      mY = powf(M_E, 100 * mX - mThresholdX); //bias needed to reach that value in time (SEE DESMOS)
      mX += mIncrementX;
    }
    else if(mX < mThresholdX) {
        mY = powf(M_E, -1 * mX * mAlpha + (mThresholdX * 0.01) ); // this compensates for initial ramp up 
        mX += mIncrementX;
    } else  {
      std::cout << mY << std::endl;
      mY = 1; 
      mX = 0;
    }
  } else { //reversed Logic
    if(mX < mThresholdX * 0.92761758634 ) { 
      mY = powf(M_E, 0.9 * (mX - mThresholdX + 0.5)); // (mx - thresh + bias ) where bias determines the ratio of envelope (mThresholdX * ratio)
      mX += mIncrementX;
    } else if(mX < mThresholdX * 0.95) {  //small sustain to makeup for percieved volume loss (in relation to expodec)
         mY = 1;      
         mX += mIncrementX;
    }else if(mX < mThresholdX ) { //quickly bring envelope down to zero before marking as done.
      mY = powf(M_E, -20 * ( (mX) - (mThresholdX * 0.95) )); 
      mX += mIncrementX;
      //std::cout << mY << std::endl;
    } else { 
      mY = mThresholdY;
      mX = 0;
    } 
  }
  return mY;
  }
private:
  float mAlpha = 1, mIncrementX, mX= 0, mY = 1, mThresholdX = -1 * std::log(0.001), mThresholdY = 0.001;
  bool mReverse = 0;
  int mTotalS = 1;
  int tempCounter = 0;
};

// envelope GENERATOR for creating a tukey window
class tukey {
public: 
  void set() {
    if (totalS <= 0) totalS = 1 ;
  }
  void set(float seconds,float alpha) {
    this->alpha = alpha;
    totalS = seconds * consts::SAMPLE_RATE;
    set();
  }
  void set(float seconds) {
    totalS = seconds * consts::SAMPLE_RATE;
    set();
  }

  bool done() { return totalS == currentS; }

  void increment() {currentS++;}

  float operator()() {
    if(currentS < (alpha * totalS)/2) {
        value = 0.5 * (1 + std::cos(M_PI * (2 * currentS / (alpha * totalS) - 1)));
        currentS++;
    } else if (currentS  <= totalS * (1 - alpha/2)) {
        value = 1;
        currentS++;
    } else if ( currentS <= totalS) {
        value = 0.5 * (1 + std::cos(M_PI * (2 * currentS / (alpha * totalS) - (2/alpha) + 1)));
        currentS++;
    } else currentS = 0;
    return value;
  }
private:
  float value = 0, alpha = 0.6;
  int currentS = 0, totalS = 1;
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

void load(std::string fileName, std::vector<Buffer<float>*>& buf) { //only works on mono files for now
  al::SearchPaths searchPaths;
  // searchPaths.addSearchPath("../../samples");
  searchPaths.addSearchPath("/Users/jkilgore/Applications/allo/EmissionControlPort/samples");
  //searchPaths.print();

  std::string filePath = searchPaths.find(fileName).filepath(); 
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

  Buffer<float>* a = new Buffer<float>();
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
  //   std::cout<< "b->size: " << b->size << " a->size: " << a->size << std::endl; 
  //   delete[] a->data;
  //   delete conversion;

  // } else buf.push_back(a);

  buf.push_back(a);

  soundFile.close();
}




} //util:: 

// typedef struct
// {   
//   float  *data_in, *data_out ;

//   long   input_frames, output_frames ;
//   long   input_frames_used, output_frames_gen ;

//   int    end_of_input ;

//   double src_ratio ;
// } SRC_DATA ;

#endif
