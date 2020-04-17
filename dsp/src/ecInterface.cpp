// ecInterface.cpp

/**** Emission Control LIB ****/
#include "ecInterface.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "al/io/al_File.hpp"

/**** External LIB ****/
// #include <nfd.h>

using namespace al;

/**** ecInterface Implementation ****/

void ecInterface::onInit() {

  dimensions(1920, 1080);

  granulator.init(&audioIO());

  execPath = util::getExecutablePath();
  File f(execPath);

  
  // Load in all files in at specified directory.
  // Set output directory for presets.
  // Set output directory of recorded files.
  #ifdef __APPLE__
    granulator.loadInitSoundFiles(f.directory(execPath)+ "../../../samples/");
    soundOutput = f.directory(execPath) + "../../../soundOutput/";
    mPresets.setRootPath(f.directory(execPath) + "../../../presets/");
  #endif

  #ifdef __linux__
    granulator.loadInitSoundFiles(f.directory(execPath)+ "samples/");
    soundOutput = f.directory(execPath) + "soundOutput/";
    mPresets.setRootPath(f.directory(execPath) + "presets/");
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
        (f.directory(execPath) + "../Resources/Fonts/Roboto-Medium.ttf").c_str(),
        14.0f);
  #endif

  #ifdef __linux__
    ImGui::GetIO().Fonts->AddFontFromFileTTF(
        (f.directory(execPath) + "Resources/Fonts/Roboto-Medium.ttf").c_str(),
        14.0f);
  #endif

    // Scale font
    ImGui::GetIO().FontGlobalScale = 1.2;
    setGUIColors();
}

void ecInterface::onSound(AudioIOData &io) { granulator.onProcess(io); }

void ecInterface::onDraw(Graphics &g) {
  g.clear(background);

  // Get window height and width
  float windowWidth = width();
  float windowHeight = height();

  // Initialize Audio IO popup to false
  bool displayIO = false;

  al::imguiBeginFrame();

  // Draw GUI

  // draw menu bar
  static bool show_app_main_menu_bar = true;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("blah", "CTRL+1")) {
      }
      if (ImGui::MenuItem("blah", "", false, false)) {
      } // Disabled item
      ImGui::EndMenu();
    }
    if (ImGui::BeginMenu("Preferences")) {
      if (ImGui::MenuItem("Audio IO", "")) {
        displayIO = true;
      }
      if (ImGui::MenuItem("blah", "", false, false)) {
      } // Disabled item
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
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

  granulator.grainRate.drawRangeSlider();
  granulator.asynchronicity.drawRangeSlider();
  granulator.intermittency.drawRangeSlider();
  granulator.streams.drawRangeSlider();
  granulator.grainDurationMs.drawRangeSlider();
  granulator.envelope.drawRangeSlider();
  granulator.tapeHead.drawRangeSlider();
  granulator.transposition.drawRangeSlider();
  granulator.filter.drawRangeSlider();
  granulator.resonance.drawRangeSlider();
  granulator.volumeDB.drawRangeSlider();
  granulator.pan.drawRangeSlider();
  granulator.soundFile.drawRangeSlider();
  ParameterGUI::endPanel();

  // Draw modulation window
  ParameterGUI::beginPanel("Modulation", 0, windowHeight / 4 + 25,
                           windowWidth / 2, windowHeight / 2, flags);
  drawModulationControl(granulator.grainRateLFO,
                        granulator.modGrainRateDepth.mParameter);
  drawModulationControl(granulator.asyncLFO,
                        granulator.modAsynchronicityDepth.mParameter);
  drawModulationControl(granulator.intermittencyLFO,
                        granulator.modIntermittencyDepth.mParameter);
  drawModulationControl(granulator.streamsLFO,
                        granulator.modStreamsDepth.mParameter);
  drawModulationControl(granulator.grainDurationLFO,
                        granulator.modGrainDurationDepth.mParameter);
  drawModulationControl(granulator.envelopeLFO,
                        granulator.modEnvelopeDepth.mParameter);
  drawModulationControl(granulator.tapeHeadLFO,
                        granulator.modTapeHeadDepth.mParameter);
  drawModulationControl(granulator.transpositionLFO,
                        granulator.modTranspositionDepth.mParameter);
  drawModulationControl(granulator.filterLFO,
                        granulator.modFilterDepth.mParameter);
  drawModulationControl(granulator.resonanceLFO,
                        granulator.modResonanceDepth.mParameter);
  drawModulationControl(granulator.volumeLFO,
                        granulator.modVolumeDepth.mParameter);
  drawModulationControl(granulator.panLFO, granulator.modPanDepth.mParameter);
  drawModulationControl(granulator.soundFileLFO,
                        granulator.modSoundFileDepth.mParameter);
  ParameterGUI::endPanel();

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

  // Draw file selector window.
  ParameterGUI::beginPanel("File Selector", 0, windowHeight * 3 / 4 + 25,
                           windowWidth / 4, windowHeight / 4, flags);
  ImGui::Text("%s", currentFile.c_str());
  if (ImGui::Button("Select File")) {
    // When the select file button is clicked, the file selector is shown
    #ifdef __APPLE__
      selector.start(f.directory(execPath)+ "../../../samples");
    #endif
    #ifdef __linux__
      selector.start(f.directory(execPath) + "samples/");
    #endif
  }
  // The file selector knows internally whether it should be drawn or not,
  // so you should always draw it. Check the return value of the draw function
  // to know if the user has selected a file through the file selector
  if (selector.drawFileSelector()) {
    auto selection = selector.getSelection();
    if (selection.count() > 0) {
      previousFile = currentFile;
      currentFile = selection[0].filepath();
    }
  }
  if (currentFile != previousFile) {
    granulator.loadSoundFile(currentFile);
    previousFile = currentFile;
  }
  ParameterGUI::endPanel();

  // Draw Scope window
  ParameterGUI::beginPanel("Scope", windowWidth / 4, windowHeight * 3 / 4 + 25,
                           windowWidth / 2, windowHeight / 4, flags);
  ImGui::Text("Number of Active Grains: %.1i ", granulator.getActiveVoices());
  // ImGui::PlotHistogram("Number of Active Grains: %.1i
  // ",granulator.getActiveVoices() );

  ParameterGUI::endPanel();

  // Pop the colors that were pushed at the start of the draw call
  // ImGui::PopStyleColor(13);
  // ImGui::PopStyleVar();

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
}