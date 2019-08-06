#include "al/core.hpp"
#include "Gamma/Oscillator.h"

using namespace al;

class grain {
public:

private:

};

class grainContainer{
public: 

private: 
};

struct sine_app : App
{
  gam::Sine<> osc;

  void onCreate() override {
    gam::Domain::master().spu(audioIO().framesPerSecond());
    osc.freq(80);
  }

  void onDraw(Graphics& g) override {
    g.clear(0);
  }

  void onSound(AudioIOData& io) override {
    while (io()) {
      float s = osc(); // Generate next sine wave sample
      s *= 0.2; // Scale the sample down a bit for output
      io.out(0) = s;
      io.out(1) = s;
    }
  }
};
int main()
{
  sine_app app;
  app.initAudio(48000, 512, 2, 0);
  app.start();
}
