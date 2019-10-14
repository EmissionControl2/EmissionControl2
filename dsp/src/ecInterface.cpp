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
  PresetHandler mPresets{"presets"};
  float background = 0.21;
  float dragTest = 1.0;



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
    //granulator.verbose(true);

    //experimenting with imgui widgets
    
  }

  virtual void onCreate() override {
    ParameterGUI::initialize();
    mPresets 
      << *granulator.grainRate.mParameter << granulator.grainRateLFO << *granulator.modGrainRateWidth.mParameter
      << *granulator.asynchronicity.mParameter << granulator.asyncLFO << *granulator.modAsynchronicityWidth.mParameter
      << *granulator.intermittency.mParameter << granulator.intermittencyLFO << *granulator.modIntermittencyWidth.mParameter
      << *granulator.streams.mParameterInt << granulator.streamsLFO << *granulator.modStreamsWidth.mParameter
      << *granulator.grainDurationMs.mParameter << granulator.grainDurationLFO << *granulator.modGrainDurationWidth.mParameter
      << *granulator.envelope.mParameter << granulator.envelopeLFO << *granulator.modEnvelopeWidth.mParameter
      << *granulator.tapeHead.mParameter << granulator.tapeHeadLFO << *granulator.modTapeHeadWidth.mParameter
      << *granulator.playbackRate.mParameter << granulator.playbackRateLFO << *granulator.modPlaybackRateWidth.mParameter
      << *granulator.volumeDB.mParameter << granulator.volumeLFO << *granulator.modVolumeWidth.mParameter
      << *granulator.modSineFrequency.mParameter << *granulator.modSinePhase.mParameter
      << *granulator.modSquareFrequency.mParameter << *granulator.modSquareWidth.mParameter
      << *granulator.modSawFrequency.mParameter << *granulator.modSawWidth.mParameter;

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
    g.clear(background);
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

    ParameterGUI::beginPanel("Recorder",950,25);
    SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("LFO Controls", 25, 25,500);
    ParameterGUI::drawParameter(granulator.modSineFrequency.mParameter);
    ParameterGUI::drawParameter(granulator.modSinePhase.mParameter);
    ParameterGUI::drawParameter(granulator.modSquareFrequency.mParameter);
    ParameterGUI::drawParameter(granulator.modSquareWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modSawFrequency.mParameter);
    ParameterGUI::drawParameter(granulator.modSawWidth.mParameter);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Granulator Controls", 675, 250,500,-1);
    ParameterGUI::drawParameter(granulator.grainRate.mParameter);
    ParameterGUI::drawParameter(granulator.asynchronicity.mParameter);
    ParameterGUI::drawParameter(granulator.intermittency.mParameter);
    ParameterGUI::drawParameterInt(granulator.streams.mParameterInt,"");
    ParameterGUI::drawParameter(granulator.grainDurationMs.mParameter);
    ParameterGUI::drawParameter(granulator.envelope.mParameter);
    ParameterGUI::drawParameter(granulator.tapeHead.mParameter);
    ParameterGUI::drawParameter(granulator.playbackRate.mParameter);
    ParameterGUI::drawParameter(granulator.volumeDB.mParameter);
    ParameterGUI::endPanel();

    //THIsWorks
    ParameterGUI::beginPanel("TestTEST MY RANGEE", 950,150,100,-1);
    granulator.grainRate.drawRangeBox(true,0.1);
    granulator.grainRate.drawRangeBox(false,0.1);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Modulation Wave", 525, 250, 150, -1);
    ParameterGUI::drawMenu(&granulator.grainRateLFO);
    ParameterGUI::drawMenu(&granulator.asyncLFO);
    ParameterGUI::drawMenu(&granulator.intermittencyLFO);
    ParameterGUI::drawMenu(&granulator.streamsLFO);
    ParameterGUI::drawMenu(&granulator.grainDurationLFO);
    ParameterGUI::drawMenu(&granulator.envelopeLFO);
    ParameterGUI::drawMenu(&granulator.tapeHeadLFO);
    ParameterGUI::drawMenu(&granulator.playbackRateLFO);
    ParameterGUI::drawMenu(&granulator.volumeLFO);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Modulation Width", 25, 250, 500, -1);
    
    ParameterGUI::drawParameter(granulator.modGrainRateWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modAsynchronicityWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modIntermittencyWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modStreamsWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modGrainDurationWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modEnvelopeWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modTapeHeadWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modPlaybackRateWidth.mParameter);
    ParameterGUI::drawParameter(granulator.modVolumeWidth.mParameter);
    ParameterGUI::endPanel();

    ParameterGUI::beginPanel("Presets", 525, 25);
    ParameterGUI::drawPresetHandler(&mPresets,12,4);
    ParameterGUI::endPanel();

    
    ImGui::Text("Number of Active Grains: %.1i ",granulator.getActiveVoices() );
    ImGui::DragFloat("TESTs",&dragTest,1,1,1000);
    
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