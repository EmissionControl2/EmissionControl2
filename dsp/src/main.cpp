#include "ecInterface.cpp"

int main(){    // Create app instance

  ecInterface app; //why does this make extraneous calls? Synth GUI manager causes it
  std::cout << "HEREEEEmade it\n";
  // Set up audio
  app.initAudio(consts::SAMPLE_RATE, consts::BLOCK_SIZE, consts::AUDIO_OUTS, consts::DEVICE_NUM);
  // Set sampling rate for Gamma objects from app's audio
  gam::sampleRate(app.audioIO().framesPerSecond());
  app.dimensions(1200, 800);

  app.audioIO().print();

  app.start();
  return 0;
}

