// ecInterface.cpp

/**** Emission Control LIB ****/
#include "ecInterface.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "al/io/al_File.hpp"
#include "nlohmann/json.hpp"
using json = nlohmann::json;
#include "al/ui/al_ParameterGUI.hpp"

/**** CSTD LIBS ****/
#include <fstream>

using namespace al;

/**** ecInterface Implementation ****/

void ecInterface::onInit() {
  dimensions(1920, 1080);

  execDir = f.directory(util::getExecutablePath());
  userPath = util::getUserHomePath();

#ifdef __APPLE__
  execDir = util::getContentPath(execDir);
  system(f.conformPathToOS((execDir + consts::CONFIG_DIR_SCRIPT_PATH)).c_str());
  configFile = consts::DEFAULT_CONFIG_FILE;
#endif

#ifdef __linux__
  std::string configPath = "/.config/EmissionControl2";
  // use xdg directories if available
  if (getenv("$XDG_CONFIG_HOME") != NULL) {
    configPath = getenv("$XDG_CONFIG_HOME") + std::string("/EmissionControl2");
  }
  configFile = configPath + "/config/config.json";
  presetsPath = configPath + "/presets";

  // create config directories if needed
  system(("mkdir -p " + userPath + configPath + "/config").c_str());
  system(("mkdir -p " + userPath + presetsPath).c_str());
#endif

  initFileIOPaths();
  jsonReadAndSetAudioSettings();
  jsonReadAndSetColorSchemeMode();
  granulator.init(&audioIO());

// Load in all files in at specified directory.
// Set output directory for presets.
// Set output directory of recorded files.
#ifdef __APPLE__
  granulator.loadInitSoundFiles(userPath + consts::DEFAULT_SAMPLE_PATH);
  mPresets.setRootPath(f.conformPathToOS(userPath + consts::DEFAULT_PRESETS_PATH));
#endif
#ifdef _WIN32_
  granulator.loadInitSoundFiles(execDir + "samples/");
  mPresets.setRootPath(f.conformPathToOS(execDir + "presets/"));
  configFile = consts::DEFAULT_CONFIG_FILE;
#endif

#ifdef __linux__
  granulator.loadInitSoundFiles(consts::DEFAULT_SAMPLE_PATH);
  mPresets.setRootPath(userPath + presetsPath);
#endif
  audioIO().append(mRecorder);
  audioIO().append(mHardClip);
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
  granulator.scanSpeed.addToPresetHandler(mPresets);
  granulator.scanWidth.addToPresetHandler(mPresets);
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
  granulator.modScanWidthDepth.addToPresetHandler(mPresets);
  granulator.modTranspositionDepth.addToPresetHandler(mPresets);
  granulator.modFilterDepth.addToPresetHandler(mPresets);
  granulator.modResonanceDepth.addToPresetHandler(mPresets);
  granulator.modVolumeDepth.addToPresetHandler(mPresets);
  granulator.modPanDepth.addToPresetHandler(mPresets);
  granulator.modSoundFileDepth.addToPresetHandler(mPresets);

  mPresets << granulator.grainRateLFO << granulator.asyncLFO << granulator.intermittencyLFO
           << granulator.streamsLFO << granulator.grainDurationLFO << granulator.envelopeLFO
           << granulator.tapeHeadLFO << granulator.scanSpeedLFO << granulator.scanWidthLFO
           << granulator.transpositionLFO << granulator.filterLFO << granulator.resonanceLFO
           << granulator.volumeLFO << granulator.panLFO << granulator.soundFileLFO;

  for (int i = 0; i < granulator.NUM_MODULATORS; i++) {
    granulator.LFOparameters[i]->frequency->addToPresetHandler(mPresets);
    mPresets << *granulator.LFOparameters[i]->shape << *granulator.LFOparameters[i]->duty
             << *granulator.LFOparameters[i]->polarity;
  }

#ifdef __APPLE__
  ImFont *bodyFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
    (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 16.0f);
  ImFont *titleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
    (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 20.0f);
#endif

#ifdef __linux__
  bodyFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
    ("/usr/local/share/fonts/EmissionControl2/Roboto-Medium.ttf"), 16.0f);
  titleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
    ("/usr/local/share/fonts/EmissionControl2/Roboto-Medium.ttf"), 20.0f);
#endif

  setGUIColors();
}

void ecInterface::onSound(AudioIOData &io) { granulator.onProcess(io); }

void ecInterface::onDraw(Graphics &g) {
  setGUIColors();
  framecounter++;
  g.clear(background);

  // Get window height and width (for responsive sizing)
  float windowWidth = width();
  float windowHeight = height();

  // Compare window size to fb size to account for HIDPI Display issues
  // std::cout << width() << std::endl;

  // Initialize Audio IO popup to false
  bool displayIO = false;

  // Initialize Font scale popup to false
  bool fontScaleWindow = false;

  al::imguiBeginFrame();

  if (width() < 1000)
    ImGui::GetIO().FontGlobalScale = width() / 1000.0f;
  else
    ImGui::GetIO().FontGlobalScale = fontScale;

  adjustScaleY = 1.0f + ((fontScale - 1.0f) / 1.5f);

  if (granulator.getNumberOfAudioFiles() == 0 && audioIO().isRunning()) {
    ImGui::OpenPopup("Load soundfiles please :,)");
    audioIO().stop();
    noSoundFiles = true;
  }

  if (granulator.getNumberOfAudioFiles() != 0 && !audioIO().isRunning() && !isPaused) {
    audioIO().start();
    noSoundFiles = false;
  }
  // Draw GUI

  // draw menu bar
  static bool show_app_main_menu_bar = true;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Settings")) {
      if (ImGui::MenuItem("Audio Output", "")) {
        displayIO = true;
      }
      if (ImGui::MenuItem("Sound Recording Folder", "")) {
        result = NFD_PickFolder(NULL, &outPath);

        if (result == NFD_OKAY) {
          std::string temp = outPath;
          jsonWriteToConfig(temp, consts::SOUND_OUTPUT_PATH_KEY);
          jsonReadAndSetSoundOutputPath();
        }
      }
      if (ImGui::MenuItem("Load Sound File", "")) {
        ImGui::Text("%s", currentFile.c_str());

        // When the select file button is clicked, the file selector is shown

        result = NFD_OpenDialogMultiple("wav;aiff;aif", NULL, &pathSet);

        if (result == NFD_OKAY) {
          size_t i;
          for (i = 0; i < NFD_PathSet_GetCount(&pathSet); ++i) {
            nfdchar_t *path = NFD_PathSet_GetPath(&pathSet, i);
            granulator.loadSoundFile(path);
          }
          NFD_PathSet_Free(&pathSet);
        }

        // if ((currentFile != previousFile) && (NFD_OKAY == 1)) {
        //   granulator.loadSoundFile(currentFile);
        //   previousFile = currentFile;
        // }
      }
      if (ImGui::MenuItem("Remove Current Sound File", "")) {
        granulator.removeCurrentSoundFile();
      }
      if (ImGui::MenuItem("Toggle Light/Dark", "")) {
        if (light) {
          PrimaryColor = &PrimaryDark;
          SecondaryColor = &SecondaryDark;
          TertiaryColor = &TertiaryDark;
          Shade1 = &Shade1Dark;
          Shade2 = &Shade2Dark;
          Shade3 = &Shade3Dark;
          Text = &TextDark;
          light = false;
        } else {
          PrimaryColor = &PrimaryLight;
          SecondaryColor = &SecondaryLight;
          TertiaryColor = &TertiaryLight;
          Shade1 = &Shade1Light;
          Shade2 = &Shade2Light;
          Shade3 = &Shade3Light;
          Text = &TextLight;
          light = true;
        }
        jsonWriteToConfig(light, consts::LIGHT_MODE_KEY);
      }
      if (ImGui::MenuItem("Font Size", "")) {
        fontScaleWindow = true;
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  // PopUp Font scale window
  if (fontScaleWindow == true) {
    ImGui::OpenPopup("Font Size");
  }
  bool fontScaleOpen = true;
  if (ImGui::BeginPopupModal("Font Size", &fontScaleOpen)) {
    ImGui::PushItemWidth(windowWidth / 3);
    ImGui::SliderFloat("Font Size", &fontScale, 0.5, 3.0, "%.2f");
    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }

  // Draw an interface to Audio IO.
  // This enables starting and stopping audio as well as selecting
  // Audio device and its parameters
  // if statement opens Audio IO popup if chosen from menu
  if (displayIO == true) {
    ImGui::OpenPopup("Audio Settings");
  }

  bool audioOpen = true;
  if (ImGui::BeginPopupModal("Audio Settings", &audioOpen)) {
    drawAudioIO(&audioIO());
    ImGui::EndPopup();
  }

  // Draw Granulator Controls
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("GRANULATOR CONTROLS", 0, 25 * adjustScaleY, windowWidth / 2,
                           windowHeight / 2, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  granulator.grainRate.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  granulator.asynchronicity.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  granulator.intermittency.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  granulator.streams.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  granulator.grainDurationMs.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  granulator.envelope.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  granulator.transposition.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  granulator.filter.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  granulator.resonance.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  granulator.soundFile.drawRangeSlider(fontScale, granulator.getCurrentAudioFileName());
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  granulator.tapeHead.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  granulator.scanWidth.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  granulator.scanSpeed.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  granulator.pan.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  granulator.volumeDB.drawRangeSlider(fontScale);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  ParameterGUI::endPanel();

  // Draw modulation window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("MODULATION", windowWidth / 2, 25 * adjustScaleY, windowWidth / 2,
                           windowHeight / 2, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  drawModulationControl(granulator.grainRateLFO, granulator.modGrainRateDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  drawModulationControl(granulator.asyncLFO, granulator.modAsynchronicityDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  drawModulationControl(granulator.intermittencyLFO, granulator.modIntermittencyDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  drawModulationControl(granulator.streamsLFO, granulator.modStreamsDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  drawModulationControl(granulator.grainDurationLFO, granulator.modGrainDurationDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  drawModulationControl(granulator.envelopeLFO, granulator.modEnvelopeDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  drawModulationControl(granulator.transpositionLFO, granulator.modTranspositionDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  drawModulationControl(granulator.filterLFO, granulator.modFilterDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  drawModulationControl(granulator.resonanceLFO, granulator.modResonanceDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  drawModulationControl(granulator.soundFileLFO, granulator.modSoundFileDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  drawModulationControl(granulator.tapeHeadLFO, granulator.modTapeHeadDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  drawModulationControl(granulator.scanWidthLFO, granulator.modScanWidthDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  drawModulationControl(granulator.scanSpeedLFO, granulator.modScanSpeedDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  drawModulationControl(granulator.panLFO, granulator.modPanDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
  drawModulationControl(granulator.volumeLFO, granulator.modVolumeDepth);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw preset window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("PRESETS", 0, windowHeight / 2 + (25 * adjustScaleY), windowWidth / 2,
                           windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ecInterface::ECdrawPresetHandler(&mPresets, 12, 4);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw LFO parameters window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("LFO CONTROLS", windowWidth / 2, windowHeight / 2 + (25 * adjustScaleY),
                           windowWidth / 2, windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  drawLFOcontrol(granulator, 0);
  drawLFOcontrol(granulator, 1);
  drawLFOcontrol(granulator, 2);
  drawLFOcontrol(granulator, 3);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw recorder window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("RECORDER", windowWidth * 3 / 4,
                           windowHeight * 3 / 4 + (25 * adjustScaleY), windowWidth * 3 / 16,
                           windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut(), soundOutput);
  if (ImGui::Button("Change Output Path")) {
    result = NFD_PickFolder(NULL, &outPath);

    if (result == NFD_OKAY) {
      std::string temp = outPath;
      jsonWriteToConfig(temp, consts::SOUND_OUTPUT_PATH_KEY);
      jsonReadAndSetSoundOutputPath();
    }
  }
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw grain histogram window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("ACTIVE GRAINS", 0, windowHeight * 3 / 4 + (25 * adjustScaleY),
                           windowWidth / 4, windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ImGui::Text("Current Active Grains: %.1i ", granulator.getActiveVoices());

  if (grainAccum < granulator.getActiveVoices()) grainAccum = granulator.getActiveVoices();
  if (framecounter % 2 == 0) {
    streamHistory.erase(streamHistory.begin());
    streamHistory.push_back(grainAccum);
    highestStreamCount = *max_element(streamHistory.begin(), streamHistory.end());
    if (highestStreamCount < 2) highestStreamCount = 2;
    grainAccum = 0;
  }
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
  ImGui::SetCursorPosY(70 * adjustScaleY);
  ImGui::PlotHistogram(
    "##Active Streams", &streamHistory[0], streamHistory.size(), 0, nullptr, 0, highestStreamCount,
    ImVec2(0, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)), sizeof(int));
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw Oscilloscope window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("OSCILLOSCOPE", windowWidth / 4,
                           windowHeight * 3 / 4 + (25 * adjustScaleY), windowWidth / 2,
                           windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ImGui::Text("Oscilloscope Timeframe (s):");
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (100 * fontScale));
  if (ImGui::SliderFloat("##Scope frame", &oscFrame, 0.001, 3.0, "%.3f")) {
    if (oscFrame <= 3.0 || globalSamplingRate != lastSamplingRate) {
      oscDataL.resize(int(oscFrame * globalSamplingRate));
      oscDataR.resize(int(oscFrame * globalSamplingRate));
      lastSamplingRate = globalSamplingRate;
    }
  }

  oscDataL = granulator.oscBufferL.getArray(oscDataL.size());
  oscDataR = granulator.oscBufferR.getArray(oscDataR.size());
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);

  ImGui::SetCursorPosY(70 * adjustScaleY);
  ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)*SecondaryColor);
  ImGui::PlotLines("ScopeL", &oscDataL[0], oscDataL.size(), 0, nullptr, -1, 1,
                   ImVec2(0, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)),
                   sizeof(float));

  ImGui::SetCursorPosY(70 * adjustScaleY);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(255, 255, 255, 0));
  ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)*TertiaryColor);
  ImGui::PlotLines("ScopeR", &oscDataR[0], oscDataR.size(), 0, nullptr, -1, 1,
                   ImVec2(0, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)),
                   sizeof(float));

  ImGui::SetCursorPosY(70 * adjustScaleY);
  ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(0, 0, 0, 255));
  ImGui::PlotLines("black_line", &blackLine[0], 2, 0, nullptr, -1.0, 1.0,
                   ImVec2(0, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)),
                   sizeof(float));

  ImGui::PopItemWidth();
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw VU Meter window
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel(" ##VU Meter", windowWidth * 15 / 16,
                           windowHeight * 3 / 4 + (25 * adjustScaleY), windowWidth * 1 / 16,
                           windowHeight / 4, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  VUdataSize = globalSamplingRate / 30;  // Size of VU meter data arrays
  if (VUdataSize != lastVUdataSize) {    // resize VU meter data arrays if SR changed
    VUdataLeft.resize(VUdataSize);
    VUdataRight.resize(VUdataSize);
    lastVUdataSize = VUdataSize;
  }

  VUdataLeft = granulator.vuBufferL.getArray(VUdataSize);   // get left data from ringbuffer
  VUdataRight = granulator.vuBufferR.getArray(VUdataSize);  // get right data from ringbuffer

  // Calculate RMS
  float VUleft = std::accumulate(VUdataLeft.begin(), VUdataLeft.end(), 0.0f) / VUdataSize;
  VUleft = sqrt(VUleft);
  float VUright = std::accumulate(VUdataRight.begin(), VUdataRight.end(), 0.0f) / VUdataSize;
  VUright = sqrt(VUright);

  // set meter colors to green
  ImVec4 VUleftCol = (ImVec4)ImColor(0, 255, 0, 100);
  ImVec4 VUrightCol = (ImVec4)ImColor(0, 255, 0, 100);

  // Set meter colors to red if clipped
  if (granulator.clipL > 0) {
    VUleftCol = (ImVec4)ImColor(255, 0, 0, 100);
    granulator.clipL--;
  }
  if (granulator.clipR > 0) {
    VUrightCol = (ImVec4)ImColor(255, 0, 0, 100);
    granulator.clipR--;
  }

  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, VUleftCol);
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, VUleftCol);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, ((ImVec4)ImColor(0, 0, 0, 180)));
  ImGui::PlotHistogram(
    "##VUleft", &VUleft, 1, 0, nullptr, 0, 1,
    ImVec2((windowWidth * 1 / 32) - 12, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)),
    sizeof(float));
  ImGui::SameLine();
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, VUrightCol);
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, VUrightCol);
  ImGui::PlotHistogram(
    "##VUright", &VUright, 1, 0, nullptr, 0, 1,
    ImVec2((windowWidth * 1 / 32) - 12, ImGui::GetContentRegionAvail().y - (30 * adjustScaleY)),
    sizeof(float));
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(0, 0, 0, 150));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, (ImVec4)ImColor(1, 1, 1, 150));
  ParameterGUI::endPanel();

  // Throw popup to remind user to load in sound files if none are present.
  if (ImGui::BeginPopupModal("Load soundfiles please :,)", &noSoundFiles)) {
    ImGui::Text(
      "Files can be loaded in from the top left menu.\nAudio will "
      "turn on once a file has been loaded.");
    // ImGui::Text(execDir.c_str()); //DEBUG
    ImGui::EndPopup();
  }
  ImGui::PopFont();

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
      isPaused = true;
      io->stop();
      io->close();
      state.currentSr = getSampleRateIndex();
    }
  } else {
    if (ImGui::Button("Update Devices")) {
      updateDevices(state);
    }
    if (ImGui::Combo("Device", &state.currentDevice, ParameterGUI::vector_getter,
                     static_cast<void *>(&state.devices), state.devices.size())) {
      // TODO adjust valid number of channels.
    }
    std::vector<std::string> samplingRates{"44100", "48000", "88100", "96000"};
    ImGui::Combo("Sampling Rate", &state.currentSr, ParameterGUI::vector_getter,
                 static_cast<void *>(&samplingRates), samplingRates.size());
    if (ImGui::Button("Start")) {
      globalSamplingRate = std::stof(samplingRates[state.currentSr]);
      io->framesPerSecond(globalSamplingRate);
      io->framesPerBuffer(consts::BLOCK_SIZE);
      io->device(AudioDevice(state.currentDevice));
      granulator.setIO(io);
      if (writeSampleRate) jsonWriteToConfig(globalSamplingRate, consts::SAMPLE_RATE_KEY);

      granulator.resampleSoundFiles();

      io->open();
      io->start();
      isPaused = false;
    }
    ImGui::SameLine();
    ImGui::Checkbox("Set as Default", &writeSampleRate);
  }
  ImGui::PopID();
}

static void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate, uint32_t numChannels,
                               std::string directory, uint32_t bufferSize) {
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
  ImGui::Text("Output File Name:");
  static char buf1[64] = "test.wav";
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 10.0f);
  ImGui::InputText("##Record Name", buf1, 63);
  ImGui::PopItemWidth();

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
          filename =
            filename.substr(0, lastDot) + std::to_string(counter++) + filename.substr(lastDot);
        }
      }
      if (!recorder->start(directory + filename, frameRate, numChannels, ringBufferSize,
                           gam::SoundFile::WAV, gam::SoundFile::FLOAT)) {
        std::cerr << "Error opening file for record" << std::endl;
      }
    } else {
      recorder->close();
    }
  }
  ImGui::SameLine();
  ImGui::Checkbox("Overwrite", &state.overwriteButton);
  ImGui::TextWrapped("Writing to:\n %s", directory.c_str());
  ImGui::PopID();
}

void ecInterface::drawLFOcontrol(ecSynth &synth, int lfoNumber) {
  ImGui::Text("LFO%i", lfoNumber + 1);
  ImGui::SameLine();
  ImGui::SetCursorPosX(50 * ImGui::GetIO().FontGlobalScale);
  ImGui::PushItemWidth(70 * ImGui::GetIO().FontGlobalScale);
  ParameterGUI::drawMenu(synth.LFOparameters[lfoNumber]->shape);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  ImGui::PushItemWidth(60 * ImGui::GetIO().FontGlobalScale);
  ParameterGUI::drawMenu(synth.LFOparameters[lfoNumber]->polarity);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  synth.LFOparameters[lfoNumber]->frequency->drawRangeSlider(fontScale, consts::LFO);
  // ParameterGUI::drawParameter(synth.LFOparameters[lfoNumber]->frequency);

  if (*synth.LFOparameters[lfoNumber]->shape == 1) {
    ImGui::Text("Duty");
    ImGui::SameLine();
    ImGui::SetCursorPosX(50 * fontScale);
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (25 * fontScale));
    ParameterGUI::drawParameter(synth.LFOparameters[lfoNumber]->duty);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::Text("Hz");
  }
}

void ecInterface::drawModulationControl(al::ParameterMenu &menu, ecParameter &slider) {
  ImGui::PushItemWidth(70 * ImGui::GetIO().FontGlobalScale);
  ParameterGUI::drawMenu(&menu);
  ImGui::PopItemWidth();
  ImGui::SameLine();
  slider.drawRangeSlider(fontScale, consts::MOD);
}

void ecInterface::setGUIColors() {
  ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)*PrimaryColor);
  ImGui::PushStyleColor(ImGuiCol_PopupBg, (ImVec4)*PrimaryColor);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_MenuBarBg, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)ImColor(0, 0, 0, 130));
  ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, (ImVec4)ImColor(0, 0, 0, 150));
  ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_Header, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_HeaderHovered, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_HeaderActive, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_TitleBg, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_TitleBgActive, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, (ImVec4)ImColor(0, 0, 0, 150));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, (ImVec4)ImColor(255, 255, 255, 150));
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)*Text);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
}

int ecInterface::getSampleRateIndex() {
  unsigned s_r = (unsigned)globalSamplingRate;
  switch (s_r) {
    case 44100:
      return 0;
    case 48000:
      return 1;
    case 88100:
      return 2;
    case 96000:
      return 3;
    default:
      return 0;
  }
}

/**** Configuration File Stuff -- Implementation****/

bool ecInterface::initJsonConfig() {
  json config;
  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open()) {
    config = json::parse(ifs);

    if (config.find(consts::SOUND_OUTPUT_PATH_KEY) == config.end())
      config[consts::SOUND_OUTPUT_PATH_KEY] =
        f.conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    if (config.find(consts::SAMPLE_RATE_KEY) == config.end())
      config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

    if (config.find(consts::LIGHT_MODE_KEY) == config.end())
      config[consts::LIGHT_MODE_KEY] = consts::LIGHT_MODE;

  } else {
    config[consts::SOUND_OUTPUT_PATH_KEY] =
      f.conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

    config[consts::LIGHT_MODE_KEY] = consts::LIGHT_MODE;
  }

  std::ofstream file((userPath + configFile).c_str());
  if (file.is_open()) file << config;

  return false;
}

void ecInterface::initFileIOPaths() {
  initJsonConfig();
  jsonReadAndSetSoundOutputPath();
}

template <typename T>
bool ecInterface::jsonWriteToConfig(T value, std::string key) {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open()) config = json::parse(ifs);

  config[key] = value;

  std::ofstream file((userPath + configFile).c_str());

  if (file.is_open()) {
    file << config;
    return true;
  } else {
    return false;
  }
}

void ecInterface::jsonReadAndSetColorSchemeMode() {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open())
    config = json::parse(ifs);
  else
    return;

  light = config.at(consts::LIGHT_MODE_KEY);
  if (!light) {
    PrimaryColor = &PrimaryDark;
    SecondaryColor = &SecondaryDark;
    TertiaryColor = &TertiaryDark;
    Shade1 = &Shade1Dark;
    Shade2 = &Shade2Dark;
    Shade3 = &Shade3Dark;
    Text = &TextDark;
  } else {
    PrimaryColor = &PrimaryLight;
    SecondaryColor = &SecondaryLight;
    TertiaryColor = &TertiaryLight;
    Shade1 = &Shade1Light;
    Shade2 = &Shade2Light;
    Shade3 = &Shade3Light;
    Text = &TextLight;
  }
}

void ecInterface::jsonReadAndSetSoundOutputPath() {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open())
    config = json::parse(ifs);
  else
    return;

  soundOutput = f.conformPathToOS(config.at(consts::SOUND_OUTPUT_PATH_KEY));
}

void ecInterface::jsonReadAndSetAudioSettings() {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open())
    config = json::parse(ifs);
  else
    return;

  globalSamplingRate = config.at(consts::SAMPLE_RATE_KEY);

  configureAudio(globalSamplingRate, consts::BLOCK_SIZE, consts::AUDIO_OUTS, consts::DEVICE_NUM);

  granulator.setIO(&audioIO());
}

/**** Custom Preset Drawer -- Implementation****/
/**** Borrowed and modified from al_ParameterGUI.cpp****/
ecInterface::PresetHandlerState &ecInterface::ECdrawPresetHandler(PresetHandler *presetHandler,
                                                                  int presetColumns,
                                                                  int presetRows) {
  static std::map<PresetHandler *, ecInterface::PresetHandlerState> stateMap;
  if (stateMap.find(presetHandler) == stateMap.end()) {
    //        std::cout << "Created state for " << (unsigned long)
    //        presetHandler
    //        << std::endl;
    stateMap[presetHandler] =
      ecInterface::PresetHandlerState{"", 0, presetHandler->availablePresetMaps()};
    if (stateMap[presetHandler].mapList.size() > 0) {
      stateMap[presetHandler].currentBank = stateMap[presetHandler].mapList[0];
      stateMap[presetHandler].currentBankIndex = 0;
    }
    presetHandler->registerPresetMapCallback(
      [&](std::string mapName) { stateMap[presetHandler].currentBank = mapName; });
  }
  ecInterface::PresetHandlerState &state = stateMap[presetHandler];
  float fontSize = ImGui::GetFontSize();

  std::string id = std::to_string((unsigned long)presetHandler);
  std::string suffix = "##PresetHandler" + id;
  ImGui::PushID(suffix.c_str());

  int selection = presetHandler->getCurrentPresetIndex();
  std::string currentPresetName = presetHandler->getCurrentPresetName();
  if (currentPresetName.length() == 0) currentPresetName = "none";
  ImGui::Text("Current Preset: %s", currentPresetName.c_str());
  int counter = state.presetHandlerBank * (presetColumns * presetRows);
  if (state.storeButtonState) {
    ImGui::PushStyleColor(ImGuiCol_Text, 0xff0000ff);
  }
  float presetWidth = (ImGui::GetContentRegionAvail().x / 12.0f) - 8.0f;
  for (int row = 0; row < presetRows; row++) {
    for (int column = 0; column < presetColumns; column++) {
      std::string name = std::to_string(counter);
      ImGui::PushID(counter);

      bool is_selected = selection == counter;
      if (is_selected) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
      }
      if (ImGui::Selectable(name.c_str(), is_selected, 0, ImVec2(presetWidth, fontSize * 1.2f))) {
        if (state.storeButtonState) {
          std::string saveName = state.enteredText;
          if (saveName.size() == 0) {
            saveName = name;
          }
          presetHandler->storePreset(counter, saveName.c_str());
          selection = counter;
          state.storeButtonState = false;
          ImGui::PopStyleColor();
          state.enteredText.clear();
        } else {
          if (presetHandler->recallPreset(counter) != "") {  // Preset is available
            selection = counter;
          }
        }
      }
      if (is_selected) {
        ImGui::PopStyleColor(1);
      }
      //                if (ImGui::IsItemHovered()) {
      //                    ImGui::SetTooltip("I am a tooltip");
      //                }
      if (column < presetColumns - 1) ImGui::SameLine();
      counter++;
      ImGui::PopID();
    }
  }
  if (state.storeButtonState) {
    ImGui::PopStyleColor();
  }
  if (ImGui::Button("<-")) {
    state.presetHandlerBank -= 1;
    if (state.presetHandlerBank < 0) {
      state.presetHandlerBank = 4;
    }
  }
  ImGui::SameLine();
  if (ImGui::Button("->")) {
    state.presetHandlerBank += 1;
    if (state.presetHandlerBank > 4) {
      state.presetHandlerBank = 0;
    }
  }
  ImGui::SameLine(0.0f, 40.0f);

  if (state.storeButtonState) {
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0, 0.0, 0.0, 1.0));
  }
  std::string storeText = state.storeButtonState ? "Cancel" : "Store";
  bool storeButtonPressed = ImGui::Button(storeText.c_str(), ImVec2(100, 0));
  if (state.storeButtonState) {
    ImGui::PopStyleColor();
  }
  if (storeButtonPressed) {
    state.storeButtonState = !state.storeButtonState;
    //          if (state.storeButtonState) {
    //            state.enteredText = currentPresetName;
    //          }
  }
  if (state.storeButtonState) {
    char buf1[64];
    strncpy(buf1, state.enteredText.c_str(), 63);
    ImGui::Text("Store preset as:");
    ImGui::SameLine();
    if (ImGui::InputText("preset", buf1, 64)) {
      state.enteredText = buf1;
    }
    ImGui::Text("Click on a preset number to store.");
  } else {
    std::vector<std::string> mapList = presetHandler->availablePresetMaps();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
    if (ImGui::BeginCombo("Preset Map", state.currentBank.data())) {
      stateMap[presetHandler].mapList = presetHandler->availablePresetMaps();
      for (auto mapName : stateMap[presetHandler].mapList) {
        bool isSelected = (state.currentBank == mapName);
        if (ImGui::Selectable(mapName.data(), isSelected)) {
          state.currentBank = mapName;
          presetHandler->setCurrentPresetMap(mapName);
        }
        if (isSelected) {
          ImGui::SetItemDefaultFocus();
        }
      }
      ImGui::EndCombo();
    }
    if (!state.newMap) {
      ImGui::SameLine();
      if (ImGui::Button("+")) {
        state.newMap = true;
      }
    } else {
      char buf2[64];
      strncpy(buf2, state.newMapText.c_str(), 63);
      ImGui::Text("New map:");
      //              ImGui::SameLine();
      ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.5f);
      if (ImGui::InputText("", buf2, 64)) {
        state.newMapText = buf2;
      }
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (ImGui::Button("Create")) {
        auto path =
          File::conformDirectory(presetHandler->getCurrentPath()) + state.newMapText + ".presetMap";
        // Create an empty file
        std::ofstream file;
        file.open(path, std::ios::out);
        file.close();
        state.newMap = false;
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        state.newMapText = "";
        state.newMap = false;
      }
    }
    // TODO options to create new bank
    //        ImGui::SameLine();
    //          ImGui::PopItemWidth();
    ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
    float morphTime = presetHandler->getMorphTime();
    if (ImGui::InputFloat("morph time", &morphTime, 0.0f, 20.0f)) {
      presetHandler->setMorphTime(morphTime);
    }
    ImGui::PopItemWidth();
  }

  //            ImGui::Text("%s", currentPresetName.c_str());
  ImGui::PopID();
  return state;
}
