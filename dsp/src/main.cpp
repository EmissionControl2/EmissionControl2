#include "granular.cpp"

int main(){    // Create app instance

  MyApp app; //why does this make extraneous calls? Synth GUI manager causes it
  std::cout << "HEREEEEmade it\n";
  // Set up audio
  app.initAudio(SAMPLE_RATE, BLOCK_SIZE, AUDIO_OUTS, DEVICE_NUM);
  // Set sampling rate for Gamma objects from app's audio
  gam::sampleRate(app.audioIO().framesPerSecond());

  app.dimensions(1200, 800);

  app.audioIO().print();


  app.start();
  return 0;
}

