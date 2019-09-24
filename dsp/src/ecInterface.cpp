#include "granular.cpp"

// We make an app.
class ecInterface : public App
{
public:


  // GUI manager for OscEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  //SynthGUIManager<Granular> synthManager {"bin/Granulator"};
  ControlGUI test;
  Granular granulator;
  PresetHandler mPresets{"bin/presets"};



  OutputRecorder mRecorder;

  virtual void onInit( ) override {

//    mRecorder.start("output.wav", audioIO().framesPerSecond(), 2);
    // Append recorder to audio IO object. This will run the recorder
    // after running the onSound() function below
    audioIO().append(mRecorder);
    //synthManager.setCurrentTab(2); // Run constant synth note.

    //synthManager.triggerOn();

    granulator.init();
    granulator.loadSoundFile("voicePop.wav");
  }

  virtual void onCreate() override {
    ParameterGUI::initialize();

    mPresets 
      << granulator.grainRate << granulator.modGrainRateWidth
      << granulator.asynchronicity << granulator.modAsynchronicityWidth
      << granulator.intermittency << granulator.modIntermittencyWidth
      << granulator.streams << granulator.modStreamsWidth 
      << granulator.grainDurationMs << granulator.modGrainDurationWidth
      << granulator.envelope << granulator.modEnvelopeWidth
      << granulator.tapeHead << granulator.modTapeHeadWidth
      << granulator.playbackRate << granulator.modPlaybackRateWidth
      << granulator.volumedB << granulator.modVolumeWidth
      << granulator.modSineFrequency << granulator.modSinePhase
      << granulator.modSquareFrequency << granulator.modSquareWidth
      << granulator.modSawFrequency << granulator.modSawWidth;

    // Play example sequence. Comment this line to start from scratch
    //        synthManager.synthSequencer().playSequence("synth2.synthSequence");
    //synthManager.synthRecorder().verbose(true);
    //        std::cout << " ----- " << std::endl;

  }

  virtual void onSound(AudioIOData &io) override {
    //synthManager.render(io); // Render audio
    granulator.onProcess(io);
  }

  virtual void onDraw(Graphics &g) override {
    g.clear();
    ParameterGUI::beginDraw();
    // synthManager.render(g);

    // // Draw GUI
    // ParameterGUI::beginDraw();
    // ParameterGUI::beginPanel(synthManager.name(),600,-1,-1,600);
    // synthManager.drawFields();
    // ParameterGUI::endPanel();

    // ParameterGUI::beginPanel("Presets",600,600);
    // synthManager.drawPresets();
    // ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Recorder",50,25);
    SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("LFO Controls", 400, 25,500);
    ParameterGUI::drawParameter(&granulator.modSineFrequency);
    ParameterGUI::drawParameter(&granulator.modSinePhase);
    ParameterGUI::drawParameter(&granulator.modSquareFrequency);
    ParameterGUI::drawParameter(&granulator.modSquareWidth);
    ParameterGUI::drawParameter(&granulator.modSawFrequency);
    ParameterGUI::drawParameter(&granulator.modSawWidth);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Granulator Controls", 600, 200,500,-1);
    ParameterGUI::drawParameter(&granulator.grainRate);
    ParameterGUI::drawParameter(&granulator.asynchronicity);
    ParameterGUI::drawParameter(&granulator.intermittency);
    ParameterGUI::drawParameterInt(&granulator.streams,"");
    ParameterGUI::drawParameter(&granulator.grainDurationMs);
    ParameterGUI::drawParameter(&granulator.envelope);
    ParameterGUI::drawParameter(&granulator.tapeHead);
    ParameterGUI::drawParameter(&granulator.playbackRate);
    ParameterGUI::drawParameter(&granulator.volumedB);
    
    
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Modulation Width", 50, 200, 500, -1);
    ParameterGUI::drawParameter(&granulator.modGrainRateWidth);
    ParameterGUI::drawParameter(&granulator.modAsynchronicityWidth);
    ParameterGUI::drawParameter(&granulator.modIntermittencyWidth);
    ParameterGUI::drawParameter(&granulator.modStreamsWidth);
    ParameterGUI::drawParameter(&granulator.modGrainDurationWidth);
    ParameterGUI::drawParameter(&granulator.modEnvelopeWidth);
    ParameterGUI::drawParameter(&granulator.modTapeHeadWidth);
    ParameterGUI::drawParameter(&granulator.modPlaybackRateWidth);
    ParameterGUI::drawParameter(&granulator.modVolumeWidth);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Presets", 400, 500);
    ParameterGUI::drawPresetHandler(&mPresets,12,4);
    ParameterGUI::endPanel();


    ParameterGUI::endDraw();

  }
/*
  virtual void onKeyDown(Keyboard const& k) override {
    if (ParameterGUI::usingKeyboard()) { //Ignore keys if GUI is using them
      return;
    }
    if (k.shift()) {
      // If shift pressed then keyboard sets preset
      int presetNumber = asciiToIndex(k.key());
      synthManager.recallPreset(presetNumber);
    } else {
      // Otherwise trigger note for polyphonic synth
      int midiNote = asciiToMIDI(k.key());

      synthManager.voice()->grainRate.set( ::powf (2.0f, (midiNote - 69.f)/12.f) * 432.f);
      if (midiNote > 0) {
        synthManager.triggerOn(midiNote);
      }
    }
  }

  virtual void onKeyUp(Keyboard const& k) override {
    int midiNote = asciiToMIDI(k.key());
    if (midiNote > 0) {
      synthManager.synth().triggerOff(midiNote);
    }
  }

  void onExit() override {
    // mRecorder.stop();
    ParameterGUI::cleanup();
    

  }
*/

};