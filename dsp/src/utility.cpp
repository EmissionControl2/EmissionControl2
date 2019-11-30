#include "utility.h"
#include "Gamma/SoundFile.h"
#include "al/io/al_File.hpp"
#include "../external/libsamplerate/src/samplerate.h"

#include <string>
#include <boost/predef/os.h>

#if (BOOST_OS_WINDOWS)
#  include <stdlib.h>
#elif (BOOST_OS_SOLARIS)
#  include <stdlib.h>
#  include <limits.h>
#elif (BOOST_OS_LINUX)
#  include <unistd.h>
#  include <limits.h>
#elif (BOOST_OS_MACOS)
#  include <mach-o/dyld.h>
#elif (BOOST_OS_BSD_FREE)
#  include <sys/types.h>
#  include <sys/sysctl.h>
#endif

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

float tukey::operator()() {
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
  if ( soundFile.channels() > 2 ) {
    std::cout << fileName << " is not a mono/stereo file" << std::endl;
    //exit(1);
    return 0;
  }

  buffer<float> *a = new buffer<float>();
  a->size = soundFile.samples() ;
  a->data = new float[a->size];
  a->channels = soundFile.channels();
  //a->channels = 1; //Use for loading in non-audio files
  soundFile.read(a->data, a->size);

  /**
   * If buffer sample rate is not equal to synth's sample rate, convert.
   * Comment out if you want to read arbitrary files.
   */
  if(soundFile.frameRate() != consts::SAMPLE_RATE) {
    buffer<float>* b = new buffer<float>();
    b->size = (a->size/a->channels)/soundFile.frameRate() * consts::SAMPLE_RATE;
    b->data = new float[b->size];
    b->channels = soundFile.channels();
    SRC_DATA *conversion = new SRC_DATA;
    conversion->data_in = a->data;
    conversion->input_frames = a->size/a->channels;
    conversion->data_out = b->data;
    conversion->output_frames = b->size/b->channels;
    conversion->src_ratio = consts::SAMPLE_RATE/soundFile.frameRate();
    src_simple(conversion, 2, soundFile.channels()); //const value changes quality of sample rate conversion
    buf.push_back(b);
    //std::cout<< "b->size: " << b->size << " a->size: " << a->size <<std::endl; 
    delete[] a->data; delete conversion;

  } else buf.push_back(a);
  
  //buf.push_back(a); //Use for loading in non audio files

  soundFile.close();
  return 1;
}



/*
 * Returns the full path to the currently running executable,
 * or an empty string in case of failure.
 */
std::string util::getExecutablePath() {
#if (BOOST_OS_WINDOWS)
    char *exePath;
    if (_get_pgmptr(&exePath) != 0)
        exePath = "";
#elif (BOOST_OS_SOLARIS)
    char exePath[PATH_MAX];
    if (realpath(getexecname(), exePath) == NULL)
        exePath[0] = '\0';
#elif (BOOST_OS_LINUX)
    char exePath[PATH_MAX];
    ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
    if (len == -1 || len == sizeof(exePath))
        len = 0;
    exePath[len] = '\0';
#elif (BOOST_OS_MACOS)
    char exePath[PATH_MAX];
    uint32_t len = sizeof(exePath);
    if (_NSGetExecutablePath(exePath, &len) != 0) {
        exePath[0] = '\0'; // buffer too small (!)
    } else {
        // resolve symlinks, ., .. if possible
        char *canonicalPath = realpath(exePath, NULL);
        if (canonicalPath != NULL) {
            strncpy(exePath,canonicalPath,len);
            free(canonicalPath);
        }
    }
#elif (BOOST_OS_BSD_FREE)
    char exePath[2048];
    int mib[4];  mib[0] = CTL_KERN;  mib[1] = KERN_PROC;  mib[2] = KERN_PROC_PATHNAME;  mib[3] = -1;
    size_t len = sizeof(exePath);
    if (sysctl(mib, 4, exePath, &len, NULL, 0) != 0)
        exePath[0] = '\0';
#endif
    return std::string(exePath);
}


