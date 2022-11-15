#include "utility.h"

#include "../external/libsamplerate/src/samplerate.h"
#include "Gamma/SoundFile.h"

/**** C STANDARD ****/
#include <string.h>

#include <cassert>
#include <string>

#ifdef _WIN32
#include <stdlib.h>
#define strcasecmp _stricmp
#define PATH_MAX 1024
#elif __linux__
#include <assert.h>
#include <limits.h>
#include <unistd.h>
#else
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdlib.h>
#endif

#include <stdio.h> /* defines FILENAME_MAX */
#ifdef _WIN32
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

using namespace util;

void FastTrig::buildTrigTable() {
  for (int i = 0; i < CIRCLE; i++) {
    COS_TABLE[i] = cos(M_PI * (float)i / HALF_CIRCLE);
  }
}

// For optimization purposes, x is assuemd to be multiplied by a factor of PI.
// Based off of
// http://www.flipcode.com/archives/Fast_Trigonometry_Functions_Using_Lookup_Tables.shtml
float FastTrig::get_cos_implied_pi_factor(float x) {
  int index = (int)(x * HALF_CIRCLE);
  if (index < 0) {
    return COS_TABLE[-((-index) & MASK_CIRCLE) + CIRCLE];
  } else {
    return COS_TABLE[index & MASK_CIRCLE];
  }

  assert(0);
}

/**** expo Class Implementation ****/
float expo::operator()() {
  if (!mReverse) {
    if (mX < mThresholdX * 0.01) {             // ratio of initial ramp up to 1
      mY = powf(M_E, 100 * mX - mThresholdX);  // bias needed to reach that
      mX += mIncrementX;                       // value in time (SEE DESMOS)
    } else if (mX < mThresholdX) {
      mY = powf(M_E, -1 * mX + (mThresholdX * 0.01));  // this compensates for initial ramp up
      mX += mIncrementX;
    } else {
      mY = mThresholdY;
      mX = 0;
    }
  } else {  // reversed Logic
    if (mX < mThresholdX * 0.92761758634) {
      mY =
        powf(M_E, 0.9 * (mX - mThresholdX + 0.5));  // (mx - thresh + bias ) where bias determines
      mX += mIncrementX;                            // the ratio of envelope (mThresholdX * ratio)
    } else if (mX < mThresholdX * 0.95) {           // small sustain to makeup for percieved
                                                    // volume loss (in relation to expodec).
      mY = 1;
      mX += mIncrementX;
    } else if (mX < mThresholdX) {  // quickly bring envelope down to zero //
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
  if (mThresholdY <= 0) mThresholdY = 0.001, mThresholdX = -1 * std::log(0.001);
  mX = 0;
  mY = mThresholdY;
  mIncrementX = (mThresholdX / mTotalS);
}

void expo::set(float seconds, bool reverse, float threshold) {
  mTotalS = seconds * mSamplingRate;
  mReverse = reverse;
  mThresholdY = threshold;
  mThresholdX = -1 * std::log(threshold);
  set();
}

void expo::set(float seconds, bool reverse) {
  mTotalS = seconds * mSamplingRate;
  mReverse = reverse;
  set();
}

void expo::set(float seconds) {
  mTotalS = seconds * mSamplingRate;
  set();
}

/**** tukey Class Implementation ****/

float tukey::operator()() {
  if (currentS < (alpha_totalS) / 2) {
    value = 0.5 * (1 + fast_trig.get_cos_implied_pi_factor((2 * currentS / (alpha_totalS)-1)));
    currentS++;
  } else if (currentS <= totalS * (1 - alpha / 2)) {
    value = 1;
    currentS++;
  } else if (currentS <= totalS) {
    value =
      0.5 *
      (1 + fast_trig.get_cos_implied_pi_factor((2 * currentS / (alpha_totalS) - (2 / alpha) + 1)));
    currentS++;
  } else
    currentS = 0;
  return value;
}

void tukey::set() {
  if (totalS <= 0) totalS = 1;
  currentS = 0;
  value = 0;
  alpha_totalS = totalS * alpha;
}

void tukey::set(float seconds, float alpha) {
  this->alpha = alpha;
  totalS = seconds * mSamplingRate;
  set();
}

void tukey::set(float seconds) {
  totalS = seconds * mSamplingRate;
  set();
}

/**** Load Soundfile into Memory ****/
bool util::load(std::string fileName, std::vector<std::shared_ptr<buffer<float>>> &buf,
                float samplingRate, bool resample, int max_samples) {
  al::SearchPaths searchPaths;
  std::string filePath = searchPaths.find(fileName).filepath();
  gam::SoundFile soundFile;
  soundFile.path(fileName);
  if (!soundFile.openRead()) {
    std::cout << "NOT LOADING: We could not read " << fileName << "!" << std::endl;
    // exit(1);
    return 0;
  }
  if (soundFile.channels() > 2) {
    std::cout << "NOT LOADING: " << fileName << " is not a mono/stereo file" << std::endl;
    // exit(1);
    return 0;
  }

  if (soundFile.samples() > max_samples) {
    std::cout << "NOT LOADING: Sound file is larger than allowed number of samples." << std::endl;
    return 0;
  }

  std::shared_ptr<buffer<float>> a(new buffer<float>());
  a->filePath = fileName;
  a->size = soundFile.samples();
  a->frames = soundFile.frames();
  a->data = new float[a->size];
  a->channels = soundFile.channels();
  // a->channels = 1; //Use for loading in non-audio files
  soundFile.read(a->data, a->frames);
  if (resample) {  // We care about resampling.

    /**
     * If buffer sample rate is not equal to synth's sample rate, convert.
     * Comment out if you want to read arbitrary files.
     */
    if (soundFile.frameRate() != samplingRate) {
      std::shared_ptr<buffer<float>> b(new buffer<float>());
      b->filePath = fileName;
      b->size = (a->size) / soundFile.frameRate() * samplingRate;
      b->frames = b->size / soundFile.channels();
      b->data = new float[b->size];
      b->channels = soundFile.channels();
      SRC_DATA *conversion = new SRC_DATA;
      conversion->data_in = a->data;
      conversion->input_frames = a->size / a->channels;
      conversion->data_out = b->data;
      conversion->output_frames = b->size / b->channels;
      conversion->src_ratio = samplingRate / soundFile.frameRate();
      src_simple(conversion, SRC_SINC_FASTEST,
                 soundFile.channels());  // const value changes quality of sample
                                         // rate conversion
      buf.push_back(b);
      // delete[] a->data;
      delete conversion;
    } else {
      buf.push_back(a);
    }

  } else
    buf.push_back(a);  // We don't care about resampling the audio buffer.
                       // Note: can be used to load in non-audio files ;)
  soundFile.close();
  return 1;
}

/*
 * Returns the full path to the currently running executable,
 * or an empty string in case of failure.
 */
std::string util::getExecutablePath() {
#if _WIN32
  char *exePath;
  if (_get_pgmptr(&exePath) != 0) exePath = "";

#elif __linux__
  char exePath[PATH_MAX];
  ssize_t len = ::readlink("/proc/self/exe", exePath, sizeof(exePath));
  if (len == -1 || len == sizeof(exePath)) len = 0;
  exePath[len] = '\0';
#else  // THIS MEANS YOU ARE USING A >
  char exePath[PATH_MAX];
  uint32_t len = sizeof(exePath);
  if (_NSGetExecutablePath(exePath, &len) != 0) {
    exePath[0] = '\0';  // buffer too small (!)
  } else {
    // resolve symlinks, ., .. if possible
    char *canonicalPath = realpath(exePath, NULL);
    if (canonicalPath != NULL) {
      strncpy(exePath, canonicalPath, len);
      free(canonicalPath);
    }
  }
#endif
  return std::string(exePath);
}

std::string util::getContentPath_OSX(std::string s) {
  char delim = '/';
  size_t counter = 0;
  size_t i = s.size() - 1;
  while (counter < 2) {
    if (s[i] == delim) counter++;
    i--;
  }
  return s.substr(0, i + 2);
}

std::string util::getUserHomePath() {
  char homedir[PATH_MAX];
#ifdef _WIN32
  snprintf(homedir, sizeof(homedir), "%s%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"));
#else
  snprintf(homedir, sizeof(homedir), "%s", getenv("HOME"));
#endif
  std::string result = strdup(homedir);
  return result;
}

float util::outputValInRange(float val, float min, float max, bool isLog, unsigned int precision) {
  assert(val >= 0 && val <= 1);
  float logarithmic_zero_epsilon = powf(0.1f, precision);
  float output_linear = min + (val * std::fabs(max - min));

  // Edge case for if max and min are flipped.
  if (max < min) {
    std::swap(min, max);
  }

  float output;
  if (isLog) {
    float min_linear = (abs(min) < logarithmic_zero_epsilon)
                         ? ((min < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon)
                         : min;
    float max_linear = (abs(max) < logarithmic_zero_epsilon)
                         ? ((max < 0.0f) ? -logarithmic_zero_epsilon : logarithmic_zero_epsilon)
                         : max;

    if ((min == 0.0f) && (max < 0.0f))
      min_linear = -logarithmic_zero_epsilon;
    else if ((max == 0.0f) && (min < 0.0f))
      max_linear = -logarithmic_zero_epsilon;

    // Set output.
    if (output_linear < min_linear) {
      output = min;
    } else if (output_linear > max_linear) {
      output = max;
    } else if ((min * max) < 0.0f) {  // Range is in negative and positive.
      float zero_point_center = (abs(min) / abs(max - min));
      if (val > zero_point_center - logarithmic_zero_epsilon &&
          val < zero_point_center + logarithmic_zero_epsilon *
                                      10) {  // hacky way to detect an equality with midi precision.
        output = 0.0f;                       // Special case for exactly zero
      } else if (val < zero_point_center) {  // val less than zero point (negative)
        float min_log = logf(abs(min_linear));
        float scale = (abs(min_log) - logf(logarithmic_zero_epsilon)) / (abs(min_linear));
        float test = logf(logarithmic_zero_epsilon) + scale * abs(output_linear);
        output = -1 * expf(logf(logarithmic_zero_epsilon) + scale * abs(output_linear));
      } else {  // val less than zero point (positive)
        float max_log = logf(max_linear);
        float scale = (max_log - logf(logarithmic_zero_epsilon)) / (max_linear);
        output = expf(logf(logarithmic_zero_epsilon) + scale * (output_linear));
      }
    } else if ((min < 0.0f) || (max < 0.0f)) {  // Entirely negative slider
      float v_min = logf(abs(min_linear));
      float v_max = logf(abs(max_linear));
      float scale = (v_min - v_max) / (min_linear - max_linear);
      output = -1 * expf((v_min + scale * (output_linear - min_linear)));
    } else {  // Entirely positive slider.
      float v_min = logf(min_linear);
      float v_max = logf(max_linear);
      float scale = (v_max - v_min) / (max_linear - min_linear);
      output = expf(v_max + scale * (output_linear - max_linear));
    }
  } else {
    output = output_linear;
  }

  return output;
}