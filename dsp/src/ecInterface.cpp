#include "granular.cpp"

// We make an app.
class ecInterface : public App
{
public:


  // GUI manager for OscEnv voices
  // The name provided determines the name of the directory
  // where the presets and sequences are stored
  SynthGUIManager<Granular> synthManager {"GranularVoices"};
  SynthGUIManager<Modulator> synthManagerMod {"Modulation"};



  OutputRecorder mRecorder;

  virtual void onInit( ) override {

//    mRecorder.start("output.wav", audioIO().framesPerSecond(), 2);
    // Append recorder to audio IO object. This will run the recorder
    // after running the onSound() function below
    audioIO().append(mRecorder);
    synthManager.setCurrentTab(2); // Run constant synth note.
    synthManagerMod.setCurrentTab(2); // Run constant synth note

    synthManager.triggerOn();
    synthManagerMod.triggerOn();
  }

  virtual void onCreate() override {
    ParameterGUI::initialize();

    // Play example sequence. Comment this line to start from scratch
    //        synthManager.synthSequencer().playSequence("synth2.synthSequence");
    synthManager.synthRecorder().verbose(true);
    //        std::cout << " ----- " << std::endl;

  }

  virtual void onSound(AudioIOData &io) override {
    synthManager.render(io); // Render audio
  }

  virtual void onDraw(Graphics &g) override {
    g.clear();
    synthManager.render(g);
    synthManagerMod.render(g);

    // Draw GUI
    ParameterGUI::beginDraw();
    ParameterGUI::beginPanel(synthManager.name(),600,-1,-1,600);
    synthManager.drawFields();
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Presets",600,600);
    synthManager.drawPresets();
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("recorder");
    SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
    ParameterGUI::endPanel();




    
    ParameterGUI::beginPanel(synthManagerMod.name(),250,-1);
    synthManagerMod.drawFields();
    ParameterGUI::endPanel();
    ParameterGUI::endDraw();

  }

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

};