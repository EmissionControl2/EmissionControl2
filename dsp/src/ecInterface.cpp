// ecInterface.cpp

/**** Emission Control LIB ****/
#include "ecInterface.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "al/io/al_File.hpp"

using namespace al;

/**** ecInterface Implementation ****/

void ecInterface::onInit() {
  dimensions(1920, 1080);
  execDir = f.directory(util::getExecutablePath());
  granulator.init(&audioIO());

// #ifdef __APPLE__

//   result = NFD_OpenDialog("wav;aiff;aif", NULL, &outPath);
//   if (result == 1)
//     execPath = outPath;
// #endif

// Load in all files in at specified directory.
// Set output directory for presets.
// Set output directory of recorded files.
#ifdef __APPLE__
  execDir = util::getContentPath(execDir);
  granulator.loadInitSoundFiles(execDir + "Resources/samples/");
  soundOutput = f.conformPathToOS(execDir + "Resources/soundOutput/");
  mPresets.setRootPath(f.conformPathToOS(execDir + "Resources/presets/"));
#endif

#ifdef _WIN32_
  granulator.loadInitSoundFiles(execDir + "samples/");
  soundOutput = f.conformPathToOS(execDir + "soundOutput/");
  mPresets.setRootPath(f.conformPathToOS(execDir + "presets/"));
#endif

#ifdef __linux__
  granulator.loadInitSoundFiles(execDir + "samples/");
  soundOutput = execDir + "soundOutput/";
  mPresets.setRootPath(execDir + "presets/");
#endif
  audioIO().append(mRecorder);
}

void ecInterface::onCreate() {
  al::imguiInit();
  granulator.grainRate.addToPresetHandler(mPresets);
  granulator.asynchronicity.addToPresetHandler(mPresets);
  granulator.intermittency.addToPresetHandler(mPresets);
  granulator.streams.addToPresetHandler(mPresets);
  granulator.grainDurationMs.addToPresetHandler(mPresets);
  granulator.envelope.addToPresetHandler(mPresets);
  granulator.tapeHead.addToPresetHandler(mPresets);
  granulator.transposition.addToPresetHandler(mPresets);
  granulator.filter.addToPresetHandler(mPresets);
  granulator.resonance.addToPresetHandler(mPresets);
  granulator.volumeDB.addToPresetHandler(mPresets);
  granulator.pan.addToPresetHandler(mPresets);
  granulator.soundFile.addToPresetHandler(mPresets);

  granulator.modGrainRateDepth.addToPresetHandler(mPresets);
  granulator.modAsynchronicityDepth.addToPresetHandler(mPresets);
  granulator.modIntermittencyDepth.addToPresetHandler(mPresets);
  granulator.modStreamsDepth.addToPresetHandler(mPresets);
  granulator.modGrainDurationDepth.addToPresetHandler(mPresets);
  granulator.modEnvelopeDepth.addToPresetHandler(mPresets);
  granulator.modTapeHeadDepth.addToPresetHandler(mPresets);
  granulator.modTranspositionDepth.addToPresetHandler(mPresets);
  granulator.modFilterDepth.addToPresetHandler(mPresets);
  granulator.modResonanceDepth.addToPresetHandler(mPresets);
  granulator.modVolumeDepth.addToPresetHandler(mPresets);
  granulator.modPanDepth.addToPresetHandler(mPresets);
  granulator.modSoundFileDepth.addToPresetHandler(mPresets);

  mPresets << granulator.grainRateLFO << granulator.asyncLFO
           << granulator.intermittencyLFO << granulator.streamsLFO
           << granulator.grainDurationLFO << granulator.envelopeLFO
           << granulator.tapeHeadLFO << granulator.transpositionLFO
           << granulator.filterLFO << granulator.resonanceLFO
           << granulator.volumeLFO << granulator.panLFO
           << granulator.soundFileLFO;

  granulator.LFOparameters[0]->frequency->addToPresetHandler(mPresets);
  mPresets << *granulator.LFOparameters[0]->shape
           << *granulator.LFOparameters[0]->duty;

  granulator.LFOparameters[1]->frequency->addToPresetHandler(mPresets);
  mPresets << *granulator.LFOparameters[1]->shape
           << *granulator.LFOparameters[1]->duty;

  granulator.LFOparameters[2]->frequency->addToPresetHandler(mPresets);
  mPresets << *granulator.LFOparameters[2]->shape
           << *granulator.LFOparameters[2]->duty;

  granulator.LFOparameters[3]->frequency->addToPresetHandler(mPresets);
  mPresets << *granulator.LFOparameters[3]->shape
           << *granulator.LFOparameters[3]->duty;

#ifdef __APPLE__
  ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 14.0f);
#endif

#ifdef __linux__
  ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 14.0f);
#endif

  // Scale font
  ImGui::GetIO().FontGlobalScale = 1.2;
  setGUIColors();
}

void ecInterface::onSound(AudioIOData &io) { granulator.onProcess(io); }

void ecInterface::onDraw(Graphics &g) {
  framecounter++;
  g.clear(background);

  // Get window height and width
  float windowWidth = width();
  float windowHeight = height();

  // Initialize Audio IO popup to false
  bool displayIO = false;

  al::imguiBeginFrame();

  if (granulator.getNumberOfAudioFiles() == 0 && audioIO().isRunning()) {
    ImGui::OpenPopup("Load soundfiles please :,)");
    audioIO().stop();
    noSoundFiles = true;
  }

  if (granulator.getNumberOfAudioFiles() != 0 && !audioIO().isRunning()) {
    audioIO().start();
    noSoundFiles = false;
  }
  // Draw GUI

  // draw menu bar
  static bool show_app_main_menu_bar = true;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Load Sound File", "")) {
        ImGui::Text("%s", currentFile.c_str());

// When the select file button is clicked, the file selector is shown
#ifdef __APPLE__

        result = NFD_OpenDialog("wav;aiff;aif", NULL, &outPath);
        if (result == 1)
          currentFile = outPath;
#endif
#ifdef __linux__ 
        result = NFD_OpenDialog("wav;aiff;aif", NULL, &outPath);
        if (result == 1)
          currentFile = outPath;
#endif

        if ((currentFile != previousFile) && (result == 1)) {
          granulator.loadSoundFile(currentFile);
          previousFile = currentFile;
        }
      }
      if (ImGui::MenuItem("Audio IO", "")) {
        displayIO = true;
      }
      if (ImGui::MenuItem("Remove Current Sound File", "")) {
        granulator.removeCurrentSoundFile();
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  // Draw an interface to Audio IO.
  // This enables starting and stopping audio as well as selecting
  // Audio device and its parameters
  // if statement opens Audio IO popup if chosen from menu
  if (displayIO == true) {
    ImGui::OpenPopup("Audio IO");
  }

  bool open = true;
  if (ImGui::BeginPopupModal("Audio IO", &open)) {
    drawAudioIO(&audioIO());
    ImGui::EndPopup();
  }

  // Draw LFO parameters window
  ParameterGUI::beginPanel("LFO Controls", 0, 25, windowWidth / 2,
                           windowHeight / 4, flags);
  drawLFOcontrol(granulator, 0);
  drawLFOcontrol(granulator, 1);
  drawLFOcontrol(granulator, 2);
  drawLFOcontrol(granulator, 3);
  ParameterGUI::endPanel();

  ParameterGUI::beginPanel("Granulator Controls", windowWidth / 2,
                           windowHeight / 4 + 25, windowWidth / 2,
                           windowHeight / 2, flags);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  granulator.grainRate.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  granulator.asynchronicity.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  granulator.intermittency.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  granulator.streams.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  granulator.grainDurationMs.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  granulator.envelope.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  granulator.transposition.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  granulator.filter.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  granulator.resonance.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  granulator.soundFile.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  granulator.tapeHead.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  granulator.pan.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  granulator.volumeDB.drawRangeSlider();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  ParameterGUI::endPanel();

  // Draw modulation window
  ParameterGUI::beginPanel("Modulation", 0, windowHeight / 4 + 25,
                           windowWidth / 2, windowHeight / 2, flags);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  drawModulationControl(granulator.grainRateLFO,
                        granulator.modGrainRateDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  drawModulationControl(granulator.asyncLFO,
                        granulator.modAsynchronicityDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  drawModulationControl(granulator.intermittencyLFO,
                        granulator.modIntermittencyDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  drawModulationControl(granulator.streamsLFO,
                        granulator.modStreamsDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  drawModulationControl(granulator.grainDurationLFO,
                        granulator.modGrainDurationDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  drawModulationControl(granulator.envelopeLFO,
                        granulator.modEnvelopeDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  drawModulationControl(granulator.transpositionLFO,
                        granulator.modTranspositionDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  drawModulationControl(granulator.filterLFO,
                        granulator.modFilterDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  drawModulationControl(granulator.resonanceLFO,
                        granulator.modResonanceDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  drawModulationControl(granulator.soundFileLFO,
                        granulator.modSoundFileDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  drawModulationControl(granulator.tapeHeadLFO,
                        granulator.modTapeHeadDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.612f, 0.690f, 0.647f));
  drawModulationControl(granulator.panLFO, granulator.modPanDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.929f, 0.933f, 0.929f));
  drawModulationControl(granulator.volumeLFO,
                        granulator.modVolumeDepth.mParameter);
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  ParameterGUI::endPanel();

  // Draw recorder window
  ParameterGUI::beginPanel("Recorder", windowWidth * 3 / 4,
                           windowHeight * 3 / 4 + 25, windowWidth / 4,
                           windowHeight / 4, flags);

  drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(),
                     audioIO().channelsOut(), soundOutput);
  ParameterGUI::endPanel();

  // Draw preset window
  ParameterGUI::beginPanel("Presets", windowWidth / 2, 25, windowWidth / 2,
                           windowHeight / 4, flags);
  ParameterGUI::drawPresetHandler(&mPresets, 12, 4);
  ParameterGUI::endPanel();

  // Draw grain histogram window
  ParameterGUI::beginPanel("Active Grains", 0, windowHeight * 3 / 4 + 25,
                           windowWidth / 4, windowHeight / 4, flags);
  ImGui::Text("Current Active Grains: %.1i ", granulator.getActiveVoices());
  if (framecounter % 10 == 0) {
    streamHistory.erase(streamHistory.begin());
    streamHistory.push_back(granulator.getActiveVoices());
  }
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(1.0f, 1.0f, 1.0f, 0.1f));
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
  ImGui::SetCursorPosY(70);
  ImGui::PlotHistogram("##Active Streams", &streamHistory[0],
                       streamHistory.size(), 0, nullptr, 0, 100,
                       ImVec2(0, (windowHeight / 4) - 120), sizeof(int));
  ParameterGUI::endPanel();

  // Draw Oscilloscope window
  ParameterGUI::beginPanel("Oscilloscope", windowWidth / 4,
                           windowHeight * 3 / 4 + 25, windowWidth / 2,
                           windowHeight / 4, flags);
  ImGui::Text("Oscilloscope Timeframe (s):");
  ImGui::SameLine();
  if (ImGui::SliderFloat("##Scope frame", &oscFrame, 0.001, 3.0, "%.3f")) {
    if (oscFrame <= 3.0) {
      oscDataL.resize(int(oscFrame * consts::SAMPLE_RATE));
      oscDataR.resize(int(oscFrame * consts::SAMPLE_RATE));
    }
  }
  oscDataL = granulator.oscBufferL.getArray(oscDataL.size());
  oscDataR = granulator.oscBufferR.getArray(oscDataR.size());
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth());
  ImGui::SetCursorPosY(70);
  ImGui::PushStyleColor(ImGuiCol_PlotLines,
                        (ImVec4)ImColor(0.0f, 0.0f, 1.0f, 1.0f));
  ImGui::PlotLines("ScopeL", &oscDataL[0], oscDataL.size(), 0, nullptr, -1, 1,
                   ImVec2(0, (windowHeight / 4) - 120), sizeof(float));
  ImGui::SetCursorPosY(70);
  ImGui::PushStyleColor(ImGuiCol_PlotLines,
                        (ImVec4)ImColor(1.0f, 0.0f, 0.0f, 1.0f));
  ImGui::PlotLines("ScopeR", &oscDataR[0], oscDataR.size(), 0, nullptr, -1, 1,
                   ImVec2(0, (windowHeight / 4) - 120), sizeof(float));
  ImGui::PopItemWidth();
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  ParameterGUI::endPanel();

  // Pop the colors that were pushed at the start of the draw call
  // ImGui::PopStyleColor(14);
  // ImGui::PopStyleVar();

  // Throw popup to remind user to load in sound files if none are present.
  if (ImGui::BeginPopupModal("Load soundfiles please :,)", &noSoundFiles)) {
    ImGui::Text("Files can be loaded in from the top left menu.\nAudio will turn on once a file has been loaded.");
    // ImGui::Text(execDir.c_str()); //DEBUG
    ImGui::EndPopup();
  }

  ImGui::End();
  al::imguiEndFrame();

  al::imguiDraw();
}

void ecInterface::drawAudioIO(AudioIO *io) {
  struct AudioIOState {
    int currentSr = 1;
    int currentBufSize = 3;
    int currentDevice = 0;
    std::vector<std::string> devices;
  };
  auto updateDevices = [&](AudioIOState &state) {
    state.devices.clear();
    int numDevices = AudioDevice::numDevices();
    for (int i = 0; i < numDevices; i++) {
      state.devices.push_back(AudioDevice(i).name());
    }
  };
  static std::map<AudioIO *, AudioIOState> stateMap;
  if (stateMap.find(io) == stateMap.end()) {
    stateMap[io] = AudioIOState();
    updateDevices(stateMap[io]);
  }
  AudioIOState &state = stateMap[io];
  ImGui::PushID(std::to_string((unsigned long)io).c_str());

  if (io->isOpen()) {
    std::string text;
    text += "Sampling Rate: " + std::to_string(int(io->fps()));
    text += "\nBuffer Size: " + std::to_string(io->framesPerBuffer());
    text += "\nInput Channels: " + std::to_string(io->channelsIn());
    text += "\nOutput Channels:" + std::to_string(io->channelsOut());
    ImGui::Text("%s", text.c_str());
    if (ImGui::Button("Stop")) {
      io->stop();
      io->close();
    }
  } else {
    if (ImGui::Button("Update Devices")) {
      updateDevices(state);
    }
    if (ImGui::Combo(
            "Device", &state.currentDevice, ParameterGUI::vector_getter,
            static_cast<void *>(&state.devices), state.devices.size())) {
      // TODO adjust valid number of channels.
    }
    std::vector<std::string> samplingRates{"44100", "48000", "88100", "96000"};
    ImGui::Combo("Sampling Rate", &state.currentSr, ParameterGUI::vector_getter,
                 static_cast<void *>(&samplingRates), samplingRates.size());
    if (ImGui::Button("Start")) {
      io->framesPerSecond(std::stof(samplingRates[state.currentSr]));
      io->framesPerBuffer(consts::BLOCK_SIZE);
      io->device(AudioDevice(state.currentDevice));
      granulator.setIO(io);
      granulator.resampleSoundFiles();
      io->open();
      io->start();
    }
  }
  ImGui::PopID();
}

static void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate,
                               uint32_t numChannels, std::string directory,
                               uint32_t bufferSize) {
  struct SoundfileRecorderState {
    bool recordButton;
    bool overwriteButton;
  };
  static std::map<SoundFileBufferedRecord *, SoundfileRecorderState> stateMap;
  if (stateMap.find(recorder) == stateMap.end()) {
    stateMap[recorder] = SoundfileRecorderState{0, false};
  }
  SoundfileRecorderState &state = stateMap[recorder];
  ImGui::PushID(std::to_string((unsigned long)recorder).c_str());
  if (ImGui::CollapsingHeader("Record Audio",
                              ImGuiTreeNodeFlags_CollapsingHeader |
                                  ImGuiTreeNodeFlags_DefaultOpen)) {
    static char buf1[64] = "test.wav";
    ImGui::InputText("Record Name", buf1, 63);
    if (state.recordButton) {
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));
      ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0, 0.5, 0.5, 1.0));
    }
    std::string buttonText = state.recordButton ? "Stop" : "Record";
    bool recordButtonClicked = ImGui::Button(buttonText.c_str());
    if (state.recordButton) {
      ImGui::PopStyleColor();
      ImGui::PopStyleColor();
    }
    if (recordButtonClicked) {
      state.recordButton = !state.recordButton;
      if (state.recordButton) {
        uint32_t ringBufferSize;
        if (bufferSize == 0) {
          ringBufferSize = 8192;
        } else {
          ringBufferSize = bufferSize * numChannels * 4;
        }
        std::string filename;
        if (!state.overwriteButton) {
          filename = buf1;
          int counter = 0;
          while (File::exists(directory + filename) && counter < 9999) {
            filename = buf1;
            int lastDot = filename.find_last_of(".");
            filename = filename.substr(0, lastDot) + std::to_string(counter++) +
                       filename.substr(lastDot);
          }
        }
        if (!recorder->start(directory + filename, frameRate, numChannels,
                             ringBufferSize)) {
          std::cerr << "Error opening file for record" << std::endl;
        }
      } else {
        recorder->close();
      }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Overwrite", &state.overwriteButton);
  }
  ImGui::PopID();
}

void ecInterface::drawLFOcontrol(ecSynth &synth, int lfoNumber) {
  ImGui::Text("LFO %i", lfoNumber + 1);
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
  ParameterGUI::drawMenu(synth.LFOparameters[lfoNumber]->shape);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.9);

  synth.LFOparameters[lfoNumber]->frequency->drawRangeSlider(true);
  // ParameterGUI::drawParameter(synth.LFOparameters[lfoNumber]->frequency);
  ImGui::PopItemWidth();
  ImGui::Indent(200);
  if (*synth.LFOparameters[lfoNumber]->shape == 1) {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.9);
    ParameterGUI::drawParameter(synth.LFOparameters[lfoNumber]->duty);
    ImGui::PopItemWidth();
  }
  ImGui::Unindent(200);
}

void ecInterface::drawModulationControl(al::ParameterMenu &menu,
                                        al::Parameter *slider) {
  ImGui::PushItemWidth(120);
  ParameterGUI::drawMenu(&menu);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvailWidth() * 0.99);
  ParameterGUI::drawParameter(slider);
  ImGui::PopItemWidth();
}

void ecInterface::setGUIColors() {
  ImGui::PushStyleColor(ImGuiCol_WindowBg,
                        (ImVec4)ImColor(0.474f, 0.568f, 0.513f));
  ImGui::PushStyleColor(ImGuiCol_PopupBg,
                        (ImVec4)ImColor(0.474f, 0.568f, 0.513f));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)ImColor(0.0f, 0.0f, 0.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_FrameBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f));
  ImGui::PushStyleColor(ImGuiCol_SliderGrab,
                        (ImVec4)ImColor(0.0f, 0.0f, 0.0f, 0.7f));
  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive,
                        (ImVec4)ImColor(0.0f, 0.0f, 0.0f, 0.7f));
  ImGui::PushStyleColor(ImGuiCol_Button,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_Header,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_HeaderActive,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBg,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed,
                        (ImVec4)ImColor(0.772f, 0.807f, 0.788f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_PlotLines,
                        (ImVec4)ImColor(0.0f, 0.0f, 1.0f, 1.0f));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                        (ImVec4)ImColor(0.0f, 0.0f, 0.0f, 0.7f));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered,
                        (ImVec4)ImColor(0.0f, 0.3f, 0.0f, 0.7f));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
}