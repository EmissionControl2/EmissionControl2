#include "al/core.hpp"
#include "Gamma/Oscillator.h"
#include <math.h>

#define SAMPLE_RATE (48000);
#define BLOCK_SIZE (512);


using namespace al;

class grain {
public:

private:
  float duration;
  float amplitude;
  float space;
};

class grainContainer{
public: 

private: 
};



class testPhasor{
public:
  float operator()() {
    phase += increment;
    if (phase < 0) phase += 1;
    if (phase > 1) phase -= 1;
    return phase;
  }
  
  void frequency(float hertz) {
    increment = hertz / SAMPLE_RATE;
  }
private: 
  float phase = 0.0; 
  float increment = 0.0001;
};

class testSine : public testPhasor {
public:
  testSine() {
    for(int i = 0; i < SIZE; i++) 
      table[i] = std::sin(2 * M_PI * i / SIZE); // this will give one period of a sine wave ! !
  }

  float operator()() {
    float phase = testPhasor::operator()();
    return table[int(SIZE * phase)];
  }

private: 
  static const int SIZE = 2048;
  float table[SIZE];
};

class linearInterpSine : public testPhasor {
public:
  linearInterpSine() {
    table = new float[SIZE];
    for (int i = 0; i < SIZE; i++)
      table[i] = std::sin(2 * M_PI * i / SIZE);
  }
  float operator()() {
    float phase = testPhasor::operator()() * SIZE;
    int left = int(phase);
    int right = left + 1; 
    float m = table[right] - table[left]; //slope      
    return ( m * (phase - left) + table[left]);
  }
private:
  int counter = 0;
  static const size_t SIZE = 2048;
  float *table; 
};

class asrEnvelope {
public:
  asrEnvelope(float attack = 0.25, float sustain = 0.5, float release = 0.25) : attack(attack), sustain(sustain), release(release){};
  //mutable
  void setAttack(float attack) {this->attack = attack;} 
  void setSustain(float sustain) {this->sustain = sustain;} 
  void setRelease(float release) {this->release = release;} 

  float operator ()() {
    if (counter == 1000) {
      std::cout << amplitude << std::endl;
      counter = 0;
    }
    counter++;

    if(amplitude < 0) {
      amplitude = 0; 
      currentSample = 0;
    }

    if(currentSample >  getAttackIncrement() + getSustainIncrement()) {
      amplitude -= 1/getReleaseIncrement();
      currentSample++;
      return amplitude;
    }

    if(currentSample < getAttackIncrement()) {
      amplitude += 1/getAttackIncrement();
      currentSample++;
      return amplitude;
    }
    if( getAttackIncrement() < currentSample < (getAttackIncrement() + getSustainIncrement())) {
      currentSample++;
      return amplitude;
    }
    
  }

  //immutable
  float getAttack() const { return attack;}
  float getSustain() const {return sustain;}
  float getRelease() const {return release;}

private: 
  float attack;
  float sustain; 
  float release; 
  float amplitude = 0;
  float currentSample = 0;
  int counter = 0;

  float getAttackIncrement() {
    return attack * SAMPLE_RATE;
  }
  float getSustainIncrement() {
    return sustain * SAMPLE_RATE;
  }
  float getReleaseIncrement() {
    return release * SAMPLE_RATE;
  }
};

struct test_app : App
{
  // testSine osc;
  // testSine osc1;
  // testSine osc2;
  // testSine osc3;
  linearInterpSine osc;
  linearInterpSine osc1;
  linearInterpSine osc2;
  linearInterpSine osc3;
  asrEnvelope testASR;


  void onCreate() override {
    printf("Audio devices found:\n");
	  AudioDevice::printAll();
	  printf("\n");
    osc.frequency(40);
    osc1.frequency(60);
    osc2.frequency(432);
    osc3.frequency(9000);
    testASR.setAttack(0.009);
    testASR.setSustain(0.2);
    testASR.setRelease(5);


  }

  void onDraw(Graphics& g) override {
    g.clear(0);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float out;
      float s = osc();
      float s1 = osc1();
      float s2 = osc2();
      float s3 = osc3();
      float env = testASR();
      out = ((s * 0.5) + (s1 * 0.45) + (s2*0.04) + (s3*0.002)) * 0.5 * env;
      io.out(0) = out;
      io.out(1) = out;
    }
  }
};
int main()
{
  test_app app;
  app.initAudio(48000, 512, 2, 3);
  app.start();
}
