#include "ecInterface.h"

int main() { // Create app instance

  ecInterface app;
  // Set up audio
  app.configureAudio(consts::SAMPLE_RATE, consts::BLOCK_SIZE,
                     consts::AUDIO_OUTS, consts::DEVICE_NUM);
  // Set sampling rate for Gamma objects from app's audio
  gam::sampleRate(app.audioIO().framesPerSecond());
  app.dimensions(1920, 1080);
  app.title("Emission Control 2");

  app.audioIO().print();

  app.start();
  return 0;
}
