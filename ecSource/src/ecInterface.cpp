// ecInterface.cpp

/**** Emission Control LIB ****/
#include "ecInterface.h"

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
  title("EmissionControl2");

  execDir = al::File::directory(util::getExecutablePath());
  userPath = util::getUserHomePath();
#ifdef __APPLE__  // Uses userPath
  configFile = consts::DEFAULT_CONFIG_FILE;
  presetsPath = consts::DEFAULT_PRESETS_PATH;
  midiPresetsPath = consts::DEFAULT_MIDI_PRESETS_PATH;
  oscPresetsPath = consts::DEFAULT_OSC_PRESETS_PATH;

  samplePresetsPath = consts::DEFAULT_SAMPLE_PRESETS_PATH;

  execDir = util::getContentPath_OSX(execDir);
  al::Dir::make(userPath + consts::PERSISTENT_DATA_PATH);
  al::Dir::make(userPath + consts::DEFAULT_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_MIDI_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_OSC_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);
  al::Dir::make(userPath + consts::DEFAULT_CONFIG_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PRESETS_PATH);
  opener = "open ";
#endif

#ifdef __linux__
  std::string configPath = "/.config/EmissionControl2";
  // use xdg directories if available
  if (getenv("$XDG_CONFIG_HOME") != NULL) {
    configPath = getenv("$XDG_CONFIG_HOME") + std::string("/EmissionControl2");
  }

  configFile = configPath + "/config/config.json";
  presetsPath = configPath + "/presets";
  midiPresetsPath = configPath + "/midi_presets/";
  oscPresetsPath = configPath + "/osc_presets/";
  samplePresetsPath = configPath + "/sample_presets/";

  // create config directories if needed
  al::Dir::make(userPath + configPath + "/config");
  al::Dir::make(userPath + presetsPath);
  al::Dir::make(userPath + midiPresetsPath);
  al::Dir::make(userPath + oscPresetsPath);
  al::Dir::make(userPath + samplePresetsPath);
  opener = "xdg-open ";
#endif

#ifdef _WIN32
  configFile = consts::DEFAULT_CONFIG_FILE;
  presetsPath = consts::DEFAULT_PRESETS_PATH;
  midiPresetsPath = consts::DEFAULT_MIDI_PRESETS_PATH;
  oscPresetsPath = consts::DEFAULT_OSC_PRESETS_PATH;
  samplePresetsPath = consts::DEFAULT_SAMPLE_PRESETS_PATH;

  al::Dir::make(userPath + consts::PERSISTENT_DATA_PATH);
  al::Dir::make(userPath + consts::DEFAULT_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_MIDI_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_OSC_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);
  al::Dir::make(userPath + consts::DEFAULT_CONFIG_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PRESETS_PATH);
  opener = "start ";
#endif

  initJsonConfig();
  json config = jsonReadConfig();
  setMIDIPresetNames(config.at(consts::MIDI_PRESET_NAMES_KEY));
  setOSCPresetNames(config.at(consts::OSC_PRESET_NAMES_KEY));
  setSoundFilePresetNames(config.at(consts::SAMPLE_PRESET_NAMES_KEY));
  setSoundOutputPath(config.at(consts::SOUND_OUTPUT_PATH_KEY));
  setAudioSettings(config.at(consts::SAMPLE_RATE_KEY), config.at(consts::BUFFER_SIZE_KEY));
  setColorSchemeMode(config.at(consts::LIGHT_MODE_KEY));
  setFontScale(config.at(consts::FONT_SCALE_KEY));
  setWindowDimensions(config.at(consts::WINDOW_WIDTH_KEY), config.at(consts::WINDOW_HEIGHT_KEY));
  setFirstLaunch(config.at(consts::IS_FIRST_LAUNCH_KEY));
  setAudioDevice(config.at(consts::DEFAULT_AUDIO_DEVICE_KEY));
  setHardClip(config.at(consts::CLIP_AUDIO_KEY));
  setOmitSoundFileParam(config.at(OMIT_SOUNDFILE_PARAM_KEY));
  setHardResetScanBegin(config.at(consts::HARD_RESET_SCANBEGIN_KEY));

  setInitFullscreen(false);

// Load in all files in at specified directory.
// Set output directory for presets.
// Set output directory of recorded files.
#if defined __APPLE__ || defined _WIN32
  if (isFirstLaunch)
    al::File::copy(execDir + "Resources/samples/440sine48k.wav",
                   userPath + consts::DEFAULT_SAMPLE_PATH);
  granulator.loadInitSoundFiles(userPath + consts::DEFAULT_SAMPLE_PATH);
#endif

#ifdef __linux__
  granulator.loadInitSoundFiles(consts::DEFAULT_SAMPLE_PATH);
#endif

  mPresets = std::make_unique<al::PresetHandler>(al::File::conformPathToOS(userPath + presetsPath));
  auto path = mPresets->buildMapPath("default");  // build default map if it doesn't exist
  std::ifstream exist(path);
  if (!exist.good()) {
    std::ofstream file;
    file.open(path, std::ios::out);
    file.close();
  }
  mPresets->setMaxMorphTime(consts::MAX_MORPH_TIME);

  for (int i = 0; i < granulator.soundClip.size(); i++)
    createAudioThumbnail(granulator.soundClip[i]->data, granulator.soundClip[i]->size);
  initMIDI();

  audioIO().setStreamName("EmissionControl2");
  auto a_d = AudioDevice(currentAudioDevice, AudioDevice::OUTPUT);
  if (!a_d.valid()) {
    audioIO().deviceOut(-1);
    currentAudioDevice = AudioDevice::defaultOutput().name();
    granulator.setOutChannels(0, audioIO().channelsOutDevice());
  } else {
    audioIO().deviceOut(a_d);
    granulator.setOutChannels(config.at(consts::LEAD_CHANNEL_KEY), audioIO().channelsOutDevice());
    // TODO, make sure only 2 channels are open corresponding to out channels
    // audioIO().channelsOut({(int)config.at(consts::LEAD_CHANNEL_KEY),(int)config.at(consts::LEAD_CHANNEL_KEY)
    // + 1});
  }
  audioIO().channelsIn(0);
  gam::sampleRate(granulator.getGlobalSamplingRate());
  granulator.initialize(&audioIO());
  audioIO().append(mRecorder);
  audioIO().clipOut(isHardClip);

  audioIO().print();
  std::cout << "Sample Rate: " + std::to_string((int)audioIO().framesPerSecond()) << std::endl;
}

void ecInterface::onCreate() {
  audioIO().close();
  audioIO().open();
  audioIO().stop();

  al::imguiInit();

  // Set if fullscreen or not.
  fullScreen(isFullScreen);

  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    granulator.ECParameters[index]->addToPresetHandler(*mPresets);
    granulator.ECModParameters[index]->addToPresetHandler(*mPresets);
  }

  for (int i = 0; i < consts::NUM_LFOS; i++) {
    granulator.LFOParameters[i]->frequency->addToPresetHandler(*mPresets);
    *mPresets << *granulator.LFOParameters[i]->shape << *granulator.LFOParameters[i]->duty
              << *granulator.LFOParameters[i]->polarity;
  }

  // Decide if we should omit the sound file parameter.
  granulator.ECParameters[consts::SOUND_FILE]->skipParamPresetHandler(*mPresets,
                                                                      isOmitSoundFileParam);
  granulator.ECModParameters[consts::SOUND_FILE]->skipParamPresetHandler(*mPresets,
                                                                         isOmitSoundFileParam);

  ImFontConfig fontConfig;
  fontConfig.OversampleH = 4;
  fontConfig.OversampleV = 4;

  bodyFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
    &RobotoMedium_compressed_data, RobotoMedium_compressed_size, 16, &fontConfig);
  titleFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
    &RobotoMedium_compressed_data, RobotoMedium_compressed_size, 20, &fontConfig);
  engineFont = ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(
    &engine_compressed_data, engine_compressed_size, 16, &fontConfig);

  currentPresetMap = mPresets->readPresetMap("default");
  setGUIParams();
  // std::cout << "LOOKIE HERE: " << std::endl;
}

void ecInterface::onExit() {
  jsonWriteToConfig(windowWidth, consts::WINDOW_WIDTH_KEY);
  jsonWriteToConfig(windowHeight, consts::WINDOW_HEIGHT_KEY);
  jsonWriteToConfig(isFullScreen, consts::FULLSCREEN_KEY);
  jsonWriteToConfig(false, consts::IS_FIRST_LAUNCH_KEY);
}

void ecInterface::onSound(AudioIOData &io) { granulator.onProcess(io); }

void ecInterface::onDraw(Graphics &g) {
  if (mLastKeyDown.key.key() == static_cast<int>(consts::KEYBOARD_TOGGLE_ENGINE) &&
      mLastKeyDown.readyToTrig && !captureKeyboardTextInput) {
    if (audioIO().isRunning()) {
      audioIO().stop();
    } else {
      audioIO().start();
    }
  }

  setGUIParams();
  framecounter++;
  if (unlearnFlash > 0) unlearnFlash--;
  g.clear(background);

  // Get window height and width (for responsive sizing)
  windowWidth = width();
  windowHeight = height();

  // Get Window Full Screen ; BROKEN
  // if (fullScreen() != isFullScreen)
  //   isFullScreen = fullScreen();

  // Initialize Audio IO popup to false
  bool displayIO = false;

  // Initialize Font scale popup to false
  bool fontScaleWindow = false;

  // Initialize About window to false
  bool aboutWindow = false;

  // Initialize MIDI windows to false
  bool isMIDIWriteWindow = false;
  bool isMIDILoadWindow = false;
  bool isMIDIDeleteWindow = false;
  bool isMIDIDevicesWindow = false;
  bool isMIDIHelpWindow = false;

  // Initialize OSC window to false
  bool isOSCConfigWindow = false;
  bool isOSCWriteWindow = false;
  bool isOSCLoadWindow = false;
  bool isOSCDeleteWindow = false;

  bool isSoundFilePresetWriteWindow = false;
  bool isSoundFilePresetLoadWindow = false;
  bool isSoundFilePresetDeleteWindow = false;

  al::imguiBeginFrame();

  ImGui::GetIO().FontGlobalScale = fontScale;

  adjustScaleY = 1.0f + ((fontScale - 1.0f) / 1.5f);

  // adjust frame padding for smaller screens
  if (ImGui::GetFrameHeightWithSpacing() * 16.8 > windowHeight / 2)
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));

  // get slider thickness
  float sliderheight = ImGui::GetFrameHeightWithSpacing();

  float menuBarHeight = 22 * adjustScaleY;
  float firstRowHeight = sliderheight * 16.9;
  float secondRowHeight = sliderheight * 8;

  // adjust second row size to fill space if graphs are not drawn
  if (windowHeight - menuBarHeight - firstRowHeight - secondRowHeight < 100)
    secondRowHeight = windowHeight - firstRowHeight - menuBarHeight;

  if (granulator.getNumberOfAudioFiles() == 0 && readyToTrigNoSoundFilePopup) {
    ImGui::OpenPopup("No Sound File");
    if (audioIO().isRunning()) audioIO().stop();
    // Edge case where we need to redraw pop up if triggered on first frame.
    // Happens because the 'no sound file' popup is drawn under everything.
    // Sequential coding makes GUI programming confusing :(
    if (firstFrame)
      readyToTrigNoSoundFilePopup = true;
    else
      readyToTrigNoSoundFilePopup = false;
  }

  if (granulator.getNumberOfAudioFiles() != 0) {
    readyToTrigNoSoundFilePopup = true;
  }

  // Throw popup to remind user to load in sound files if none are
  // present.
  if (ImGui::BeginPopupModal("No Sound File")) {
    isPaused = true;
    ImGui::Text("Load a sound file to continue using EmissionControl");
    if (ImGui::Button("Load Sound File")) {
      result = NFD_OpenDialogMultiple("wav;aiff;aif", NULL, &pathSet);

      if (result == NFD_OKAY) {
        size_t i;
        for (i = 0; i < NFD_PathSet_GetCount(&pathSet); ++i) {
          nfdchar_t *path = NFD_PathSet_GetPath(&pathSet, i);
          bool success = granulator.loadSoundFileRT(path);
          if (success) {
            createAudioThumbnail(granulator.soundClip.back()->data,
                                 granulator.soundClip.back()->size);
            readyToTrigNoSoundFilePopup = true;
          } else {
            std::cerr << "Failed to load: " << path << std::endl;
          }
        }
        NFD_PathSet_Free(&pathSet);
      }
      if (readyToTrigNoSoundFilePopup) ImGui::CloseCurrentPopup();
    }
    if (ImGui::Button("Load Sound File Preset")) {
      isSoundFilePresetLoadWindow = true;
    }
    ImGui::EndPopup();
  }

  // Draw GUI

  // draw menu bar ----------------------------------------------------
  // static bool show_app_main_menu_bar = true;
  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("Audio")) {
      if (ImGui::MenuItem("Audio Output", "")) {
        displayIO = true;
      }
      if (ImGui::MenuItem("Sound Recording Folder", "")) {
        result = NFD_PickFolder(NULL, &outPath);

        if (result == NFD_OKAY) {
          std::string temp = outPath;
          jsonWriteToConfig(temp, consts::SOUND_OUTPUT_PATH_KEY);
          setSoundOutputPath(outPath);
        }
      }
      if (ImGui::Checkbox("Clip Audio", &isHardClip)) {
        audioIO().clipOut(isHardClip);
        jsonWriteToConfig(isHardClip, consts::CLIP_AUDIO_KEY);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Sound Files")) {
      if (ImGui::MenuItem("Load Sound File", "")) {
        ImGui::Text("%s", currentFile.c_str());

        // When the select file button is clicked, the file selector is shown

        result = NFD_OpenDialogMultiple("wav;aiff;aif", NULL, &pathSet);

        if (result == NFD_OKAY) {
          size_t i;
          for (i = 0; i < NFD_PathSet_GetCount(&pathSet); ++i) {
            nfdchar_t *path = NFD_PathSet_GetPath(&pathSet, i);
            bool success = granulator.loadSoundFileRT(path);
            if (success)
              createAudioThumbnail(granulator.soundClip.back()->data,
                                   granulator.soundClip.back()->size);
            else
              std::cerr << "Failed to load: " << path << std::endl;
          }
          NFD_PathSet_Free(&pathSet);
        }
      }
      if (ImGui::MenuItem("Remove Current Sound File", "")) {
        audioThumbnails.erase(audioThumbnails.begin() + granulator.mModClip);
        granulator.removeCurrentSoundFile();
      }
      if (ImGui::MenuItem("Clear All Sound Files", "")) {
        int clipsToRemove = granulator.mClipNum;
        for (int i = 0; i < clipsToRemove; i++) {
          audioThumbnails.erase(audioThumbnails.begin() + i);
          granulator.removeSoundFile(i);
        }
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Save Sound File Preset", "")) {
        isSoundFilePresetWriteWindow = true;
      }
      if (ImGui::MenuItem("Load Sound File Preset", "")) {
        isSoundFilePresetLoadWindow = true;
      }
      if (ImGui::MenuItem("Delete Sound File Preset", "")) {
        isSoundFilePresetDeleteWindow = true;
      }
      ImGui::EndMenu();
    }

    // BEGIN WRITE SOUND FILE PRESET
    if (isSoundFilePresetWriteWindow) {
      ImGui::OpenPopup("Save Sound File Preset");
    }
    bool isSoundFilePresetWriteOpen = true;
    bool isWriteJSON = false;
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));
    if (ImGui::BeginPopupModal("Save Sound File Preset", &isSoundFilePresetWriteOpen)) {
      ImGui::InputText("Enter Preset Name", mCurrentSoundFilePresetName, 50,
                       ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_CharsNoBlank);
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
      }

      if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

      ImGui::SameLine();

      if (ImGui::Button("Save")) {
        ImGui::CloseCurrentPopup();
        isSoundFilePresetWriteOpen = false;
        isWriteJSON = true;
      }

      ImGui::SameLine();

      ImGui::Checkbox("Overwrite", &allowSoundFilePresetOverwrite);

      ImGui::EndPopup();
    }
    if (!isSoundFilePresetWriteOpen && isWriteJSON) {
      writeJSONSoundFilePreset(mCurrentSoundFilePresetName, allowSoundFilePresetOverwrite);
      isWriteJSON = false;
    }
    // END WRITE SOUND FILE PRESET

    // BEGIN LOAD SOUND FILE PRESET
    if (isSoundFilePresetLoadWindow) {
      ImGui::OpenPopup("Load Sound File Preset");
    }
    // Sound File Load Preset Window
    bool isSoundFileLoadOpen = true;
    bool isLoadJSON = false;
    std::string sound_file_preset_name = "";
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));

    if (ImGui::BeginPopupModal("Load Sound File Preset", &isSoundFileLoadOpen)) {
      for (auto iter = SoundFilePresetNames.begin(); iter != SoundFilePresetNames.end(); iter++) {
        if (ImGui::Selectable(iter->c_str())) {
          isSoundFileLoadOpen = false;
          isLoadJSON = true;
          sound_file_preset_name = *iter;
        }
      }
      ImGui::EndPopup();
    }

    if (!isSoundFileLoadOpen && isLoadJSON) {
      failed_paths = loadJSONSoundFilePreset(sound_file_preset_name);
      isLoadJSON = false;
    }

    if (failed_paths.size() != 0) {
      ImGui::OpenPopup("Failed Paths");
    }
    bool plsGiveMeAnXImGui = true;
    if (ImGui::BeginPopupModal("Failed Paths", &plsGiveMeAnXImGui)) {
      ImGui::Text("Sound Files Not Found:");
      for (int index = 0; index < failed_paths.size(); index++) {
        ImGui::Text("%s", failed_paths[index].c_str());
      }
      ImGui::EndPopup();
    }
    if (!plsGiveMeAnXImGui) {
      failed_paths.clear();
    }

    if ((!isSoundFileLoadOpen || !plsGiveMeAnXImGui) && granulator.getNumberOfAudioFiles() == 0)
      readyToTrigNoSoundFilePopup = true;
    // END LOAD SOUND FILE PRESET

    // BEGIN DELETE SOUND FILE PRESET
    if (isSoundFilePresetDeleteWindow) {
      ImGui::OpenPopup("Delete Sound File Preset");
    }
    bool isSoundFilePresetDeleteOpen = true;
    bool isDeleteJSON = false;
    sound_file_preset_name = "";
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));
    if (ImGui::BeginPopupModal("Delete Sound File Preset", &isSoundFilePresetDeleteOpen)) {
      for (auto iter = SoundFilePresetNames.begin(); iter != SoundFilePresetNames.end(); iter++) {
        if (ImGui::Selectable(iter->c_str())) {
          isSoundFilePresetDeleteOpen = false;
          isDeleteJSON = true;
          sound_file_preset_name = *iter;
        }
      }
      ImGui::EndPopup();
    }
    if (!isSoundFilePresetDeleteOpen && isDeleteJSON) {
      deleteJSONSoundFilePreset(sound_file_preset_name);
      isDeleteJSON = false;
    }
    // END DELETE SOUND FILE PRESET
    // END SOUND FILE PRESETS

    if (ImGui::BeginMenu("MIDI/OSC")) {
      if (ImGui::MenuItem("MIDI Devices", "")) {
        MIDIMessageHandler::clearBindings();
        for (int index = 0; index < midiIn.size(); index++) {
          if (midiIn[index].isPortOpen()) {
            midiIn[index].closePort();
          }
        }
        SelectedMIDIDevices.resize(midiIn[0].getPortCount());
        isMIDIDevicesWindow = true;
      }

      if (ImGui::MenuItem("Clear MIDI Learn", "")) {
        clearActiveMIDI();
      }
      ImGui::Separator();
      if (ImGui::MenuItem("Save MIDI Learn Preset", "")) {
        isMIDIWriteWindow = true;
      }

      if (ImGui::MenuItem("Load MIDI Learn Preset", "")) {
        isMIDILoadWindow = true;
      }

      if (ImGui::MenuItem("Delete MIDI Learn Preset", "")) {
        isMIDIDeleteWindow = true;
      }
      if (ImGui::MenuItem("MIDI Learn Help", "")) {
        isMIDIHelpWindow = true;
      }
      ImGui::Separator();
      if (ImGui::Checkbox("OSC On", &isOSCOn)) {
        if (isOSCOn) {
          resetOSC();
        } else {
          oscServer->stop();
        }
      }
      if (ImGui::MenuItem("OSC Config", "")) {
        isOSCConfigWindow = true;
      }

      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Control Preferences")) {
      if (ImGui::Checkbox("Omit 'Sound File' from Presets", &isOmitSoundFileParam)) {
        granulator.ECParameters[consts::SOUND_FILE]->skipParamPresetHandler(*mPresets,
                                                                            isOmitSoundFileParam);
        granulator.ECModParameters[consts::SOUND_FILE]->skipParamPresetHandler(
          *mPresets, isOmitSoundFileParam);
        jsonWriteToConfig(isOmitSoundFileParam, consts::OMIT_SOUNDFILE_PARAM_KEY);
      }

      if (ImGui::Checkbox("Hard Reset 'Scan Begin'", &isHardResetScanBegin)) {
        granulator.setHardScanBegin(isHardResetScanBegin);
        jsonWriteToConfig(isHardResetScanBegin, consts::HARD_RESET_SCANBEGIN_KEY);
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
      if (ImGui::MenuItem("Toggle Light/Dark Mode", "")) {
        if (light) {
          PrimaryColor = &PrimaryDark;
          ECyellow = &YellowDark;
          ECred = &RedDark;
          ECgreen = &GreenDark;
          ECblue = &BlueDark;
          Shade1 = &Shade1Dark;
          Shade2 = &Shade2Dark;
          Shade3 = &Shade3Dark;
          Text = &TextDark;
          light = false;
        } else {
          PrimaryColor = &PrimaryLight;
          ECyellow = &YellowLight;
          ECred = &RedLight;
          ECgreen = &GreenLight;
          ECblue = &BlueLight;
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
    if (ImGui::BeginMenu("Help")) {
      if (ImGui::MenuItem("Manual", "")) {
        // open url to manual on github
        system((opener + manualURL).c_str());
      }
      if (ImGui::MenuItem("Submit Bug Report", "")) {
        // open url to manual on github
        system((opener + bugReportURL).c_str());
      }
      if (ImGui::MenuItem("About", "")) {
        aboutWindow = true;
      }
      ImGui::EndMenu();
    }
    if (audioIO().isRunning()) {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));

    } else {
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
    }
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(*ECred));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0.5, 0.5, 1.0));
    ImGui::PushFont(engineFont);
    // ImGui::SetCursorPosX(width() - 106 * fontScale);
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 20);
    if (!audioIO().isRunning()) {
      if (ImGui::Button("ENGINE START")) {
        if (granulator.getNumberOfAudioFiles() != 0) {
          audioIO().open();
          audioIO().start();
          isPaused = false;
          readyToTrigNoSoundFilePopup = true;
        }
      }
    } else {
      if (ImGui::Button("ENGINE STOP")) {
        audioIO().stop();
        audioIO().close();
        isPaused = true;
      }
    }
    ImGui::PopStyleColor(3);
    ImGui::PopFont();

    ImGui::EndMainMenuBar();
  }

  // DRAW MIDI DEVICE WINDOW
  if (isMIDIDevicesWindow) {
    ImGui::OpenPopup("MIDI Devices");
  }
  bool isMIDIDevicesOpen = true;
  bool isWriteMIDIDevices = false;
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("MIDI Devices", &isMIDIDevicesOpen)) {
    ImGui::Text("Select Up to %s MIDI Inputs:", std::to_string(consts::MAX_MIDI_IN).c_str());
    if (midiIn[0].getPortCount() != SelectedMIDIDevices.size()) {
      SelectedMIDIDevices.resize(midiIn[0].getPortCount());
    }
    int truth_count = 0;
    bool setDeviceFalse = false;
    if (midiIn[0].getPortCount() > 0) {
      for (int index = 0; index < midiIn[0].getPortCount(); index++) {
        bool temp = SelectedMIDIDevices[index];
        if (temp) {
          truth_count++;

          // Limit the amount of MIDI devices allowed.
          if (truth_count > consts::MAX_MIDI_IN) {
            truth_count--;
            temp = 0;
            setDeviceFalse = true;
          }
        }
        ImGui::Checkbox(midiIn[0].getPortName(index).c_str(), &temp);

        // Make sure device doesn't connect in the backend.
        if (setDeviceFalse) {
          SelectedMIDIDevices[index] = 0;
          setDeviceFalse = false;
        } else {
          SelectedMIDIDevices[index] = temp;
        }
      }
    } else {
      ImGui::Text("No MIDI devices found.\n");
    }
    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::SameLine();

    if (ImGui::Button("Confirm")) {
      ImGui::CloseCurrentPopup();
      isMIDIDevicesOpen = false;
      isWriteMIDIDevices = true;
    }
    ImGui::EndPopup();
  }

  // On Close of MIDI Device Window
  if (!isMIDIDevicesOpen && isWriteMIDIDevices) {
    int temp_counter = -1;
    for (int index = 0; index < SelectedMIDIDevices.size(); index++) {
      if (SelectedMIDIDevices[index]) {
        temp_counter++;
        if (temp_counter >= consts::MAX_MIDI_IN) break;
        MIDIMessageHandler::bindTo(midiIn[temp_counter], index);
        midiIn[temp_counter].openPort(index, midiIn[temp_counter].getPortName(index));
        printf("Opened port to %s\n", midiIn[temp_counter].getPortName(index).c_str());
      }
    }
  }

  // END DRAW MIDI DEVICE WINDOW

  if (isMIDILoadWindow) {
    ImGui::OpenPopup("Load MIDI Preset");
  }
  // MIDI Load Preset Window
  bool isMIDILoadOpen = true;
  bool isLoadJSON = false;
  std::string midi_preset_name = "";
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Load MIDI Preset", &isMIDILoadOpen)) {
    for (auto iter = MIDIPresetNames.begin(); iter != MIDIPresetNames.end(); iter++) {
      if (ImGui::Selectable(iter->c_str())) {
        isMIDILoadOpen = false;
        isLoadJSON = true;
        midi_preset_name = *iter;
      }
    }
    ImGui::EndPopup();
  }
  if (!isMIDILoadOpen && isLoadJSON) {
    clearActiveMIDI();
    loadJSONMIDIPreset(midi_preset_name);
    isLoadJSON = false;
  }

  // MIDI Delete Preset Window
  if (isMIDIDeleteWindow) {
    ImGui::OpenPopup("Delete MIDI Preset");
  }
  bool isMIDIDeleteOpen = true;
  bool isDeleteJSON = false;
  midi_preset_name = "";
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Delete MIDI Preset", &isMIDIDeleteOpen)) {
    for (auto iter = MIDIPresetNames.begin(); iter != MIDIPresetNames.end(); iter++) {
      if (ImGui::Selectable(iter->c_str())) {
        isMIDIDeleteOpen = false;
        isDeleteJSON = true;
        midi_preset_name = *iter;
      }
    }
    ImGui::EndPopup();
  }
  if (!isMIDIDeleteOpen && isDeleteJSON) {
    deleteJSONMIDIPreset(midi_preset_name);
    isDeleteJSON = false;
  }

  // Save MIDI Preset

  if (isMIDIWriteWindow) {
    ImGui::OpenPopup("Save MIDI Preset");
  }
  bool isMIDIWriteOpen = true;
  bool isWriteJSON = false;
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Save MIDI Preset", &isMIDIWriteOpen)) {
    ImGui::InputText("Enter Preset Name", mCurrentMIDIPresetName, 50,
                     ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_CharsNoBlank);
    if (ImGui::IsItemActivated()) {
      captureKeyboardTextInput = true;
    } else if (ImGui::IsItemDeactivated()) {
      captureKeyboardTextInput = false;
    }

    if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

    ImGui::SameLine();

    if (ImGui::Button("Save")) {
      ImGui::CloseCurrentPopup();
      isMIDIWriteOpen = false;
      isWriteJSON = true;
    }

    ImGui::SameLine();

    ImGui::Checkbox("Overwrite", &allowMIDIPresetOverwrite);

    ImGui::EndPopup();
  }
  if (!isMIDIWriteOpen && isWriteJSON) {
    writeJSONMIDIPreset(mCurrentMIDIPresetName, allowMIDIPresetOverwrite);
    isWriteJSON = false;
  }

  // MIDI Learn Help Window
  if (isMIDIHelpWindow) {
    ImGui::OpenPopup("MIDI Learn Help");
  }
  bool isMIDIHelpOpen = true;
  ImGui::SetNextWindowSize(ImVec2(500 * fontScale, 400 * adjustScaleY));
  if (ImGui::BeginPopupModal("MIDI Learn Help", &isMIDIHelpOpen, ImGuiWindowFlags_NoResize)) {
    for (int i = 0; i < MIDIHelpLines.size(); i++) {
      ImGui::SetCursorPosX((250 * fontScale) -
                           (ImGui::CalcTextSize(MIDIHelpLines[i].c_str()).x / 2));
      ImGui::TextUnformatted(MIDIHelpLines[i].c_str());
    }
    ImGui::EndPopup();
  }

  // OSC Config Window
  if (isOSCConfigWindow) {
    ImGui::OpenPopup("OSC Configuration");
  }
  bool isOSCConfigOpen = true;
  ImGui::SetNextWindowSizeConstraints(ImVec2(600 * fontScale, 400 * adjustScaleY),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("OSC Configuration", &isOSCConfigOpen)) {
    if (ImGui::Button("Save")) {
      isOSCWriteWindow = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Load")) {
      isOSCLoadWindow = true;
    }
    ImGui::SameLine();
    if (ImGui::Button("Delete Preset")) {
      isOSCDeleteWindow = true;
    }
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));

    if (InputText("IP Address", &oscAddr, ImGuiInputTextFlags_EnterReturnsTrue, oscAddrCallback,
                  oscAddrCallbackUserData)) {
      resetOSC();
    }
    if (ImGui::IsItemActivated()) {
      captureKeyboardTextInput = true;
    } else if (ImGui::IsItemDeactivated()) {
      captureKeyboardTextInput = false;
    }
    if (ImGui::InputInt("Port", &oscPort, 1, 1, ImGuiInputTextFlags_EnterReturnsTrue)) {
      resetOSC();
    }
    if (ImGui::IsItemActivated()) {
      captureKeyboardTextInput = true;
    } else if (ImGui::IsItemDeactivated()) {
      captureKeyboardTextInput = false;
    }
    if (ImGui::InputFloat("Timeout", &oscTimeout, 0.01, 0.01, "%.2f",
                          ImGuiInputTextFlags_EnterReturnsTrue)) {
      if (oscTimeout < 0.01) {
        oscTimeout = 0.01;
      } else {
        resetOSC();
      }
    }
    if (ImGui::IsItemActivated()) {
      captureKeyboardTextInput = true;
    } else if (ImGui::IsItemDeactivated()) {
      captureKeyboardTextInput = false;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip("How often (in seconds) the server checks for new data on the on the port");
      ImGui::PopStyleVar();
    }
    if (ImGui::Button("Reconnect OSC")) resetOSC();
    ImGui::Dummy(ImVec2(0.0f, 20.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("GRANULATION CONTROLS");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    for (int i = 0; i < consts::NUM_PARAMS; i++) {
      InputText((granulator.ECParameters[i]->getDisplayName()).c_str(),
                &granulator.ECParameters[i]->mOscArgument, ImGuiInputTextFlags_EnterReturnsTrue,
                granulator.ECParameters[i]->inputTextCallback,
                granulator.ECParameters[i]->CallbackUserData);
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip("Enter OSC address to listen to");
        ImGui::PopStyleVar();
      }
      if (!granulator.ECParameters[i]->mOscCustomRange) {
        if (ImGui::Button(
              ("Custom Mapping##osc_" + granulator.ECParameters[i]->getDisplayName()).c_str())) {
          granulator.ECParameters[i]->mOscCustomRange = true;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Click to customize the mapping of OSC values to parameter values");
          ImGui::PopStyleVar();
        }
      } else {
        if (ImGui::Button(
              ("Default Value Mapping##osc_" + granulator.ECParameters[i]->getDisplayName())
                .c_str())) {
          granulator.ECParameters[i]->mOscCustomRange = false;
        }
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.25);
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip(
            "Click to return to a one-to-one mapping of OSC values to parameter values");
          ImGui::PopStyleVar();
        }
        ImGui::InputFloat(
          ("Range Min##osc_" + granulator.ECParameters[i]->getDisplayName()).c_str(),
          &granulator.ECParameters[i]->mOscMin, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Minimum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }

        ImGui::SameLine();
        ImGui::InputFloat(
          ("Range Max##osc_" + granulator.ECParameters[i]->getDisplayName()).c_str(),
          &granulator.ECParameters[i]->mOscMax, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Maximum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }
        ImGui::PopItemWidth();
      }
      ImGui::Dummy(ImVec2(0.0f, 10.0f));
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("MODULATION CONTROLS");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    for (int i = 0; i < consts::NUM_PARAMS; i++) {
      InputText((granulator.ECParameters[i]->getDisplayName() + " Mod").c_str(),
                &granulator.ECModParameters[i]->mOscArgument, ImGuiInputTextFlags_EnterReturnsTrue,
                granulator.ECModParameters[i]->inputTextCallback,
                granulator.ECModParameters[i]->CallbackUserData);
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip("Enter OSC address to listen to");
        ImGui::PopStyleVar();
      }
      if (!granulator.ECModParameters[i]->mOscCustomRange) {
        if (ImGui::Button(
              ("Custom Mapping##osc_" + granulator.ECParameters[i]->getDisplayName() + "_MOD")
                .c_str())) {
          granulator.ECModParameters[i]->mOscCustomRange = true;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Click to customize the mapping of OSC values to parameter values");
          ImGui::PopStyleVar();
        }
      } else {
        if (ImGui::Button(("Default Value Mapping##osc_" +
                           granulator.ECParameters[i]->getDisplayName() + "_MOD")
                            .c_str())) {
          granulator.ECModParameters[i]->mOscCustomRange = false;
        }
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.25);
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip(
            "Click to return to a one-to-one mapping of OSC values to parameter "
            "values");
          ImGui::PopStyleVar();
        }
        ImGui::InputFloat(
          ("Range Min##osc_" + granulator.ECParameters[i]->getDisplayName() + "_MOD").c_str(),
          &granulator.ECModParameters[i]->mOscMin, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Minimum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }
        ImGui::SameLine();
        ImGui::InputFloat(
          ("Range Max##osc_" + granulator.ECParameters[i]->getDisplayName() + "_MOD").c_str(),
          &granulator.ECModParameters[i]->mOscMax, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Maximum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }
        ImGui::PopItemWidth();
      }
      ImGui::Dummy(ImVec2(0.0f, 10.0f));
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Text("LFO CONTROLS");
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    for (int i = 0; i < consts::NUM_LFOS; i++) {
      InputText(("LFO " + toString(i + 1)).c_str(), &granulator.LFOParameters[i]->mOscArgument,
                ImGuiInputTextFlags_EnterReturnsTrue,
                granulator.LFOParameters[i]->inputTextCallback,
                granulator.LFOParameters[i]->CallbackUserData);
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip("Enter OSC address to listen to");
        ImGui::PopStyleVar();
      }

      if (!granulator.LFOParameters[i]->mOscCustomRange) {
        if (ImGui::Button(("Custom Mapping##osc_" + toString("LFO") + toString(i + 1)).c_str())) {
          granulator.LFOParameters[i]->mOscCustomRange = true;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip(
            "Click to customize the mapping of OSC values to parameter "
            "values");
          ImGui::PopStyleVar();
        }
      } else {
        if (ImGui::Button(
              ("Default Value Mapping##osc_" + toString("LFO") + toString(i + 1)).c_str())) {
          granulator.LFOParameters[i]->mOscCustomRange = false;
          granulator.LFOParameters[i]->mOscMin =
            granulator.LFOParameters[i]->frequency->getCurrentMin();
          granulator.LFOParameters[i]->mOscMax =
            granulator.LFOParameters[i]->frequency->getCurrentMax();
        }
        ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.25);
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip(
            "Click to return to a one-to-one mapping of OSC values to "
            "parameter values");
          ImGui::PopStyleVar();
        }
        ImGui::InputFloat(("Range Min##osc_" + toString("LFO") + toString(i + 1)).c_str(),
                          &granulator.LFOParameters[i]->mOscMin, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Minimum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }
        ImGui::SameLine();
        ImGui::InputFloat(("Range Max##osc_" + toString("LFO") + toString(i + 1)).c_str(),
                          &granulator.LFOParameters[i]->mOscMax, 0.1);
        if (ImGui::IsItemActivated()) {
          captureKeyboardTextInput = true;
        } else if (ImGui::IsItemDeactivated()) {
          captureKeyboardTextInput = false;
        }
        if (ImGui::IsItemHovered()) {
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("Maximum expected value to be received via OSC.");
          ImGui::PopStyleVar();
        }
        ImGui::PopItemWidth();
      }
      ImGui::Dummy(ImVec2(0.0f, 10.0f));
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    InputText("Morph Time", &morphTimeOSCArg, ImGuiInputTextFlags_EnterReturnsTrue,
              morphTimeOSCCallback, morphTimeOSCCallbackUserData);
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip("Enter OSC address to listen to");
      ImGui::PopStyleVar();
    }

    if (!morphTimeOscCustomRange) {
      if (ImGui::Button("Custom Mapping##osc_MorphTime")) {
        morphTimeOscCustomRange = true;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip(
          "Click to customize the mapping of OSC values to "
          "parameter values");
        ImGui::PopStyleVar();
      }
    } else {
      if (ImGui::Button("Default Value Mapping##osc_MorphTime")) {
        morphTimeOscCustomRange = false;
        morphTimeOscMin = 0;
        morphTimeOscMax = 50;
      }
      ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() * 0.25);
      ImGui::InputFloat("Range Min##osc_MorphTime", &morphTimeOscMin, 0.1);
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip("Minimum expected value to be received via OSC.");
        ImGui::PopStyleVar();
      }
      ImGui::SameLine();
      ImGui::InputFloat("Range Max##osc_MorphTime", &morphTimeOscMax, 0.1);
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
      }
      if (ImGui::IsItemHovered()) {
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
        ImGui::SetTooltip("Maximum expected value to be received via OSC.");
        ImGui::PopStyleVar();
      }
      ImGui::PopItemWidth();
    }
    ImGui::Dummy(ImVec2(0.0f, 10.0f));

    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, 10.0f));
    InputText("Preset", &presetOSCArg, ImGuiInputTextFlags_EnterReturnsTrue, presetOSCCallback,
              presetOSCCallbackUserData);
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip(
        "Send an integer to this address to load a preset "
        "(e.g. \"/preset 8\" to load preset 8)");
      ImGui::PopStyleVar();
    }
    InputText("Record", &recordOSCArg, ImGuiInputTextFlags_EnterReturnsTrue, recordOSCCallback,
              recordOSCCallbackUserData);
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip(
        "Send a 1 to this address to start recording or a "
        "0 to stop recording");
      ImGui::PopStyleVar();
    }
    InputText("File Name", &fileNameOSCArg, ImGuiInputTextFlags_EnterReturnsTrue,
              fileNameOSCCallback, fileNameOSCCallbackUserData);
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip(
        "Send a string to this address to change the "
        "name for the next recorded sound file");
      ImGui::PopStyleVar();
    }
    InputText("Output Folder", &outputFolderOSCArg, ImGuiInputTextFlags_EnterReturnsTrue,
              outputFolderOSCCallback, outputFolderOSCCallbackUserData);
    if (ImGui::IsItemHovered()) {
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      ImGui::SetTooltip(
        "Send a string to this address to change the "
        "output folder for recorded sound files");
      ImGui::PopStyleVar();
    }

    // OSC Write Window
    if (isOSCWriteWindow) {
      ImGui::OpenPopup("Save OSC Preset");
    }
    bool isOSCWriteOpen = true;
    bool isOSCWriteJSON = false;
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));
    if (ImGui::BeginPopupModal("Save OSC Preset", &isOSCWriteOpen)) {
      ImGui::InputText("Enter Preset Name", mCurrentOSCPresetName, 50,
                       ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_CharsNoBlank);
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
      }

      if (ImGui::Button("Cancel")) ImGui::CloseCurrentPopup();

      ImGui::SameLine();

      if (ImGui::Button("Save")) {
        ImGui::CloseCurrentPopup();
        isOSCWriteOpen = false;
        isOSCWriteJSON = true;
      }

      ImGui::SameLine();

      ImGui::Checkbox("Overwrite", &allowOSCPresetOverwrite);

      ImGui::EndPopup();
    }
    if (!isOSCWriteOpen && isOSCWriteJSON) {
      writeJSONOSCPreset(mCurrentOSCPresetName, allowOSCPresetOverwrite);
      isOSCWriteJSON = false;
    }

    // OSC Load Preset Window
    if (isOSCLoadWindow) {
      ImGui::OpenPopup("Load OSC Preset");
    }
    bool isOSCLoadOpen = true;
    bool isOSCLoadJSON = false;
    std::string osc_preset_name = "";
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));
    if (ImGui::BeginPopupModal("Load OSC Preset", &isOSCLoadOpen)) {
      for (auto iter = OSCPresetNames.begin(); iter != OSCPresetNames.end(); iter++) {
        if (ImGui::Selectable(iter->c_str())) {
          isOSCLoadOpen = false;
          isOSCLoadJSON = true;
          osc_preset_name = *iter;
        }
      }
      ImGui::EndPopup();
    }
    if (!isOSCLoadOpen && isOSCLoadJSON) {
      loadJSONOSCPreset(osc_preset_name);
      isOSCLoadJSON = false;
    }

    // OSC Delete Preset Window
    if (isOSCDeleteWindow) {
      ImGui::OpenPopup("Delete OSC Preset");
    }
    bool isOSCDeleteOpen = true;
    bool isOSCDeleteJSON = false;
    osc_preset_name = "";
    ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 5)),
                                        ImVec2(windowWidth, windowHeight));
    if (ImGui::BeginPopupModal("Delete OSC Preset", &isOSCDeleteOpen)) {
      for (auto iter = OSCPresetNames.begin(); iter != OSCPresetNames.end(); iter++) {
        if (ImGui::Selectable(iter->c_str())) {
          isOSCDeleteOpen = false;
          isOSCDeleteJSON = true;
          osc_preset_name = *iter;
        }
      }
      ImGui::EndPopup();
    }
    if (!isOSCDeleteOpen && isOSCDeleteJSON) {
      deleteJSONOSCPreset(osc_preset_name);
      isOSCDeleteJSON = false;
    }

    if (isOscWarningWindow) {
      ImGui::OpenPopup("OSC Error");
    }
    bool isOSCWarningOpen = true;
    if (ImGui::BeginPopupModal("OSC Error", &isOSCWarningOpen)) {
      isOscWarningWindow = false;
      ImGui::TextColored(ImVec4(*ECred), "Warning");
      ImGui::Text(
        "Could not bind to UDP socket. Is there a "
        "server already bound to that port?");
      ImGui::EndPopup();
    }

    ImGui::EndPopup();
  }

  // PopUp Font scale window
  if (fontScaleWindow) {
    ImGui::OpenPopup("Font Size");
  }
  bool fontScaleOpen = true;
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 3)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Font Size", &fontScaleOpen)) {
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (50 * fontScale));
    ImGui::SliderFloat("Scale", &fontScale, 0.5, 3.0, "%.1f");
    ImGui::PopItemWidth();
    ImGui::EndPopup();
  }
  if (!fontScaleOpen) jsonWriteToConfig(fontScale, consts::FONT_SCALE_KEY);

  // PopUp Audio IO window
  // -------------------------------------------- This
  // enables starting and stopping audio as well as
  // selecting Audio device and its parameters if
  // statement opens Audio IO popup if chosen from
  // menu
  if (displayIO) {
    ImGui::OpenPopup("Audio Settings");
  }
  bool audioOpen = true;
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 6)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Audio Settings", &audioOpen)) {
    drawAudioIO(&audioIO());
    ImGui::EndPopup();
  }

  if (aboutWindow) {
    ImGui::OpenPopup("About");
  }
  bool aboutOpen = true;
  ImGui::SetNextWindowSize(ImVec2(500 * fontScale, 400 * adjustScaleY));
  if (ImGui::BeginPopupModal("About", &aboutOpen, ImGuiWindowFlags_NoResize)) {
    ImGui::PushFont(titleFont);
    ImGui::SetCursorPosX((250 * fontScale) - (ImGui::CalcTextSize("EmissionControl2").x / 2));
    ImGui::Text("EmissionControl2");
    ImGui::PopFont();
    for (int i = 0; i < aboutLines.size(); i++) {
      ImGui::SetCursorPosX((250 * fontScale) - (ImGui::CalcTextSize(aboutLines[i].c_str()).x / 2));
      ImGui::TextUnformatted(aboutLines[i].c_str());
    }
    ImGui::SetCursorPosX((250 * fontScale) - (100 * fontScale));
    if (ImGui::Button("License", ImVec2(200 * fontScale, 20 * adjustScaleY))) {
      system((opener + licenseURL).c_str());
    }
    ImGui::EndPopup();
  }

  // Draw Granulator Controls
  // -----------------------------------------
  if (granulator.getNumberOfAudioFiles() != 0) {
    // Set Dynamic Slider Text
    granulator.ECParameters[consts::SOUND_FILE]->setSliderText(
      granulator.getCurrentAudioFileName());
  } else {
    granulator.ECParameters[consts::SOUND_FILE]->setSliderText("No Sound Files");
  }
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("    GRANULATION CONTROLS", 0, menuBarHeight, windowWidth / 2,
                           firstRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    colPushCount = 0;
    // set alternating slider background shade (green
    // if midi learning)
    if (mCurrentLearningMIDIKey.getType() == consts::M_PARAM &&
        mCurrentLearningMIDIKey.getKeysIndex() == index && mIsLinkingParamAndMIDI) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECgreen);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECgreen);
      colPushCount += 2;
    } else if (mCurrentLearningMIDIKey.getType() == consts::M_PARAM &&
               mCurrentLearningMIDIKey.getKeysIndex() == index && (unlearnFlash % 20) > 10) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECred);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECred);
      colPushCount += 2;
    } else if (index % 3 == 0) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
      colPushCount += 1;
    } else if (index % 3 == 1) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
      colPushCount += 1;
    } else {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
      colPushCount += 1;
    }

    // set slider colors
    if (index < 4) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECblue);
      colPushCount += 1;
    } else if (index > 3 && index < 7) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECgreen);
      colPushCount += 1;
    } else if (index > 6 && index < 11) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECyellow);
      colPushCount += 1;
    } else {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECred);
      colPushCount += 1;
    }
    granulator.ECParameters[index]->drawRangeSlider(&mMIDILearn, &mLastKeyDown);
    if (mMIDILearn.mParamAdd) {
      // This inits. the onMidiMessage loop to listen
      // for midi input. This first MIDI input to come
      // through will be linked.
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_PARAM);
      mIsLinkingParamAndMIDI = true;
    }
    if (mMIDILearn.mParamDel) {
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_PARAM);
      unlinkParamAndMIDI(mCurrentLearningMIDIKey);
      unlearnFlash = 60;
    }
    ImGui::PopStyleColor(colPushCount);
  }
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw modulation window
  // -------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("    MODULATION CONTROLS", windowWidth / 2, menuBarHeight,
                           windowWidth / 2, firstRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    colPushCount = 0;
    // set alternating slider background shade (green
    // if midi learning)
    if (mCurrentLearningMIDIKey.getType() == consts::M_MOD &&
        mCurrentLearningMIDIKey.getKeysIndex() == index && mIsLinkingParamAndMIDI) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECgreen);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECgreen);
      colPushCount += 2;
    } else if (mCurrentLearningMIDIKey.getType() == consts::M_MOD &&
               mCurrentLearningMIDIKey.getKeysIndex() == index && (unlearnFlash % 20) > 10) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECred);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECred);
      colPushCount += 2;
    } else if (index % 3 == 0) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade3);
      colPushCount += 1;
    } else if (index % 3 == 1) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
      colPushCount += 1;
    } else {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade1);
      colPushCount += 1;
    }

    // set slider colors
    if (index < 4) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECblue);
      colPushCount += 1;
    } else if (index > 3 && index < 7) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECgreen);
      colPushCount += 1;
    } else if (index > 6 && index < 11) {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECyellow);
      colPushCount += 1;
    } else {
      ImGui::PushStyleColor(ImGuiCol_SliderGrab, (ImVec4)*ECred);
      colPushCount += 1;
    }
    granulator.ECModParameters[index]->drawModulationControl(&mMIDILearn, &mLastKeyDown);
    if (mMIDILearn.mParamAdd) {
      // This inits. the onMidiMessage loop to listen
      // for midi input. This first MIDI input to come
      // through will be linked.
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_MOD);
      mIsLinkingParamAndMIDI = true;
    }
    if (mMIDILearn.mParamDel) {
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_MOD);
      unlinkParamAndMIDI(mCurrentLearningMIDIKey);
      unlearnFlash = 60;
    }
    ImGui::PopStyleColor(colPushCount);
  }
  ImGui::PopFont();
  float NextWindowYPosition = firstRowHeight + menuBarHeight;

  ParameterGUI::endPanel();

  // Draw preset window
  // -----------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("    PRESETS", 0, NextWindowYPosition, windowWidth / 4, secondRowHeight,
                           flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  int numColumns = 12;
  if (ImGui::GetContentRegionAvail().x / fontScale < 300) numColumns = 6;
  ecInterface::ECdrawPresetHandler(mPresets.get(), numColumns, 3);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw recorder window
  // ---------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("    RECORDER", windowWidth / 4, NextWindowYPosition, windowWidth / 4,
                           secondRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut() <= 1 ? 1 : 2,
                     soundOutput);
  if (ImGui::Button("Change Output Path")) {
    result = NFD_PickFolder(NULL, &outPath);

    if (result == NFD_OKAY) {
      std::string temp = outPath;
      jsonWriteToConfig(temp, consts::SOUND_OUTPUT_PATH_KEY);
      setSoundOutputPath(outPath);
    }
  }
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw LFO parameters window
  // ---------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("    LFO CONTROLS", windowWidth / 2, NextWindowYPosition,
                           windowWidth / 2, secondRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_LFOS; index++) {
    bool no_learn = false;
    // THIS WILL DRAW THE MAIN LFO
    colPushCount = 0;
    if (mCurrentLearningMIDIKey.getType() == consts::M_LFO &&
        mCurrentLearningMIDIKey.getKeysIndex() == index && mIsLinkingParamAndMIDI) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECgreen);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECgreen);
      colPushCount += 2;
    } else if (mCurrentLearningMIDIKey.getType() == consts::M_LFO &&
               mCurrentLearningMIDIKey.getKeysIndex() == index && (unlearnFlash % 20) > 10) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECred);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECred);
      colPushCount += 2;
    }

    // WARNING, hacky as fuck.
    // This is to tell the drawRangeSlider that this
    // is the last parameter to check. This allows
    // mLasyKeyDown.readyToTrig to be set to false.
    if (index == consts::NUM_LFOS - 1) mLastKeyDown.lastParamCheck = true;

    int x_offset = granulator.LFOParameters[index]->drawLFOControl(&mMIDILearn, &mLastKeyDown);
    if (mMIDILearn.mParamAdd) {
      // This inits. the onMidiMessage loop to listen
      // for midi input. This first MIDI input to come
      // through will be linked.
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_LFO);
      mIsLinkingParamAndMIDI = true;
      no_learn = true;
    }
    if (mMIDILearn.mParamDel) {
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_LFO);
      unlinkParamAndMIDI(mCurrentLearningMIDIKey);
      unlearnFlash = 60;
      no_learn = true;
    }
    ImGui::PopStyleColor(colPushCount);

    // THIS WILL DRAW THE DUTY CYCLE
    colPushCount = 0;
    if (mCurrentLearningMIDIKey.getType() == consts::M_DUTY &&
        mCurrentLearningMIDIKey.getKeysIndex() == index && mIsLinkingParamAndMIDI) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECgreen);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECgreen);
      colPushCount += 2;
    } else if (mCurrentLearningMIDIKey.getType() == consts::M_DUTY &&
               mCurrentLearningMIDIKey.getKeysIndex() == index && (unlearnFlash % 20) > 10) {
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECred);
      ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECred);
      colPushCount += 2;
    }

    // WARNING, hacky as fuck.
    // This is to tell the drawRangeSlider that this
    // is the last parameter to check. This allows
    // mLasyKeyDown.readyToTrig to be set to false.
    if (index == consts::NUM_LFOS - 1) mLastKeyDown.lastParamCheck = true;

    // Will only draw this if a square.
    granulator.LFOParameters[index]->drawLFODuty(&mMIDILearn, &mLastKeyDown, x_offset);
    if (mMIDILearn.mParamAdd && !no_learn) {
      // This inits. the onMidiMessage loop to listen
      // for midi input. This first MIDI input to come
      // through will be linked.
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_DUTY);
      mIsLinkingParamAndMIDI = true;
    }
    if (mMIDILearn.mParamDel && !no_learn) {
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_DUTY);
      unlinkParamAndMIDI(mCurrentLearningMIDIKey);
      unlearnFlash = 60;
    }
    ImGui::PopStyleColor(colPushCount);
  }

  ImGui::PopFont();
  ParameterGUI::endPanel();

  NextWindowYPosition += secondRowHeight;
  float graphHeight = (windowHeight - NextWindowYPosition);
  if (graphHeight >= 100) {
    float graphPosY = (sliderheight + 9) * 2;
    if (graphHeight <= 200) {
      graphFlags = flags + ImGuiWindowFlags_NoTitleBar;
      graphPosY = sliderheight + 12;
    } else {
      graphFlags = flags;
    }
    // Draw Scan Display
    // ------------------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("    SCAN DISPLAY", 0, NextWindowYPosition, windowWidth,
                             graphHeight / 3, graphFlags);
    ImGui::PopFont();
    if (granulator.getNumberOfAudioFiles() != 0) {
      ImGui::PushFont(bodyFont);
      float plotWidth = ImGui::GetContentRegionAvail().x;
      float plotHeight = ImGui::GetContentRegionAvail().y;
      ImVec2 p = ImGui::GetCursorScreenPos();

      int soundFileFrames = granulator.soundClip[granulator.mModClip]->frames;

      float scanHead = granulator.ECParameters[consts::SCAN_BEGIN]->getModParam(
        granulator.ECModParameters[consts::SCAN_BEGIN]->getWidthParam());
      float scanWidth = granulator.ECParameters[consts::SCAN_RANGE]->getModParam(
        granulator.ECModParameters[consts::SCAN_RANGE]->getWidthParam());
      float scanPos = granulator.getCurrentIndex() / soundFileFrames;
      if (scanPos < 0) scanPos += 1;
      if (scanPos > 1) scanPos -= 1;

      ImU32 semitransBlue =
        IM_COL32(ECblue->Value.x * 255, ECblue->Value.y * 255, ECblue->Value.z * 255, 100);

      // Draw Waveform
      float displayPosX = ImGui::GetCursorPosX();

      ImGui::PlotHistogram("##scanDisplayPos", audioThumbnails[granulator.mModClip]->data() + 1,
                           audioThumbnails[granulator.mModClip]->size() / 2, 0, nullptr, -1, 1,
                           ImVec2(plotWidth, plotHeight), sizeof(float) * 2);
      ImGui::SameLine();
      ImGui::SetCursorPosX(displayPosX);
      ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
      ImGui::PlotHistogram("##scanDisplayNeg", audioThumbnails[granulator.mModClip]->data(),
                           audioThumbnails[granulator.mModClip]->size() / 2, 0, nullptr, -1, 1,
                           ImVec2(plotWidth, plotHeight), sizeof(float) * 2);
      ImGui::PopStyleColor();
      ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
      if (ImGui::IsItemHovered())
        ImGui::SetTooltip("Scan Head: %i\nScan Range: %i", int(scanHead * 100),
                          int(scanWidth * 100));
      ImGui::PopStyleVar();

      ImDrawList *drawList = ImGui::GetWindowDrawList();

      // Draw Scan Width
      if (scanHead + scanWidth > 1.0f) {
        drawList->AddRectFilled(ImVec2(p.x + (scanHead * plotWidth), p.y),
                                ImVec2(p.x + plotWidth, p.y + plotHeight), semitransBlue);
        drawList->AddRectFilled(
          ImVec2(p.x, p.y),
          ImVec2(p.x + ((scanHead + scanWidth - 1.0f) * plotWidth), p.y + plotHeight),
          semitransBlue);
      } else if (scanHead + scanWidth < 0.0f) {
        drawList->AddRectFilled(ImVec2(p.x + (scanHead * plotWidth), p.y),
                                ImVec2(p.x, p.y + plotHeight), semitransBlue);
        drawList->AddRectFilled(
          ImVec2(p.x + plotWidth, p.y),
          ImVec2(p.x + ((scanHead + scanWidth + 1.0f) * plotWidth), p.y + plotHeight),
          semitransBlue);
      } else {
        drawList->AddRectFilled(
          ImVec2(p.x + (scanHead * plotWidth), p.y),
          ImVec2(p.x + ((scanHead + scanWidth) * plotWidth), p.y + plotHeight), semitransBlue);
      }

      // Draw Scan Head
      drawList->AddLine(ImVec2(p.x + (scanHead * plotWidth), p.y),
                        ImVec2(p.x + (scanHead * plotWidth), p.y + plotHeight), *ECblue, 6.0f);

      // Draw Scanner position
      drawList->AddLine(ImVec2(p.x + (scanPos * plotWidth), p.y),
                        ImVec2(p.x + (scanPos * plotWidth), p.y + plotHeight), *Text, 2.0f);

      // Draw Individual Grains
      granulator.copyActiveGrainIndicies(GrainDisplayIndices, &numGrainsToDisplay,
                                         consts::MAX_GRAIN_DISPLAY);

      for (int grain = 0; grain < numGrainsToDisplay; grain++) {
        float temp_line_val = GrainDisplayIndices[grain] / soundFileFrames;
        drawList->AddLine(ImVec2(p.x + (temp_line_val * plotWidth), p.y),
                          ImVec2(p.x + (temp_line_val * plotWidth), p.y + plotHeight), *ECred,
                          2.0f);
      }
      ImGui::PopFont();
    }
    NextWindowYPosition += graphHeight / 3;

    ParameterGUI::endPanel();

    // Draw grain histogram window
    // --------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("    ACTIVE GRAINS", 0, NextWindowYPosition, windowWidth / 4,
                             graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    ImGui::Text("Counter: %.1i ", granulator.getNumActiveVoices());

    if (grainAccum < granulator.getNumActiveVoices()) grainAccum = granulator.getNumActiveVoices();
    if (framecounter % 2 == 0) {
      streamHistory.erase(streamHistory.begin());
      streamHistory.push_back(grainAccum);
      highestStreamCount = *max_element(streamHistory.begin(), streamHistory.end());
      if (highestStreamCount < 2) highestStreamCount = 2;
      grainAccum = 0;
    }
    if (framecounter % 60 == 0) {
      grainsPerSecond = granulator.grainCounter;
      granulator.grainCounter = 0;
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SetCursorPosY(graphPosY);
    ImGui::PlotHistogram("##Active Streams", &streamHistory[0], streamHistory.size(), 0, nullptr, 0,
                         highestStreamCount, ImGui::GetContentRegionAvail(), sizeof(int));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Grains Per Second: %i", grainsPerSecond);
    ImGui::PopStyleVar();
    ImGui::PopFont();
    ParameterGUI::endPanel();

    // Draw Oscilloscope window
    // -----------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("    OSCILLOSCOPE", windowWidth / 4, NextWindowYPosition,
                             windowWidth * 11 / 16, graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
    ImGui::Text("Time Frame (s):");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (100 * fontScale));
    ImGui::SliderFloat("##Scope frame", &oscilloscopeFrame, 0.001, 3.0, "%.3f");

    // Draw left channel oscilloscope
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SetCursorPosY(graphPosY);
    ImGui::PushStyleColor(ImGuiCol_PlotLines, light ? (ImVec4)*ECblue : (ImVec4)*ECblue);
    int offset =
      granulator.oscBufferL.getTail() - (oscilloscopeFrame * granulator.getGlobalSamplingRate());
    if (offset < 0) offset += granulator.oscBufferL.getMaxSize();
    util::Plot_RingBufferGetterData data_l(granulator.oscBufferL.data(), sizeof(float), offset,
                                           granulator.oscBufferL.getMaxSize());
    ImGui::PlotLines("##ScopeL", &util::Plot_RingBufferGetter, (void *)&data_l,
                     int(oscilloscopeFrame * granulator.getGlobalSamplingRate()), 0.0, nullptr, -1,
                     1, ImVec2(0, ImGui::GetContentRegionAvail().y));
    // Draw a black line across the center of the
    // scope
    ImGui::SetCursorPosY(graphPosY);
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(0, 0, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
    ImGui::PlotLines("##black_line", &blackLine[0], 2, 0, nullptr, -1, 1,
                     ImVec2(0, ImGui::GetContentRegionAvail().y),
                     sizeof(float));  // before opt
    // Draw right channel oscilloscope
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)*ECred);
    ImGui::SetCursorPosY(graphPosY + 1);
    util::Plot_RingBufferGetterData data_r(granulator.oscBufferR.data(), sizeof(float), offset,
                                           granulator.oscBufferR.getMaxSize());
    ImGui::PlotLines("##ScopeR", &util::Plot_RingBufferGetter, (void *)&data_r,
                     int(oscilloscopeFrame * granulator.getGlobalSamplingRate()), 0.0, nullptr, -1,
                     1, ImVec2(0, ImGui::GetContentRegionAvail().y));
    // Draw a black line across the center of the
    // scope
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(0, 0, 0, 255));
    ImGui::SetCursorPosY(graphPosY + 1);
    ImGui::PlotLines("##black_line", &blackLine[0], 2, 0, nullptr, -1, 1,
                     ImVec2(0, ImGui::GetContentRegionAvail().y), sizeof(float));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("Oscilloscope");
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar();

    ImGui::PopFont();
    ParameterGUI::endPanel();

    // Draw VU Meter window
    // ---------------------------------------------
    ImGui::PushFont(titleFont);
    if (width() < 1250) ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 12));
    ParameterGUI::beginPanel("    VU", windowWidth * 15 / 16, NextWindowYPosition,
                             windowWidth * 1 / 16, graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    // Size of VU meter data arrays in samples

    float VUleft =
      -20 * log10(granulator.vuBufferL.getRMS(granulator.getGlobalSamplingRate() / 20));
    float VUright =
      -20 * log10(granulator.vuBufferR.getRMS(granulator.getGlobalSamplingRate() / 20));

    // Set meter colors to green
    ImVec4 VUleftCol = (ImVec4)*ECgreen;
    ImVec4 VUrightCol = (ImVec4)*ECgreen;
    // Set meter colors to red if clipped
    if (granulator.clipL > 0) {
      VUleftCol = (ImVec4)*ECred;
      granulator.clipL--;
    }
    if (granulator.clipR > 0) {
      VUrightCol = (ImVec4)*ECred;
      granulator.clipR--;
    }

    // Draw VU Meters
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, VUleftCol);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, VUleftCol);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ((ImVec4)ImColor(0, 0, 0, 180)));
    ImGui::PlotHistogram(
      "##VUleft", &VUleft, 1, 0, nullptr, 96, 0,
      ImVec2((ImGui::GetContentRegionAvail().x / 2) - 4, ImGui::GetContentRegionAvail().y),
      sizeof(float));
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("L Peak: %f", 20 * log10(granulator.peakL));
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, VUrightCol);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, VUrightCol);
    ImGui::PlotHistogram("##VUright", &VUright, 1, 0, nullptr, 96, 0,
                         ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                         sizeof(float));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered()) ImGui::SetTooltip("R Peak: %f", 20 * log10(granulator.peakR));
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(5);
    ImGui::PopFont();
    ParameterGUI::endPanel();
  }
  if (width() < 1250) ImGui::PopStyleVar();
  ImGui::PopStyleColor(18);
  ImGui::PopStyleVar(3);
  al::imguiEndFrame();
  al::imguiDraw();
  firstFrame = false;
}

bool ecInterface::onKeyDown(Keyboard const &k) {
  if (!captureKeyboardTextInput) {
    mLastKeyDown.key = k;
    mLastKeyDown.readyToTrig = true;
    mLastKeyDown.lastParamCheck = false;
  }
  return true;
}

void ecInterface::initMIDI() {
  /* This is the single stupidest code I have ever
   written.

   RtMIDI seems to access trash memory, instead of
   mBindings[index] for the first
      *consts::MAX_MIDI_IN* setCallback functions.

   This is only seen when we have multiple midi input
   ports allowed.

   To get around this you simply execute
   consts::MAX_MIDI_IN dummy bindTo calls and then
   immediately clear mBinding of this fake data.

   This seems to properly init the pointers to
   mBindings memory when setCallback uses them.

   What the fuck. I hate this, I hate computers. I'm
   goingg outside.
  */
  for (int i = 0; i < consts::MAX_MIDI_IN; i++) MIDIMessageHandler::bindTo(midiIn[i], 0);
  MIDIMessageHandler::clearBindings();
  /* End of stupid as shit code. **/

  if (midiIn[0].getPortCount() > 0) {
    // Bind ourself to the RtMidiIn[0] object, to have
    // the onMidiMessage() callback called whenever a
    // MIDI message is received Open the last device
    // found
    unsigned int port = 0;
    MIDIMessageHandler::bindTo(midiIn[0], port);
    midiIn[0].openPort(port);
    SelectedMIDIDevices.resize(1);
    SelectedMIDIDevices[port] = true;
    printf("Opened port to %s\n", midiIn[0].getPortName(port).c_str());
  } else {
    printf("Error: No MIDI devices found.\n");
  }
}
bool first = true;
void ecInterface::onMIDIMessage(const MIDIMessage &m) {
  switch (m.type()) {
    // Control messages need to be parsed again...
    case MIDIByte::CONTROL_CHANGE:
      if (mIsLinkingParamAndMIDI) {
        bool isKeyPresent = false;
        for (int index = 0; index < ActiveMIDI.size(); index++) {
          if (ActiveMIDI[index].getKeysIndex() == mCurrentLearningMIDIKey.getKeysIndex() &&
              ActiveMIDI[index].getType() == mCurrentLearningMIDIKey.getType()) {
            isKeyPresent = true;
            ActiveMIDI[index].mInfo.push_back(m);
          }
        }
        if (!isKeyPresent) {
          mCurrentLearningMIDIKey.mInfo = std::vector<al::MIDIMessage>(1, m);
          ActiveMIDI.push_back(mCurrentLearningMIDIKey);
        }
        mIsLinkingParamAndMIDI = false;
      }

      updateActiveMIDIParams(m);
      break;
    default:;
  }
}

void ecInterface::updateActiveMIDIParams(const MIDIMessage &m) {
  for (int index = 0; index < ActiveMIDI.size(); index++) {
    for (int jndex = 0; jndex < ActiveMIDI[index].mInfo.size(); jndex++) {
      if (ActiveMIDI[index].mInfo[jndex].channel() == m.channel() &&
          ActiveMIDI[index].mInfo[jndex].controlNumber() == m.controlNumber()) {
        switch (ActiveMIDI[index].getType()) {
          case M_PARAM:
            updateECParamMIDI(m.controlValue(), ActiveMIDI[index].getKeysIndex());
            break;
          case M_MOD:
            updateECModParamMIDI(m.controlValue(), ActiveMIDI[index].getKeysIndex());
            break;
          case M_LFO:
            updateLFOParamMIDI(m.controlValue(), ActiveMIDI[index].getKeysIndex());
            break;
          case M_DUTY:
            updateLFODutyParamMIDI(m.controlValue(), ActiveMIDI[index].getKeysIndex());
            break;
          case M_MORPH:
            updatePresetMorphParamMIDI(m.controlValue());
            break;
          default:
            updateECParamMIDI(m.controlValue(), ActiveMIDI[index].getKeysIndex());
        }
      }
    }
  }
}

void ecInterface::unlinkParamAndMIDI(MIDIKey &paramKey) {
  int index;
  bool found = false;
  for (index = 0; index < ActiveMIDI.size(); index++) {
    if (ActiveMIDI[index].getKeysIndex() == paramKey.getKeysIndex() &&
        ActiveMIDI[index].getType() == paramKey.getType()) {
      found = true;
      break;
    }
  }
  if (found) ActiveMIDI.erase(ActiveMIDI.begin() + index);
}

void ecInterface::drawAudioIO(AudioIO *io) {
  struct AudioIOState {
    int currentSr = 1;
    int currentBufSize = 3;
    int currentDevice = 0;
    int currentOut = 1;
    int currentMaxOut;
    std::vector<std::string> devices;
  };
  auto updateOutDevices = [&](AudioIOState &state) {
    state.devices.clear();
    int numDevices = AudioDevice::numDevices();
    int dev_out_index = 0;
    for (int i = 0; i < numDevices; i++) {
      if (!AudioDevice(i).hasOutput()) continue;

      state.devices.push_back(AudioDevice(i).name());
      if (currentAudioDevice == AudioDevice(i).name()) {
        state.currentDevice = dev_out_index;
        state.currentOut = granulator.getLeadChannel() + 1;
        state.currentMaxOut = AudioDevice(i).channelsOutMax();
      }
      dev_out_index++;
    }
  };
  static std::map<AudioIO *, AudioIOState> stateMap;
  if (stateMap.find(io) == stateMap.end()) {
    stateMap[io] = AudioIOState();
    updateOutDevices(stateMap[io]);
  }
  AudioIOState &state = stateMap[io];
  ImGui::PushID(std::to_string((unsigned long)io).c_str());

  if (io->isOpen()) {
    std::string text;
    text += "Device: " + state.devices.at(state.currentDevice);
    text += "\nSampling Rate: " + std::to_string(int(io->fps()));
    text += "\nBuffer Size: " + std::to_string(io->framesPerBuffer());
    text += "\nOutput Channels: " + std::to_string(state.currentOut) + ", " +
            std::to_string(state.currentOut + 1);
    ImGui::Text("%s", text.c_str());
    if (ImGui::Button("Stop")) {
      isPaused = true;
      io->close();
      state.currentSr = getSampleRateIndex();
      state.currentBufSize = getBufSizeIndex();
    }
  } else {
    if (ImGui::Button("Update Devices")) {
      updateOutDevices(state);
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (100 * fontScale));
    if (ImGui::Combo("Device", &state.currentDevice, ParameterGUI::vector_getter,
                     static_cast<void *>(&state.devices), state.devices.size())) {
      state.currentMaxOut =
        AudioDevice(state.devices.at(state.currentDevice), AudioDevice::OUTPUT).channelsOutMax();
      io->device(AudioDevice(state.devices.at(state.currentDevice), AudioDevice::OUTPUT));
      currentAudioDevice = state.devices.at(state.currentDevice);
    }
    std::string chan_label = "Select Outs: (Up to " + std::to_string(state.currentMaxOut) + " )";
    ImGui::Text(chan_label.c_str(), "%s");
    // ImGui::SameLine();
    // ImGui::Checkbox("Mono/Stereo", &isStereo);
    ImGui::Indent(25 * fontScale);
    ImGui::PushItemWidth(50 * fontScale);
    if (ImGui::DragInt("Chan 1", &state.currentOut, 1.0f, 0, state.currentMaxOut - 1, "%d",
                       1 << 4)) {
      if (state.currentOut > state.currentMaxOut - 1) state.currentOut = state.currentMaxOut - 1;
      if (state.currentOut < 1) state.currentOut = 1;
    }
    ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.5f);
    for (int i = 1; i < MAX_AUDIO_OUTS; i++) {
      ImGui::SameLine();
      int temp = state.currentOut + i;
      std::string channel = "Chan " + std::to_string(i + 1);
      ImGui::DragInt(channel.c_str(), &temp, 1.0f, 0, state.currentMaxOut, "%d", 1 << 4);
    }
    ImGui::PopStyleVar();
    ImGui::PopItemFlag();

    ImGui::Unindent(25 * fontScale);
    ImGui::PopItemWidth();

    std::vector<std::string> samplingRates{"44100", "48000", "88200", "96000"};
    if (ImGui::Combo("Sampling Rate", &state.currentSr, ParameterGUI::vector_getter,
                     static_cast<void *>(&samplingRates), samplingRates.size())) {
      granulator.setGlobalSamplingRate(std::stof(samplingRates[state.currentSr]));
      io->framesPerSecond(granulator.getGlobalSamplingRate());
    }
    std::vector<std::string> bufferSizes{"128", "256", "512", "1024", "2048"};
    if (ImGui::Combo("Buffer Size", &state.currentBufSize, ParameterGUI::vector_getter,
                     static_cast<void *>(&bufferSizes), bufferSizes.size())) {
      granulator.setGlobalBufferSize(std::stof(bufferSizes[state.currentBufSize]));
      io->framesPerBuffer(granulator.getGlobalBufferSize());
    }
    ImGui::PopItemWidth();
    if (ImGui::Button("Start")) {
      granulator.setOutChannels(state.currentOut - 1, state.currentMaxOut);
      io->channelsIn(0);
      granulator.setIO(io);
      if (saveDefaultAudio) {
        jsonWriteToConfig(granulator.getGlobalSamplingRate(), consts::SAMPLE_RATE_KEY);
        jsonWriteToConfig(granulator.getGlobalBufferSize(), consts::BUFFER_SIZE_KEY);
        jsonWriteToConfig(currentAudioDevice, consts::DEFAULT_AUDIO_DEVICE_KEY);
        jsonWriteToConfig(state.currentOut - 1, consts::LEAD_CHANNEL_KEY);
      }

      granulator.resampleSoundFiles();

      if (io->start()) {
        isPaused = false;
      }
    }
    ImGui::SameLine();
    ImGui::Checkbox("Set as Default", &saveDefaultAudio);
  }
  ImGui::PopID();
}

void ecInterface::drawRecorderWidget(al::OutputRecorder *recorder, double frameRate,
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
  ImGui::Text("Output File Name:");

  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 10.0f);
  InputText("##Record Name", &buf1, ImGuiInputTextFlags_EnterReturnsTrue, buf1Callback,
            buf1CallbackUserData);
  if (ImGui::IsItemActivated()) {
    captureKeyboardTextInput = true;
  } else if (ImGui::IsItemDeactivated()) {
    captureKeyboardTextInput = false;
  }
  ImGui::PopItemWidth();

  if (state.recordButton) {
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)*ECred);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0.5, 0.5, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
  }
  std::string buttonText = state.recordButton ? "Stop" : "Record";
  bool recordButtonClicked = ImGui::Button(buttonText.c_str());
  if (state.recordButton) {
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
  }
  if (recordButtonClicked) {
    state.recordButton = !state.recordButton;
    if (state.recordButton) {
      // checking that file name is valid
      if (buf1.empty()) {
        buf1.append("EC2_Output.wav");
      } else if (buf1.size() < 5) {
        buf1.append(".wav");
      } else if (buf1.substr(buf1.size() - 4, 4) != ".wav") {
        buf1.append(".wav");
      }
      uint32_t ringBufferSize;
      if (bufferSize == 0) {
        ringBufferSize = 8192;
      } else {
        ringBufferSize = bufferSize * numChannels * 4;
      }
      recordFilename = buf1;
      if (!state.overwriteButton) {
        int counter = 1;
        while (File::exists(directory + recordFilename) && counter < 9999) {
          recordFilename = buf1;
          int lastDot = recordFilename.find_last_of(".");
          recordFilename = recordFilename.substr(0, lastDot) + "_" + std::to_string(counter++) +
                           recordFilename.substr(lastDot);
        }
      }
      if (!recorder->start(directory + recordFilename, frameRate, numChannels, ringBufferSize,
                           gam::SoundFile::WAV, gam::SoundFile::FLOAT)) {
        std::cerr << "Error opening file for record" << std::endl;
      }
    } else {
      recorder->close();
    }
  }
  ImGui::SameLine();
  ImGui::Checkbox("Overwrite", &state.overwriteButton);
  ImGui::Text("Writing to:");
  ImGui::TextWrapped("%s", directory.c_str());

  ImGui::PopID();
}

void ecInterface::setGUIParams() {
  ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)*PrimaryColor);
  ImGui::PushStyleColor(ImGuiCol_PopupBg, (ImVec4)*PrimaryColor);
  ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*Shade2);
  ImGui::PushStyleColor(ImGuiCol_FrameBgHovered,
                        (ImVec4)ImColor(ECblue->Value.x, ECblue->Value.y, ECblue->Value.z, 0.3));
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
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram, light ? (ImVec4)ImColor(0, 0, 0, 150)
                                                      : (ImVec4)ImColor(255, 255, 255, 150));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, (ImVec4)*ECgreen);
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)*Text);
  ImGui::PushStyleColor(ImGuiCol_CheckMark, light ? (ImVec4)ImColor(0, 0, 0, 150)
                                                  : (ImVec4)ImColor(255, 255, 255, 150));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 12));
}

int ecInterface::getSampleRateIndex() {
  unsigned s_r = (int)granulator.getGlobalSamplingRate();
  switch (s_r) {
    case 44100:
      return 0;
    case 48000:
      return 1;
    case 88200:
      return 2;
    case 96000:
      return 3;
    default:
      return 0;
  }
}

int ecInterface::getBufSizeIndex() {
  unsigned bufSize = (int)granulator.getGlobalBufferSize();
  switch (bufSize) {
    case 128:
      return 0;
    case 256:
      return 1;
    case 512:
      return 2;
    case 1024:
      return 3;
    case 2048:
      return 4;
    default:
      return 0;
  }
}

/**** Borrowed and modified from
 * al_ParameterGUI.cpp****/
ecInterface::PresetHandlerState &ecInterface::ECdrawPresetHandler(PresetHandler *presetHandler,
                                                                  int presetColumns,
                                                                  int presetRows) {
  static std::map<PresetHandler *, ecInterface::PresetHandlerState> stateMap;
  if (stateMap.find(presetHandler) == stateMap.end()) {
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

  std::string id = std::to_string((unsigned long)presetHandler);
  std::string suffix = "##PresetHandler" + id;
  ImGui::PushID(suffix.c_str());

  int selection = presetHandler->getCurrentPresetIndex();
  std::string currentPresetName = presetHandler->getCurrentPresetName();
  // JACK KILGORE CHANGE, remove map from display
  int delim_index = 0;
  if (currentPresetName.length() == 0)
    currentPresetName = "none";
  else
    delim_index = (state.currentBank + "-").size();

  if (currentPresetName.size() > delim_index &&
      currentPresetName.substr(0, delim_index) == state.currentBank + "-")
    ImGui::Text("Current Preset: %s", currentPresetName.substr(delim_index).c_str());
  else
    ImGui::Text("Current Preset: %s", currentPresetName.c_str());
  int counter = state.presetHandlerBank * (presetColumns * presetRows);
  if (state.storeButtonState) {
    ImGui::PushStyleColor(ImGuiCol_Text, light ? (ImVec4)*ECblue : (ImVec4)*ECgreen);
  }
  float presetWidth = (ImGui::GetContentRegionAvail().x / presetColumns) - 8.0f;
  for (int row = 0; row < presetRows; row++) {
    for (int column = 0; column < presetColumns; column++) {
      if (counter < 180) {
        std::string name = std::to_string(counter);
        ImGui::PushID(counter);

        bool nothingStored = currentPresetMap.find(counter) == currentPresetMap.end();
        if (nothingStored) ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3);

        bool is_selected = selection == counter;
        if (is_selected) {
          ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
        }

        const bool selectableSelected = ImGui::Selectable(
          name.c_str(), is_selected, 0, ImVec2(presetWidth, ImGui::GetFontSize() * 1.2f));
        if (ImGui::IsItemHovered() && !nothingStored)  // tooltip showing preset
                                                       // name
        {
          const std::string currentlyhoveringPresetName = presetHandler->getPresetName(counter);
          ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
          ImGui::SetTooltip("%s", currentlyhoveringPresetName.c_str());
          ImGui::PopStyleVar();
        }

        if (selectableSelected) {
          if (state.storeButtonState) {
            std::string saveName = state.enteredText;
            if (saveName.size() == 0) {
              saveName = name;
            }
            // JACK KILGORE CHANGE --
            // OLD : resetHandler->storePreset(counter,
            // saveName.c_str()); NEW : Adds preset map
            // name to stored preset to allow presets w/
            // the same name over
            //       different maps.
            presetHandler->storePreset(counter, (state.currentBank + "-" + saveName).c_str());
            selection = counter;
            state.storeButtonState = false;
            ImGui::PopStyleColor();
            state.enteredText.clear();
            currentPresetMap = presetHandler->readPresetMap(state.currentBank);
          } else {
            if (presetHandler->recallPreset(counter) != "") {  // Preset is available
              selection = counter;
            }
          }
        }
        if (nothingStored) ImGui::PopStyleVar();
        if (is_selected) ImGui::PopStyleColor(1);

        if (column < presetColumns - 1) ImGui::SameLine();
        counter++;
        ImGui::PopID();
      }
    }
  }
  if (state.storeButtonState) {
    ImGui::PopStyleColor();
  }
  if (ImGui::Button("<-")) {
    state.presetHandlerBank -= 1;
    if (state.presetHandlerBank < 0) {
      state.presetHandlerBank = 179 / (presetColumns * presetRows);
    }
  }
  ImGui::SameLine(0.0f, 2.0f);
  if (ImGui::Button("->")) {
    state.presetHandlerBank += 1;
    if (state.presetHandlerBank > 179 / (presetColumns * presetRows)) {
      state.presetHandlerBank = 0;
    }
  }
  ImGui::SameLine(0.0f, 5.0f);

  if (state.storeButtonState) {
    ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)*ECgreen);
  }
  std::string storeText = state.storeButtonState ? "Cancel" : "Store";
  bool storeButtonPressed = ImGui::Button(storeText.c_str(), ImVec2(0, 0));
  if (state.storeButtonState) {
    ImGui::PopStyleColor();
  }
  if (storeButtonPressed) {
    state.storeButtonState = !state.storeButtonState;
  }
  ImGui::SameLine();
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (70 * fontScale));

  // BEGIN MIDI LOGIC
  colPushCount = 0;
  if (mIsLinkingParamAndMIDI && mCurrentLearningMIDIKey.getType() == consts::M_MORPH) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECgreen);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECgreen);
    colPushCount = 2;
  } else if (mCurrentLearningMIDIKey.getType() == consts::M_MORPH && (unlearnFlash % 20) > 10) {
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)*ECred);
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, (ImVec4)*ECred);
    colPushCount += 2;
  }

  // Draw Morph Time Control
  float morphTime = presetHandler->getMorphTime();
  char morphTime_str[64];
  // Shown format to the user.
  sprintf(morphTime_str, "%.2f s", morphTime);

  // Offset by 1.0 because the log scale feels nicer
  // this way.
  morphTime = morphTime + 1.;
  bool is_text_input = false;
  if (ImGui::SliderFloat("Morph Time", &morphTime, 1.0f, consts::MAX_MORPH_TIME + 1., morphTime_str,
                         ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat)) {
    // Map back to the true value.
    morphTime = morphTime - 1.;
    presetHandler->setMorphTime(morphTime);
    is_text_input = (ImGui::IsItemActive() && ImGui::TempInputIsActive(ImGui::GetActiveID()));
  }
  // If user used the text input, don't remap, just
  // use the users inputted val. Omg this is hacky
  // what are you doing jack.
  if (ImGui::IsItemDeactivatedAfterEdit()) {
    if (is_text_input) presetHandler->setMorphTime(morphTime);
  }

  ImGui::PopStyleColor(colPushCount);

  // Press m while hovering over a parameter to start
  // midi learn.
  if (ImGui::IsItemHovered() &&
      mLastKeyDown.key.key() == static_cast<int>(consts::KEYBOARD_MIDI_LEARN) &&
      !mLastKeyDown.key.shift() && mLastKeyDown.readyToTrig) {
    mMIDILearn.mParamAdd = true;
  }
  if (ImGui::IsItemHovered() &&
      mLastKeyDown.key.key() == static_cast<int>(consts::KEYBOARD_MIDI_LEARN) &&
      mLastKeyDown.key.shift() && mLastKeyDown.readyToTrig) {
    mMIDILearn.mParamDel = true;
  }

  if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1)) {
    ImGui::OpenPopup("rightClickMorph");
  }
  if (ImGui::BeginPopup("rightClickMorph")) {
    if (ImGui::Selectable("MIDI Learn")) {
      mMIDILearn.mParamAdd = true;
    }
    if (ImGui::Selectable("MIDI Unlearn")) {
      mMIDILearn.mParamDel = true;
    }
    ImGui::Separator();
    ImGui::EndPopup();
  }
  if (mMIDILearn.mParamAdd) {
    mCurrentLearningMIDIKey.setKeysIndex(0, consts::M_MORPH);
    mIsLinkingParamAndMIDI = true;
  }
  if (mMIDILearn.mParamDel) {
    mCurrentLearningMIDIKey.setKeysIndex(0, consts::M_MORPH);
    unlinkParamAndMIDI(mCurrentLearningMIDIKey);
    unlearnFlash = 60;
  }
  // END MIDI LOGIC

  ImGui::PopItemWidth();
  if (state.storeButtonState) {
    char buf2[64];
    strncpy(buf2, state.enteredText.c_str(), 63);
    ImGui::Text("Store preset as:");
    ImGui::SameLine();
    if (ImGui::InputText("preset", buf2, 64)) {
      state.enteredText = buf2;
    }
    if (ImGui::IsItemActivated()) {
      captureKeyboardTextInput = true;
    } else if (ImGui::IsItemDeactivated()) {
      captureKeyboardTextInput = false;
    }
    ImGui::Text("Click on a preset number to store.");
  } else {
    // std::vector<std::string> mapList =
    // presetHandler->availablePresetMaps(); // before
    // opt
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
    if (ImGui::BeginCombo("Preset Map", state.currentBank.data())) {
      // stateMap[presetHandler].mapList =
      // presetHandler->availablePresetMaps();
      // //before opt
      for (auto mapName : stateMap[presetHandler].mapList) {
        bool isSelected = (state.currentBank == mapName);
        if (ImGui::Selectable(mapName.data(), isSelected)) {
          state.currentBank = mapName;
          presetHandler->setCurrentPresetMap(mapName);
          currentPresetMap = presetHandler->readPresetMap(mapName);
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
      if (ImGui::IsItemActivated()) {
        captureKeyboardTextInput = true;
      } else if (ImGui::IsItemDeactivated()) {
        captureKeyboardTextInput = false;
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
        stateMap[presetHandler].mapList = presetHandler->availablePresetMaps();  // optimize
        // CHANGE : when new map is made,
        // automatically switch to it.
        state.currentBank = state.newMapText;
        presetHandler->setCurrentPresetMap(state.newMapText);
        currentPresetMap = presetHandler->readPresetMap(state.newMapText);
      }
      ImGui::SameLine();
      if (ImGui::Button("Cancel")) {
        state.newMapText = "";
        state.newMap = false;
      }
    }
  }
  ImGui::PopID();
  return state;
}

bool ecInterface::onMouseDown(const Mouse &m) {
  if (mIsLinkingParamAndMIDI) mIsLinkingParamAndMIDI = false;
  return true;
}

void ecInterface::onMessage(al::osc::Message &m) {  // OSC input handling
  // m.print();
  if (isOSCOn) {
    float val = 0;
    if (m.typeTags()[0] == 's') {
      m >> oscValString;
      oscMessageType = "s";
    } else if (m.typeTags()[0] == 'f') {
      m >> oscValFloat;
      oscMessageType = "f";
      val = oscValFloat;
    } else if (m.typeTags()[0] == 'i') {
      m >> oscValInt;
      oscMessageType = "i";
      val = oscValInt;
    } else if (m.typeTags()[0] == 'd') {
      m >> oscValDouble;
      oscMessageType = "d";
      val = oscValDouble;
    }

    for (int i = 0; i < consts::NUM_PARAMS; i++) {
      if (m.addressPattern() == granulator.ECParameters[i]->mOscArgument) {
        if (granulator.ECParameters[i]->mOscCustomRange) {
          val = (val - granulator.ECParameters[i]->mOscMin) /
                fabs(granulator.ECParameters[i]->mOscMax - granulator.ECParameters[i]->mOscMin);
          val = util::outputValInRange(val, granulator.ECParameters[i]->getCurrentMin(),
                                       granulator.ECParameters[i]->getCurrentMax(), false);
        }
        granulator.ECParameters[i]->setParam(val);
      }
      if (m.addressPattern() == granulator.ECModParameters[i]->mOscArgument) {
        if (granulator.ECModParameters[i]->mOscCustomRange) {
          val =
            (val - granulator.ECModParameters[i]->mOscMin) /
            fabs(granulator.ECModParameters[i]->mOscMax - granulator.ECModParameters[i]->mOscMin);
          val = util::outputValInRange(val, granulator.ECModParameters[i]->param.getCurrentMin(),
                                       granulator.ECModParameters[i]->param.getCurrentMax(), false);
        }
        granulator.ECModParameters[i]->param.setParam(val);
      }
    }
    for (int i = 0; i < consts::NUM_LFOS; i++) {
      if (m.addressPattern() == granulator.LFOParameters[i]->mOscArgument) {
        if (granulator.LFOParameters[i]->mOscCustomRange) {
          val = (val - granulator.LFOParameters[i]->mOscMin) /
                fabs(granulator.LFOParameters[i]->mOscMax - granulator.LFOParameters[i]->mOscMin);
          val =
            util::outputValInRange(val, granulator.LFOParameters[i]->frequency->getCurrentMin(),
                                   granulator.LFOParameters[i]->frequency->getCurrentMax(), false);
        }
        granulator.LFOParameters[i]->frequency->setParam(val);
      }
    }
    if (m.addressPattern() == morphTimeOSCArg) {
      if (morphTimeOscCustomRange) {
        val = (val - morphTimeOscMin) / fabs(morphTimeOscMax - morphTimeOscMin);
        val = util::outputValInRange(val, 0, MAX_MORPH_TIME, false);
      }
      mPresets->setMorphTime(val);
    }
    if (m.addressPattern() == presetOSCArg) {
      mPresets->recallPreset(mPresets->getPresetName(int(val)));
    }
    if (m.addressPattern() == fileNameOSCArg) {
      buf1 = oscValString;
    }
    if (m.addressPattern() == outputFolderOSCArg) {
      soundOutput = oscValString;
    }
    if (m.addressPattern() == recordOSCArg) {
      if (val != 0) {
        recordFilename = buf1;
        std::cout << soundOutput << recordFilename << std::endl;
        mRecorder.start(soundOutput + recordFilename, audioIO().framesPerSecond(),
                        audioIO().channelsOut() <= 1 ? 1 : 2, 8192, gam::SoundFile::WAV,
                        gam::SoundFile::FLOAT);
      } else {
        mRecorder.stop();
      }
    }
  }
}

void ecInterface::createAudioThumbnail(float *soundfile, int lengthInSamples) {
  // cap thumbnail length to 1000 samples
  int thumbnailLength = lengthInSamples > 1000 ? 1000 : lengthInSamples;
  // size of chunks to find min/max over
  float chunkSize = float(lengthInSamples) / thumbnailLength;
  int startChunk, endChunk;
  // add a new empty audiothumbnail to array
  audioThumbnails.push_back(std::make_unique<std::vector<float>>(thumbnailLength * 2, 0.0f));

  for (int i = 0; i < thumbnailLength; i++) {
    startChunk = i * chunkSize;
    endChunk = startChunk + chunkSize - 1;
    audioThumbnails.back()->data()[i * 2] =
      std::min(0.0f, *std::min_element(soundfile + startChunk, soundfile + endChunk));
    audioThumbnails.back()->data()[i * 2 + 1] =
      std::max(0.0f, *std::max_element(soundfile + startChunk, soundfile + endChunk));
  }
}

/**** Configuration File Stuff -- Implementation****/

bool ecInterface::initJsonConfig() {
  json config;
  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open()) {
    config = json::parse(ifs);
    if (config.find(consts::MIDI_PRESET_NAMES_KEY) == config.end())
      config[consts::MIDI_PRESET_NAMES_KEY] = json::array();

    if (config.find(consts::OSC_PRESET_NAMES_KEY) == config.end())
      config[consts::OSC_PRESET_NAMES_KEY] = json::array();

    if (config.find(consts::SAMPLE_PRESET_NAMES_KEY) == config.end())
      config[consts::SAMPLE_PRESET_NAMES_KEY] = json::array();

    if (config.find(consts::SOUND_OUTPUT_PATH_KEY) == config.end())
      config[consts::SOUND_OUTPUT_PATH_KEY] =
        al::File::conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    if (config.find(consts::SAMPLE_RATE_KEY) == config.end())
      config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

    if (config.find(consts::BUFFER_SIZE_KEY) == config.end())
      config[consts::BUFFER_SIZE_KEY] = consts::BUFFER_SIZE;

    if (config.find(consts::LIGHT_MODE_KEY) == config.end())
      config[consts::LIGHT_MODE_KEY] = consts::LIGHT_MODE;

    if (config.find(consts::FONT_SCALE_KEY) == config.end())
      config[consts::FONT_SCALE_KEY] = consts::FONT_SCALE;

    if (config.find(consts::WINDOW_WIDTH_KEY) == config.end())
      config[consts::WINDOW_WIDTH_KEY] = consts::WINDOW_WIDTH;

    if (config.find(consts::WINDOW_HEIGHT_KEY) == config.end())
      config[consts::WINDOW_HEIGHT_KEY] = consts::WINDOW_HEIGHT;

    if (config.find(consts::FULLSCREEN_KEY) == config.end())
      config[consts::FULLSCREEN_KEY] = consts::FULLSCREEN;

    if (config.find(consts::IS_FIRST_LAUNCH_KEY) == config.end())
      config[consts::IS_FIRST_LAUNCH_KEY] = consts::IS_FIRST_LAUNCH;

    if (config.find(consts::DEFAULT_AUDIO_DEVICE_KEY) == config.end())
      config[consts::DEFAULT_AUDIO_DEVICE_KEY] = consts::DEFAULT_AUDIO_DEVICE;

    if (config.find(consts::LEAD_CHANNEL_KEY) == config.end())
      config[consts::LEAD_CHANNEL_KEY] = consts::DEFAULT_LEAD_CHANNEL;

    if (config.find(consts::CLIP_AUDIO_KEY) == config.end())
      config[consts::CLIP_AUDIO_KEY] = consts::DEFAULT_CLIP_AUDIO;

    if (config.find(consts::OMIT_SOUNDFILE_PARAM_KEY) == config.end())
      config[consts::OMIT_SOUNDFILE_PARAM_KEY] = consts::DEFAULT_OMIT_SOUNDFILE_PARAM;

    if (config.find(consts::HARD_RESET_SCANBEGIN_KEY) == config.end())
      config[consts::HARD_RESET_SCANBEGIN_KEY] = consts::DEFAULT_HARD_RESET_SCANBEGIN;

  } else {
    config[consts::MIDI_PRESET_NAMES_KEY] = json::array();

    config[consts::OSC_PRESET_NAMES_KEY] = json::array();

    config[consts::SAMPLE_PRESET_NAMES_KEY] = json::array();

    config[consts::SOUND_OUTPUT_PATH_KEY] =
      al::File::conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

    config[consts::BUFFER_SIZE_KEY] = consts::BUFFER_SIZE;

    config[consts::LIGHT_MODE_KEY] = consts::LIGHT_MODE;

    config[consts::FONT_SCALE_KEY] = consts::FONT_SCALE;

    config[consts::WINDOW_WIDTH_KEY] = consts::WINDOW_WIDTH;

    config[consts::WINDOW_HEIGHT_KEY] = consts::WINDOW_HEIGHT;

    config[consts::FULLSCREEN_KEY] = consts::FULLSCREEN;

    config[consts::IS_FIRST_LAUNCH_KEY] = consts::IS_FIRST_LAUNCH;

    config[consts::DEFAULT_AUDIO_DEVICE_KEY] = consts::DEFAULT_AUDIO_DEVICE;

    config[consts::LEAD_CHANNEL_KEY] = consts::DEFAULT_LEAD_CHANNEL;

    config[consts::CLIP_AUDIO_KEY] = consts::DEFAULT_CLIP_AUDIO;

    config[consts::OMIT_SOUNDFILE_PARAM_KEY] = consts::DEFAULT_OMIT_SOUNDFILE_PARAM;

    config[consts::HARD_RESET_SCANBEGIN_KEY] = consts::DEFAULT_HARD_RESET_SCANBEGIN;
  }

  std::ofstream file((userPath + configFile).c_str());
  if (file.is_open()) file << config;

  return false;
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

bool ecInterface::jsonWriteMapToConfig(std::unordered_set<std::string> &presetNames,
                                       std::string key) {
  json config;

  std::ifstream ifs(userPath + configFile);
  if (ifs.is_open()) config = json::parse(ifs);

  json preset_names = json::array();
  for (auto iter = presetNames.begin(); iter != presetNames.end(); iter++) {
    preset_names.push_back(*iter);
  }
  config[key] = preset_names;

  std::ofstream file((userPath + configFile).c_str());
  if (file.is_open()) {
    file << config;
    return true;
  } else {
    return false;
  }
}

json ecInterface::jsonReadConfig() {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open()) config = json::parse(ifs);

  return config;
}

void ecInterface::setMIDIPresetNames(json preset_names) {
  for (auto iter = preset_names.begin(); iter != preset_names.end(); iter++) {
    MIDIPresetNames.insert(iter->get<std::string>());
  }
}

void ecInterface::setOSCPresetNames(json preset_names) {
  for (auto iter = preset_names.begin(); iter != preset_names.end(); iter++) {
    OSCPresetNames.insert(iter->get<std::string>());
  }
}

void ecInterface::setSoundFilePresetNames(json preset_names) {
  for (auto iter = preset_names.begin(); iter != preset_names.end(); iter++) {
    SoundFilePresetNames.insert(iter->get<std::string>());
  }
}

void ecInterface::setSoundOutputPath(std::string sound_output_path) {
  soundOutput = al::File::conformPathToOS(sound_output_path);
}

void ecInterface::setAudioSettings(float sample_rate, int buffer_size) {
  granulator.setGlobalSamplingRate(sample_rate);
  configureAudio(sample_rate, buffer_size, consts::MAX_AUDIO_OUTS, consts::DEVICE_NUM);
}

void ecInterface::setColorSchemeMode(bool is_light) {
  light = is_light;
  if (!light) {
    PrimaryColor = &PrimaryDark;
    ECyellow = &YellowDark;
    ECred = &RedDark;
    ECgreen = &GreenDark;
    ECblue = &BlueDark;
    Shade1 = &Shade1Dark;
    Shade2 = &Shade2Dark;
    Shade3 = &Shade3Dark;
    Text = &TextDark;
  } else {
    PrimaryColor = &PrimaryLight;
    ECyellow = &YellowLight;
    ECred = &RedLight;
    ECgreen = &GreenLight;
    ECblue = &BlueLight;
    Shade1 = &Shade1Light;
    Shade2 = &Shade2Light;
    Shade3 = &Shade3Light;
    Text = &TextLight;
  }
}

void ecInterface::setFontScale(float font_scale) { fontScale = font_scale; }

void ecInterface::setWindowDimensions(float width, float height) {
  windowWidth = width;
  windowHeight = height;
  dimensions(width, height);
}

// MIDI Preset Jsons
void ecInterface::writeJSONMIDIPreset(std::string name, bool allowOverwrite) {
  if (name == "") return;

  std::string filename = name;
  if (!allowOverwrite) {
    int counter = 1;
    while (File::exists(userPath + midiPresetsPath + filename + ".json") && counter < 9999) {
      filename = name + "_" + std::to_string(counter++);
    }
  }

  MIDIPresetNames.insert(filename);
  jsonWriteMapToConfig(MIDIPresetNames, consts::MIDI_PRESET_NAMES_KEY);
  json midi_config = json::array();

  json temp;
  for (int index = 0; index < ActiveMIDI.size(); index++) {
    ActiveMIDI[index].toJSON(temp);
    midi_config.push_back(temp);
  }

  std::ofstream file((userPath + midiPresetsPath + filename + ".json").c_str());
  if (file.is_open()) file << midi_config;
}

void ecInterface::loadJSONMIDIPreset(std::string midi_preset_name) {
  std::ifstream ifs(userPath + midiPresetsPath + midi_preset_name + ".json");

  json midi_config;

  if (ifs.is_open())
    midi_config = json::parse(ifs);
  else
    return;

  for (int index = 0; index < midi_config.size(); index++) {
    MIDIKey temp;
    temp.fromJSON(midi_config[index]);
    ActiveMIDI.push_back(temp);
  }
}

void ecInterface::deleteJSONMIDIPreset(std::string midi_preset_name) {
  MIDIPresetNames.erase(midi_preset_name);
  jsonWriteMapToConfig(MIDIPresetNames, consts::MIDI_PRESET_NAMES_KEY);
  std::remove((userPath + midiPresetsPath + midi_preset_name + ".json").c_str());
}

// OSC Preset Jsons
void ecInterface::writeJSONOSCPreset(std::string name, bool allowOverwrite) {
  if (name == "") return;

  std::string filename = name;
  if (!allowOverwrite) {
    int counter = 1;
    while (File::exists(userPath + oscPresetsPath + filename + ".json") && counter < 9999) {
      filename = name + "_" + std::to_string(counter++);
    }
  }

  OSCPresetNames.insert(filename);
  jsonWriteMapToConfig(OSCPresetNames, consts::OSC_PRESET_NAMES_KEY);
  json osc_config = json::array();

  json temp;
  json package;

  temp["ADDRESS"] = oscAddr;
  temp["PORT"] = oscPort;
  temp["TIMEOUT"] = oscTimeout;
  package["OSC_SETUP"] = temp;
  temp.clear();
  for (int i = 0; i < NUM_PARAMS; i++) {
    temp["MAX"] = granulator.ECParameters[i]->mOscMax;
    temp["MIN"] = granulator.ECParameters[i]->mOscMin;
    temp["MAPPING"] = granulator.ECParameters[i]->mOscCustomRange;
    temp["OSC_ARG"] = granulator.ECParameters[i]->mOscArgument;
    package[granulator.ECParameters[i]->getDisplayName()] = temp;
  }
  for (int i = 0; i < NUM_PARAMS; i++) {
    temp["MAX"] = granulator.ECModParameters[i]->mOscMax;
    temp["MIN"] = granulator.ECModParameters[i]->mOscMin;
    temp["MAPPING"] = granulator.ECModParameters[i]->mOscCustomRange;
    temp["OSC_ARG"] = granulator.ECModParameters[i]->mOscArgument;
    package[granulator.ECParameters[i]->getDisplayName() + "_MOD"] = temp;
  }
  for (int i = 0; i < NUM_LFOS; i++) {
    temp["MAX"] = granulator.LFOParameters[i]->mOscMax;
    temp["MIN"] = granulator.LFOParameters[i]->mOscMin;
    temp["MAPPING"] = granulator.LFOParameters[i]->mOscCustomRange;
    temp["OSC_ARG"] = granulator.LFOParameters[i]->mOscArgument;
    package[toString("LFO" + toString(i + 1))] = temp;
  }
  temp["MAX"] = morphTimeOscMax;
  temp["MIN"] = morphTimeOscMin;
  temp["MAPPING"] = morphTimeOscCustomRange;
  temp["OSC_ARG"] = morphTimeOSCArg;
  package["MORPH_TIME"] = temp;

  temp["OSC_ARG"] = presetOSCArg;
  package["PRESET"] = temp;

  temp["OSC_ARG"] = recordOSCArg;
  package["RECORD"] = temp;

  temp["OSC_ARG"] = fileNameOSCArg;
  package["FILE_NAME"] = temp;

  temp["OSC_ARG"] = outputFolderOSCArg;
  package["OUTPUT_FOLDER"] = temp;

  osc_config.push_back(package);

  std::ofstream file((userPath + oscPresetsPath + filename + ".json").c_str());
  if (file.is_open()) file << osc_config;
}

void ecInterface::loadJSONOSCPreset(std::string osc_preset_name) {
  std::ifstream ifs(userPath + oscPresetsPath + osc_preset_name + ".json");

  json osc_config;

  if (ifs.is_open())
    osc_config = json::parse(ifs);
  else
    return;
  oscAddr = osc_config[0].at("OSC_SETUP").at("ADDRESS");
  oscPort = osc_config[0].at("OSC_SETUP").at("PORT");
  oscTimeout = osc_config[0].at("OSC_SETUP").at("TIMEOUT");

  for (int i = 0; i < NUM_PARAMS; i++) {
    std::string name = granulator.ECParameters[i]->getDisplayName();
    granulator.ECParameters[i]->mOscArgument = osc_config[0].at(name).at("OSC_ARG");
    granulator.ECParameters[i]->mOscCustomRange = osc_config[0].at(name).at("MAPPING");
    if (granulator.ECParameters[i]
          ->mOscCustomRange) {  // if custom mapping is on, load mapping min/max
      granulator.ECParameters[i]->mOscMin = osc_config[0].at(name).at("MIN");
      granulator.ECParameters[i]->mOscMax = osc_config[0].at(name).at("MAX");
    } else {  // if custom mapping is off, set osc min/max to current parameter min/max
      granulator.ECParameters[i]->mOscMin = granulator.ECParameters[i]->getCurrentMin();
      granulator.ECParameters[i]->mOscMax = granulator.ECParameters[i]->getCurrentMax();
    }
    name = name + "_MOD";
    granulator.ECModParameters[i]->mOscArgument = osc_config[0].at(name).at("OSC_ARG");
    granulator.ECModParameters[i]->mOscCustomRange = osc_config[0].at(name).at("MAPPING");
    if (granulator.ECModParameters[i]
          ->mOscCustomRange) {  // if custom mapping is on, load mapping min/max
      granulator.ECModParameters[i]->mOscMin = osc_config[0].at(name).at("MIN");
      granulator.ECModParameters[i]->mOscMax = osc_config[0].at(name).at("MAX");
    } else {  // if custom mapping is off, set osc min/max to current parameter min/max
      granulator.ECModParameters[i]->mOscMin = granulator.ECModParameters[i]->param.getCurrentMin();
      granulator.ECModParameters[i]->mOscMax = granulator.ECModParameters[i]->param.getCurrentMax();
    }
  }
  for (int i = 0; i < NUM_LFOS; i++) {
    std::string name = toString("LFO" + toString(i + 1));
    granulator.LFOParameters[i]->mOscArgument = osc_config[0].at(name).at("OSC_ARG");
    granulator.LFOParameters[i]->mOscCustomRange = osc_config[0].at(name).at("MAPPING");
    if (granulator.LFOParameters[i]->mOscCustomRange) {
      granulator.LFOParameters[i]->mOscMin = osc_config[0].at(name).at("MIN");
      granulator.LFOParameters[i]->mOscMax = osc_config[0].at(name).at("MAX");
    } else {
      granulator.LFOParameters[i]->mOscMin =
        granulator.LFOParameters[i]->frequency->getCurrentMin();
      granulator.LFOParameters[i]->mOscMax =
        granulator.LFOParameters[i]->frequency->getCurrentMax();
    }
  }
  morphTimeOSCArg = osc_config[0].at("MORPH_TIME").at("OSC_ARG");
  morphTimeOscCustomRange = osc_config[0].at("MORPH_TIME").at("MAPPING");
  if (morphTimeOscCustomRange) {
    morphTimeOscMin = osc_config[0].at("MORPH_TIME").at("MIN");
    morphTimeOscMax = osc_config[0].at("MORPH_TIME").at("MAX");
  } else {
    morphTimeOscMin = 0;
    morphTimeOscMax = 50;
  }

  presetOSCArg = osc_config[0].at("PRESET").at("OSC_ARG");
  recordOSCArg = osc_config[0].at("RECORD").at("OSC_ARG");
  fileNameOSCArg = osc_config[0].at("FILE_NAME").at("OSC_ARG");
  outputFolderOSCArg = osc_config[0].at("OUTPUT_FOLDER").at("OSC_ARG");
}

void ecInterface::deleteJSONOSCPreset(std::string osc_preset_name) {
  OSCPresetNames.erase(osc_preset_name);
  jsonWriteMapToConfig(OSCPresetNames, consts::OSC_PRESET_NAMES_KEY);
  std::remove((userPath + oscPresetsPath + osc_preset_name + ".json").c_str());
}

void ecInterface::writeJSONSoundFilePreset(std::string name, bool allowOverwrite) {
  if (name == "") return;

  std::string filename = name;
  if (!allowOverwrite) {
    int counter = 1;
    while (File::exists(userPath + samplePresetsPath + filename + ".json") && counter < 9999) {
      filename = name + "_" + std::to_string(counter++);
    }
  }

  SoundFilePresetNames.insert(filename);
  jsonWriteMapToConfig(SoundFilePresetNames, consts::SAMPLE_PRESET_NAMES_KEY);
  json sound_file_config = json::array();

  for (int index = 0; index < granulator.soundClipFileName.size(); index++) {
    sound_file_config.push_back(granulator.soundClipFileName[index]);
  }

  std::ofstream file((userPath + samplePresetsPath + filename + ".json").c_str());
  if (file.is_open()) file << sound_file_config;
}

// JSON Sound File Load
std::vector<std::string> ecInterface::loadJSONSoundFilePreset(std::string sound_file_preset_name) {
  std::ifstream ifs(userPath + samplePresetsPath + sound_file_preset_name + ".json");

  json sound_file_config;

  if (ifs.is_open())
    sound_file_config = json::parse(ifs);
  else
    return {};

  // Lock so the audio thread doesn't misread buffer
  // while clearing files.
  std::unique_lock<std::mutex> lk(mLock);
  // granulator.clearSoundFiles();
  int counter = 0;
  int size = granulator.soundClipFileName.size();
  for (int index = 0; index < size; index++) {
    if (std::find(sound_file_config.begin(), sound_file_config.end(),
                  granulator.soundClipFileName[index - counter]) == sound_file_config.end()) {
      audioThumbnails.erase(audioThumbnails.begin() + (index - counter));
      granulator.removeSoundFile(index - counter);
      counter++;
    }
  }

  std::string temp_path;
  std::vector<std::string> failed_loads;
  for (int index = 0; index < sound_file_config.size(); index++) {
    temp_path = al::File::conformPathToOS(sound_file_config[index]);
    if (std::find(granulator.soundClipFileName.begin(), granulator.soundClipFileName.end(),
                  temp_path) != granulator.soundClipFileName.end()) {
      continue;
    }
    if (al::File::exists(temp_path)) {
      granulator.loadSoundFileRT(sound_file_config[index]);
      createAudioThumbnail(granulator.soundClip[granulator.soundClip.size() - 1]->data,
                           granulator.soundClip[granulator.soundClip.size() - 1]->size);
    } else {
      failed_loads.push_back(temp_path);
    }
  }
  granulator.mModClip = 0;
  return failed_loads;
}

void ecInterface::deleteJSONSoundFilePreset(std::string sound_file_preset_name) {
  SoundFilePresetNames.erase(sound_file_preset_name);
  jsonWriteMapToConfig(SoundFilePresetNames, consts::SAMPLE_PRESET_NAMES_KEY);
  std::remove((userPath + samplePresetsPath + sound_file_preset_name + ".json").c_str());
}

struct InputTextCallback_UserData {
  std::string *Str;
  ImGuiInputTextCallback ChainCallback;
  void *ChainCallbackUserData;
};

static int InputTextCallback(ImGuiInputTextCallbackData *data) {
  InputTextCallback_UserData *user_data = (InputTextCallback_UserData *)data->UserData;
  if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
    // Resize string callback
    // If for some reason we refuse the new length
    // (BufTextLen) and/or capacity (BufSize) we need
    // to set them back to what we want.
    std::string *str = user_data->Str;
    IM_ASSERT(data->Buf == str->c_str());
    str->resize(data->BufTextLen);
    data->Buf = (char *)str->c_str();
  } else if (user_data->ChainCallback) {
    // Forward to user callback, if any
    data->UserData = user_data->ChainCallbackUserData;
    return user_data->ChainCallback(data);
  }
  return 0;
};

bool ecInterface::InputText(const char *label, std::string *str, ImGuiInputTextFlags flags,
                            ImGuiInputTextCallback callback, void *user_data) {
  IM_ASSERT((flags & ImGuiInputTextFlags_CallbackResize) == 0);
  flags |= ImGuiInputTextFlags_CallbackResize;

  InputTextCallback_UserData cb_user_data;
  cb_user_data.Str = str;
  cb_user_data.ChainCallback = callback;
  cb_user_data.ChainCallbackUserData = user_data;
  return ImGui::InputText(label, (char *)str->c_str(), str->capacity() + 1, flags,
                          InputTextCallback, &cb_user_data);
}