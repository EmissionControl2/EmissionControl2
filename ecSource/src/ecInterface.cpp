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
#ifdef __APPLE__ // Uses userPath
  configFile = consts::DEFAULT_CONFIG_FILE;
  presetsPath = consts::DEFAULT_PRESETS_PATH;
  midiPresetsPath = consts::DEFAULT_MIDI_PRESETS_PATH;

  execDir = util::getContentPath_OSX(execDir);
  al::Dir::make(userPath + consts::PERSISTENT_DATA_PATH);
  al::Dir::make(userPath + consts::DEFAULT_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_MIDI_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);
  al::Dir::make(userPath + consts::DEFAULT_CONFIG_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PATH);
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
  midiPresetsPath = configPath + "/midi_presets";

  // create config directories if needed
  al::Dir::make(userPath + configPath + "/config");
  al::Dir::make(userPath + presetsPath);
  al::Dir::make(userPath + midiPresetsPath);
  al::Dir::make(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);
  opener = "xdg-open ";
#endif

#ifdef _WIN32
  configFile = consts::DEFAULT_CONFIG_FILE;
  presetsPath = consts::DEFAULT_PRESETS_PATH;
  midiPresetsPath = consts::DEFAULT_MIDI_PRESETS_PATH;

  al::Dir::make(userPath + consts::PERSISTENT_DATA_PATH);
  al::Dir::make(userPath + consts::DEFAULT_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_MIDI_PRESETS_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);
  al::Dir::make(userPath + consts::DEFAULT_CONFIG_PATH);
  al::Dir::make(userPath + consts::DEFAULT_SAMPLE_PATH);
  opener = "start ";
#endif

  initJsonConfig();
  json config = jsonReadConfig();
  setMIDIPresetNames(config.at(consts::MIDI_PRESET_NAMES_KEY));
  setSoundOutputPath(config.at(consts::SOUND_OUTPUT_PATH_KEY));
  setAudioSettings(config.at(consts::SAMPLE_RATE_KEY));
  setColorSchemeMode(config.at(consts::LIGHT_MODE_KEY));
  setFontScale(config.at(consts::FONT_SCALE_KEY));
  setWindowDimensions(config.at(consts::WINDOW_WIDTH_KEY), config.at(consts::WINDOW_HEIGHT_KEY));
  setFirstLaunch(config.at(consts::IS_FIRST_LAUNCH_KEY));
  setAudioDevice(config.at(consts::DEFAULT_AUDIO_DEVICE_KEY));
  setInitFullscreen(false);

// Load in all files in at specified directory.
// Set output directory for presets.
// Set output directory of recorded files.
#if defined __APPLE__ || defined _WIN32
  if (isFirstLaunch)
    al::File::copy(execDir + "Resources/samples/440sine48k.wav",
                   userPath + consts::DEFAULT_SAMPLE_PATH);
  granulator.loadInitSoundFiles(userPath + consts::DEFAULT_SAMPLE_PATH);
  mPresets = std::make_unique<al::PresetHandler>(al::File::conformPathToOS(userPath + presetsPath));
#endif

#ifdef __linux__
  granulator.loadInitSoundFiles(consts::DEFAULT_SAMPLE_PATH);
  mPresets = std::make_unique<al::PresetHandler>(al::File::conformPathToOS(userPath + presetsPath));
#endif

  for (int i = 0; i < granulator.soundClip.size(); i++)
    createAudioThumbnail(granulator.soundClip[i]->data, granulator.soundClip[i]->size);
  initMIDI();

  gam::sampleRate(audioIO().framesPerSecond());
  granulator.initialize(&audioIO());
  audioIO().append(mRecorder);
  audioIO().append(mHardClip);
  audioIO().channelsIn(0);
  audioIO().setStreamName("EmissionControl2");
  auto a_d = AudioDevice(currentAudioDevice, AudioDevice::OUTPUT);
  if (!a_d.valid()) {
    audioIO().deviceOut(-1);
    currentAudioDevice = AudioDevice::defaultOutput().name();
  } else
    audioIO().deviceOut(a_d);
  audioIO().print();
  std::cout << "Frame Rate:  " + std::to_string((int)audioIO().framesPerSecond()) << std::endl;
}

void ecInterface::onCreate() {
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

#ifdef __APPLE__
  bodyFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 16.0f);
  titleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 20.0f);
  ferrariFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/ferrari.ttf").c_str(), 16.0f);
#endif

#ifdef __linux__
  bodyFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      ("/usr/local/share/fonts/EmissionControl2/Roboto-Medium.ttf"), 16.0f);
  titleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      ("/usr/local/share/fonts/EmissionControl2/Roboto-Medium.ttf"), 20.0f);
  ferrariFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      ("/usr/local/share/fonts/EmissionControl2/ferrari.ttf"), 16.0f);
#endif

#ifdef _WIN32
  bodyFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 16.0f);
  titleFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/Roboto-Medium.ttf").c_str(), 20.0f);
  ferrariFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
      (execDir + "Resources/Fonts/ferrari.ttf").c_str(), 16.0f);
#endif

  currentPresetMap = mPresets->readPresetMap("default");
  setGUIParams();
  audioIO().stop();
}

void ecInterface::onExit() {
  jsonWriteToConfig(windowWidth, consts::WINDOW_WIDTH_KEY);
  jsonWriteToConfig(windowHeight, consts::WINDOW_HEIGHT_KEY);
  jsonWriteToConfig(isFullScreen, consts::FULLSCREEN_KEY);
  jsonWriteToConfig(false, consts::IS_FIRST_LAUNCH_KEY);
}

void ecInterface::onSound(AudioIOData &io) { granulator.onProcess(io); }

void ecInterface::onDraw(Graphics &g) {
  setGUIParams();
  framecounter++;
  if (unlearnFlash > 0)
    unlearnFlash--;
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

  if (granulator.getNumberOfAudioFiles() == 0) {
    ImGui::OpenPopup("No Sound File");
    if (audioIO().isRunning())
      audioIO().stop();
    noSoundFiles = true;
  }

  if (granulator.getNumberOfAudioFiles() != 0) {
    noSoundFiles = false;
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
          }
          NFD_PathSet_Free(&pathSet);
        }
      }
      if (ImGui::MenuItem("Remove Current Sound File", "")) {
        audioThumbnails.erase(audioThumbnails.begin() + granulator.mModClip);
        granulator.removeCurrentSoundFile();
      }
      ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("MIDI")) {
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

      if (ImGui::MenuItem("Save MIDI Learn Preset", "")) {
        isMIDIWriteWindow = true;
      }

      if (ImGui::MenuItem("Load MIDI Learn Preset", "")) {
        isMIDILoadWindow = true;
      }

      if (ImGui::MenuItem("Delete MIDI Learn Preset", "")) {
        isMIDIDeleteWindow = true;
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
      if (ImGui::MenuItem("About", "")) {
        aboutWindow = true;
      }
      ImGui::EndMenu();
    }
    if (audioIO().isRunning())
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0, 1.0, 1.0, 1.0));
    else
      ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(*ECred));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8, 0.5, 0.5, 1.0));
    ImGui::PushFont(ferrariFont);
    ImGui::SetCursorPosX(width() - 106 * fontScale);
    if (ImGui::Button("ENGINE START")) {
      if (!audioIO().isRunning()) {
        if (granulator.getNumberOfAudioFiles() != 0) {
          isPaused = false;
          audioIO().open();
          audioIO().start();
          noSoundFiles = false;
        } else {
          ImGui::OpenPopup("No Sound File");
          noSoundFiles = true;
        }
      } else if (audioIO().isRunning()) {
        isPaused = true;
        audioIO().stop();
        audioIO().close();
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
    ImGui::Text(("Select Up to " + std::to_string(consts::MAX_MIDI_IN) + " MIDI Inputs:").c_str());
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
    if (ImGui::Button("Cancel"))
      ImGui::CloseCurrentPopup();

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
        if (temp_counter >= consts::MAX_MIDI_IN)
          break;
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
    ImGui::InputText("Enter Preset Name", mCurrentPresetName, 50,
                     ImGuiInputTextFlags_CtrlEnterForNewLine | ImGuiInputTextFlags_CharsNoBlank);

    if (ImGui::Button("Cancel"))
      ImGui::CloseCurrentPopup();

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
    writeJSONMIDIPreset(mCurrentPresetName, allowMIDIPresetOverwrite);
    isWriteJSON = false;
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
  if (!fontScaleOpen)
    jsonWriteToConfig(fontScale, consts::FONT_SCALE_KEY);

  // PopUp Audio IO window --------------------------------------------
  // This enables starting and stopping audio as well as selecting
  // Audio device and its parameters
  // if statement opens Audio IO popup if chosen from menu
  if (displayIO) {
    ImGui::OpenPopup("Audio Settings");
  }
  bool audioOpen = true;
  ImGui::SetNextWindowSizeConstraints(ImVec2(300 * fontScale, (sliderheight * 6)),
                                      ImVec2(windowWidth, windowHeight));
  if (ImGui::BeginPopupModal("Audio Settings", &audioOpen)) {
    drawAudioIO(&audioIO(), displayIO);
    ImGui::EndPopup();
  }

  if (aboutWindow) {
    ImGui::OpenPopup("About");
  }

  bool aboutOpen = true;
  ImGui::SetNextWindowSize(ImVec2(500 * fontScale, 350 * adjustScaleY));
  if (ImGui::BeginPopupModal("About", &aboutOpen, ImGuiWindowFlags_NoResize)) {
    const char *aboutLines[13] = {
        "Curtis Roads, Jack Kilgore, Rodney DuPlessis",
        "CREATE (Center for Research in Electronic Art Technology)",
        "University of California, Santa Barbara",
        " ",
        "Thanks to the Allosphere Research Group for their work on Allolib,",
        "the framework on which this software is built.",
        "Special thanks to Dr. Andres Cabrera for help and guidance",
        "and for handling the Windows port of the software",
        "Supported by a Faculty Research Grant from the UCSB Academic Senate",
        " ",
        "Copyright 2020 Curtis Roads, Jack Kilgore, Rodney Duplessis",
        "This program comes with absolutely no warranty.",
        "See the GNU General Public License, version 3 or later for details."};
    ImGui::PushFont(ferrariFont);
    ImGui::SetCursorPosX((250 * fontScale) - (ImGui::CalcTextSize("EmissionControl2").x / 2));
    ImGui::Text("EmissionControl2");
    ImGui::PopFont();
    for (int i = 0; i < 13; i++) {
      ImGui::SetCursorPosX((250 * fontScale) - (ImGui::CalcTextSize(aboutLines[i]).x / 2));
      ImGui::TextUnformatted(aboutLines[i]);
    }
    ImGui::SetCursorPosX((250 * fontScale) - (100 * fontScale));
    if (ImGui::Button("License", ImVec2(200 * fontScale, 20 * adjustScaleY))) {
      system((opener + licenseURL).c_str());
    }
    ImGui::EndPopup();
  }

  // Draw Granulator Controls -----------------------------------------
  if (granulator.getNumberOfAudioFiles() != 0) {
    // Set Dynamic Slider Text
    granulator.ECParameters[consts::SOUND_FILE]->setSliderText(
        granulator.getCurrentAudioFileName());
  }
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("GRANULATION CONTROLS", 0, menuBarHeight, windowWidth / 2,
                           firstRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    colPushCount = 0;
    // set alternating slider background shade (green if midi learning)
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
      // This inits. the onMidiMessage loop to listen for midi input.
      // This first MIDI input to come through will be linked.
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

  // Draw modulation window -------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("MODULATION CONTROLS", windowWidth / 2, menuBarHeight, windowWidth / 2,
                           firstRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_PARAMS; index++) {
    colPushCount = 0;
    // set alternating slider background shade (green if midi learning)
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
      // This inits. the onMidiMessage loop to listen for midi input.
      // This first MIDI input to come through will be linked.
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

  // Draw preset window -----------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("PRESETS", 0, NextWindowYPosition, windowWidth / 4, secondRowHeight,
                           flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  ecInterface::ECdrawPresetHandler(mPresets.get(), 12, 3);
  ImGui::PopFont();
  ParameterGUI::endPanel();

  // Draw recorder window ---------------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("RECORDER", windowWidth / 4, NextWindowYPosition, windowWidth / 4,
                           secondRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut(), soundOutput);
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

  // Draw LFO parameters window ---------------------------------------
  ImGui::PushFont(titleFont);
  ParameterGUI::beginPanel("LFO CONTROLS", windowWidth / 2, NextWindowYPosition, windowWidth / 2,
                           secondRowHeight, flags);
  ImGui::PopFont();
  ImGui::PushFont(bodyFont);
  for (int index = 0; index < consts::NUM_LFOS; index++) {
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
    // This is to tell the drawRangeSlider that this is the last parameter to check.
    // This allows mLasyKeyDown.readyToTrig to be set to false.
    if (index == consts::NUM_LFOS - 1)
      mLastKeyDown.lastParamCheck = true;

    granulator.LFOParameters[index]->drawLFOControl(&mMIDILearn, &mLastKeyDown);
    if (mMIDILearn.mParamAdd) {
      // This inits. the onMidiMessage loop to listen for midi input.
      // This first MIDI input to come through will be linked.
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_LFO);
      mIsLinkingParamAndMIDI = true;
    }
    if (mMIDILearn.mParamDel) {
      mCurrentLearningMIDIKey.setKeysIndex(index, consts::M_LFO);
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
    // Draw Scan Display ------------------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("SCAN DISPLAY", 0, NextWindowYPosition, windowWidth, graphHeight / 3,
                             graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    if (granulator.getNumberOfAudioFiles() != 0) {

      float plotWidth = ImGui::GetContentRegionAvail().x;
      float plotHeight = ImGui::GetContentRegionAvail().y;
      ImVec2 p = ImGui::GetCursorScreenPos();

      int soundFileFrames = granulator.soundClip[granulator.mModClip]->frames;

      float scanHead = granulator.ECParameters[consts::SCAN_BEGIN]->getModParam(
          granulator.ECModParameters[consts::SCAN_BEGIN]->getWidthParam());
      float scanWidth = granulator.ECParameters[consts::SCAN_RANGE]->getModParam(
          granulator.ECModParameters[consts::SCAN_RANGE]->getWidthParam());
      float scanPos = granulator.getCurrentIndex() / soundFileFrames;
      if (scanPos < 0)
        scanPos += 1;
      if (scanPos > 1)
        scanPos -= 1;

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
        ImGui::SetTooltip("Scan Head: %i% \nScan Range: %i%", int(scanHead * 100),
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

    // Draw grain histogram window --------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("ACTIVE GRAINS", 0, NextWindowYPosition, windowWidth / 4,
                             graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    ImGui::Text("Counter: %.1i ", granulator.getNumActiveVoices());

    if (grainAccum < granulator.getNumActiveVoices())
      grainAccum = granulator.getNumActiveVoices();
    if (framecounter % 2 == 0) {
      streamHistory.erase(streamHistory.begin());
      streamHistory.push_back(grainAccum);
      highestStreamCount = *max_element(streamHistory.begin(), streamHistory.end());
      if (highestStreamCount < 2)
        highestStreamCount = 2;
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
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Grains Per Second: %i", grainsPerSecond);
    ImGui::PopStyleVar();
    ImGui::PopFont();
    ParameterGUI::endPanel();

    // Draw Oscilloscope window -----------------------------------------
    ImGui::PushFont(titleFont);
    ParameterGUI::beginPanel("OSCILLOSCOPE", windowWidth / 4, NextWindowYPosition,
                             windowWidth * 11 / 16, graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 0));
    ImGui::Text("Time Frame (s):");
    ImGui::SameLine();
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (100 * fontScale));
    if (ImGui::SliderFloat("##Scope frame", &oscFrame, 0.001, 3.0, "%.3f")) {
      if (oscFrame <= 3.0 || globalSamplingRate != lastSamplingRate) {
        oscSize = (int)(oscFrame * globalSamplingRate);
        lastSamplingRate = globalSamplingRate;
      }
    }

    // Draw left channel oscilloscope
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x);
    ImGui::SetCursorPosY(graphPosY);
    ImGui::PushStyleColor(ImGuiCol_PlotLines, light ? (ImVec4)*ECblue : (ImVec4)*ECblue);
    int offset = granulator.oscBufferL.getTail() - oscSize;
    if (offset < 0)
      offset += granulator.oscBufferL.getMaxSize();
    util::Plot_RingBufferGetterData data_l(granulator.oscBufferL.data(), sizeof(float), offset,
                                           granulator.oscBufferL.getMaxSize());
    ImGui::PlotLines("##ScopeL", &util::Plot_RingBufferGetter, (void *)&data_l, oscSize, 0.0,
                     nullptr, -1, 1, ImVec2(0, ImGui::GetContentRegionAvail().y));
    // Draw a black line across the center of the scope
    ImGui::SetCursorPosY(graphPosY);
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(0, 0, 0, 255));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, (ImVec4)ImColor(0, 0, 0, 0));
    ImGui::PlotLines("##black_line", &blackLine[0], 2, 0, nullptr, -1, 1,
                     ImVec2(0, ImGui::GetContentRegionAvail().y), sizeof(float)); // before opt
    // Draw right channel oscilloscope
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)*ECred);
    ImGui::SetCursorPosY(graphPosY + 1);
    util::Plot_RingBufferGetterData data_r(granulator.oscBufferR.data(), sizeof(float), offset,
                                           granulator.oscBufferR.getMaxSize());
    ImGui::PlotLines("##ScopeR", &util::Plot_RingBufferGetter, (void *)&data_r, oscSize, 0.0,
                     nullptr, -1, 1, ImVec2(0, ImGui::GetContentRegionAvail().y));
    // Draw a black line across the center of the scope
    ImGui::PushStyleColor(ImGuiCol_PlotLines, (ImVec4)ImColor(0, 0, 0, 255));
    ImGui::SetCursorPosY(graphPosY + 1);
    ImGui::PlotLines("##black_line", &blackLine[0], 2, 0, nullptr, -1, 1,
                     ImVec2(0, ImGui::GetContentRegionAvail().y), sizeof(float));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Oscilloscope");
    ImGui::PopStyleVar();
    ImGui::PopItemWidth();
    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar();

    ImGui::PopFont();
    ParameterGUI::endPanel();

    // Draw VU Meter window
    // ---------------------------------------------
    ImGui::PushFont(titleFont);
    if (width() < 1250)
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(4, 12));
    ParameterGUI::beginPanel("VU", windowWidth * 15 / 16, NextWindowYPosition, windowWidth * 1 / 16,
                             graphHeight * 2 / 3, graphFlags);
    ImGui::PopFont();
    ImGui::PushFont(bodyFont);
    // Size of VU meter data arrays in samples
    VUdataSize = globalSamplingRate / 30;

    float VUleft = granulator.vuBufferL.getRMS(VUdataSize);
    float VUright = granulator.vuBufferR.getRMS(VUdataSize);

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
        "##VUleft", &VUleft, 1, 0, nullptr, 0, 1,
        ImVec2((ImGui::GetContentRegionAvail().x / 2) - 4, ImGui::GetContentRegionAvail().y),
        sizeof(float));
    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("L Peak: %f", granulator.peakL);
    ImGui::PopStyleVar();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, VUrightCol);
    ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, VUrightCol);
    ImGui::PlotHistogram("##VUright", &VUright, 1, 0, nullptr, 0, 1,
                         ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y),
                         sizeof(float));
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.8);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("R Peak: %f", granulator.peakR);
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(5);
    ImGui::PopFont();
    ParameterGUI::endPanel();
  }
  if (width() < 1250)
    ImGui::PopStyleVar();

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
            noSoundFiles = false;
          }
        }
        NFD_PathSet_Free(&pathSet);
      }
      if(!noSoundFiles)
        ImGui::CloseCurrentPopup();
    }
    ImGui::EndPopup();
  }
  ImGui::PopStyleColor(18);
  ImGui::PopStyleVar(3);
  al::imguiEndFrame();

  al::imguiDraw();
}

bool ecInterface::onKeyDown(Keyboard const &k) {
  mLastKeyDown.key = k;
  mLastKeyDown.readyToTrig = true;
  mLastKeyDown.lastParamCheck = false;
  return true;
}

void ecInterface::initMIDI() {
  /* This is the single stupidest code I have ever written.

   RtMIDI seems to access trash memory, instead of mBindings[index] for the first
      *consts::MAX_MIDI_IN* setCallback functions.

   This is only seen when we have multiple midi input ports allowed.

   To get around this you simply execute consts::MAX_MIDI_IN dummy bindTo calls and then
   immediately clear mBinding of this fake data.

   This seems to properly init the pointers to mBindings memory when setCallback uses them.

   What the fuck. I hate this, I hate computers. I'm goingg outside.
  */
  for (int i = 0; i < consts::MAX_MIDI_IN; i++)
    MIDIMessageHandler::bindTo(midiIn[i], 0);
  MIDIMessageHandler::clearBindings();
  /* End of stupid as shit code. **/

  if (midiIn[0].getPortCount() > 0) {
    // Bind ourself to the RtMidiIn[0] object, to have the onMidiMessage()
    // callback called whenever a MIDI message is received
    // Open the last device found
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
  if (found)
    ActiveMIDI.erase(ActiveMIDI.begin() + index);
}

void ecInterface::drawAudioIO(AudioIO *io, bool trig) {
  struct AudioIOState {
    int currentSr = 1;
    int currentBufSize = 3;
    int currentDevice = 0;
    std::vector<std::string> devices;
  };
  auto updateOutDevices = [&](AudioIOState &state) {
    state.devices.clear();
    int numDevices = AudioDevice::numDevices();
    int dev_out_index = 0;
    for (int i = 0; i < numDevices; i++) {
      if (!AudioDevice(i).hasOutput())
        continue;

      state.devices.push_back(AudioDevice(i).name());
      if (currentAudioDevice == AudioDevice(i).name()) {
        state.currentDevice = dev_out_index;
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
      updateOutDevices(state);
    }
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - (100 * fontScale));
    if (ImGui::Combo("Device", &state.currentDevice, ParameterGUI::vector_getter,
                     static_cast<void *>(&state.devices), state.devices.size())) {
    }
    std::vector<std::string> samplingRates{"44100", "48000", "88200", "96000"};
    ImGui::Combo("Sampling Rate", &state.currentSr, ParameterGUI::vector_getter,
                 static_cast<void *>(&samplingRates), samplingRates.size());
    ImGui::PopItemWidth();
    if (ImGui::Button("Start")) {
      globalSamplingRate = std::stof(samplingRates[state.currentSr]);
      io->framesPerSecond(globalSamplingRate);
      io->framesPerBuffer(consts::BLOCK_SIZE);
      io->device(AudioDevice(state.devices.at(state.currentDevice), AudioDevice::OUTPUT));
      currentAudioDevice = state.devices.at(state.currentDevice);
      granulator.setIO(io);
      if (writeSampleRate) {
        jsonWriteToConfig(globalSamplingRate, consts::SAMPLE_RATE_KEY);
        jsonWriteToConfig(currentAudioDevice, consts::DEFAULT_AUDIO_DEVICE_KEY);
      }

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
  static char buf1[64] = "test.wav";
  ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x - 10.0f);
  ImGui::InputText("##Record Name", buf1, 63);
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
      uint32_t ringBufferSize;
      if (bufferSize == 0) {
        ringBufferSize = 8192;
      } else {
        ringBufferSize = bufferSize * numChannels * 4;
      }
      std::string filename;
      if (!state.overwriteButton) {
        filename = buf1;
        int counter = 1;
        while (File::exists(directory + filename) && counter < 9999) {
          filename = buf1;
          int lastDot = filename.find_last_of(".");
          filename = filename.substr(0, lastDot) + "_" + std::to_string(counter++) +
                     filename.substr(lastDot);
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
  ImGui::PushStyleColor(ImGuiCol_PlotHistogram,
                        light ? (ImVec4)ImColor(0, 0, 0, 150)
                              : (ImVec4)ImColor(255, 255, 255, 150));
  ImGui::PushStyleColor(ImGuiCol_PlotHistogramHovered, (ImVec4)*ECgreen);
  ImGui::PushStyleColor(ImGuiCol_Text, (ImVec4)*Text);
  ImGui::PushStyleColor(ImGuiCol_CheckMark,
                        light ? (ImVec4)ImColor(0, 0, 0, 150)
                              : (ImVec4)ImColor(255, 255, 255, 150));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(24, 12));
}

int ecInterface::getSampleRateIndex() {
  unsigned s_r = (unsigned)globalSamplingRate;
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

/**** Borrowed and modified from al_ParameterGUI.cpp****/
ecInterface::PresetHandlerState &
ecInterface::ECdrawPresetHandler(PresetHandler *presetHandler, int presetColumns, int presetRows) {
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
  if (currentPresetName.length() == 0)
    currentPresetName = "none";
  ImGui::Text("Current Preset: %s", currentPresetName.c_str());
  int counter = state.presetHandlerBank * (presetColumns * presetRows);
  if (state.storeButtonState) {
    ImGui::PushStyleColor(ImGuiCol_Text, light ? (ImVec4)*ECblue : (ImVec4)*ECgreen);
  }
  float presetWidth = (ImGui::GetContentRegionAvail().x / 12.0f) - 8.0f;
  for (int row = 0; row < presetRows; row++) {
    for (int column = 0; column < presetColumns; column++) {
      std::string name = std::to_string(counter);
      ImGui::PushID(counter);

      bool nothingStored = currentPresetMap.find(counter) == currentPresetMap.end();
      if (nothingStored)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3);

      bool is_selected = selection == counter;
      if (is_selected) {
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
      }

      if (ImGui::Selectable(name.c_str(), is_selected, 0,
                            ImVec2(presetWidth, ImGui::GetFontSize() * 1.2f))) {
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
          currentPresetMap = presetHandler->readPresetMap(state.currentBank);
        } else {
          if (presetHandler->recallPreset(counter) != "") { // Preset is available
            selection = counter;
          }
        }
      }
      if (nothingStored)
        ImGui::PopStyleVar();
      if (is_selected)
        ImGui::PopStyleColor(1);

      if (column < presetColumns - 1)
        ImGui::SameLine();
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
  ImGui::SameLine(0.0f, 20.0f);

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
  float morphTime = presetHandler->getMorphTime();
  if (ImGui::InputFloat("Morph Time", &morphTime, 0.0f, 20.0f)) {
    presetHandler->setMorphTime(morphTime);
  }
  ImGui::PopItemWidth();
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
    // std::vector<std::string> mapList = presetHandler->availablePresetMaps(); // before opt
    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
    if (ImGui::BeginCombo("Preset Map", state.currentBank.data())) {
      // stateMap[presetHandler].mapList = presetHandler->availablePresetMaps(); //before opt
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
      ImGui::PopItemWidth();
      ImGui::SameLine();
      if (ImGui::Button("Create")) {
        auto path = File::conformDirectory(presetHandler->getCurrentPath()) + state.newMapText +
                    ".presetMap";
        // Create an empty file
        std::ofstream file;
        file.open(path, std::ios::out);
        file.close();
        state.newMap = false;
        stateMap[presetHandler].mapList = presetHandler->availablePresetMaps(); // optimize
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
  if (mIsLinkingParamAndMIDI)
    mIsLinkingParamAndMIDI = false;
  return true;
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

    if (config.find(consts::SOUND_OUTPUT_PATH_KEY) == config.end())
      config[consts::SOUND_OUTPUT_PATH_KEY] =
          al::File::conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    if (config.find(consts::SAMPLE_RATE_KEY) == config.end())
      config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

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

  } else {
    config[consts::MIDI_PRESET_NAMES_KEY] = json::array();

    config[consts::SOUND_OUTPUT_PATH_KEY] =
        al::File::conformPathToOS(userPath + consts::DEFAULT_SOUND_OUTPUT_PATH);

    config[consts::SAMPLE_RATE_KEY] = consts::SAMPLE_RATE;

    config[consts::LIGHT_MODE_KEY] = consts::LIGHT_MODE;

    config[consts::FONT_SCALE_KEY] = consts::FONT_SCALE;

    config[consts::WINDOW_WIDTH_KEY] = consts::WINDOW_WIDTH;

    config[consts::WINDOW_HEIGHT_KEY] = consts::WINDOW_HEIGHT;

    config[consts::FULLSCREEN_KEY] = consts::FULLSCREEN;

    config[consts::IS_FIRST_LAUNCH_KEY] = consts::IS_FIRST_LAUNCH;

    config[consts::DEFAULT_AUDIO_DEVICE_KEY] = consts::DEFAULT_AUDIO_DEVICE;
  }

  std::ofstream file((userPath + configFile).c_str());
  if (file.is_open())
    file << config;

  return false;
}

template <typename T> bool ecInterface::jsonWriteToConfig(T value, std::string key) {
  json config;

  std::ifstream ifs(userPath + configFile);

  if (ifs.is_open())
    config = json::parse(ifs);

  config[key] = value;

  std::ofstream file((userPath + configFile).c_str());

  if (file.is_open()) {
    file << config;
    return true;
  } else {
    return false;
  }
}

bool ecInterface::jsonWriteMIDIPresetNames(std::unordered_set<std::string> &presetNames) {
  json config;

  std::ifstream ifs(userPath + configFile);
  if (ifs.is_open())
    config = json::parse(ifs);

  json preset_names = json::array();
  for (auto iter = presetNames.begin(); iter != presetNames.end(); iter++) {
    preset_names.push_back(*iter);
  }
  config[consts::MIDI_PRESET_NAMES_KEY] = preset_names;

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

  if (ifs.is_open())
    config = json::parse(ifs);

  return config;
}

void ecInterface::setMIDIPresetNames(json preset_names) {
  for (auto iter = preset_names.begin(); iter != preset_names.end(); iter++) {
    MIDIPresetNames.insert(iter->get<std::string>());
  }
}

void ecInterface::setSoundOutputPath(std::string sound_output_path) {
  soundOutput = al::File::conformPathToOS(sound_output_path);
}

void ecInterface::setAudioSettings(float sample_rate) {
  globalSamplingRate = sample_rate;

  configureAudio(globalSamplingRate, consts::BLOCK_SIZE, consts::AUDIO_OUTS, consts::DEVICE_NUM);
  granulator.setGlobalSamplingRate(sample_rate);
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
  if (name == "")
    return;

  std::string filename = name;
  if (!allowOverwrite) {
    int counter = 1;
    while (File::exists(userPath + midiPresetsPath + filename + ".json") && counter < 9999) {
      filename = name + "_" + std::to_string(counter++);
    }
  }

  MIDIPresetNames.insert(filename);
  jsonWriteMIDIPresetNames(MIDIPresetNames);
  json midi_config = json::array();

  json temp;
  for (int index = 0; index < ActiveMIDI.size(); index++) {
    ActiveMIDI[index].toJSON(temp);
    midi_config.push_back(temp);
  }

  std::ofstream file((userPath + midiPresetsPath + filename + ".json").c_str());
  if (file.is_open())
    file << midi_config;
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
  jsonWriteMIDIPresetNames(MIDIPresetNames);
  std::remove((userPath + midiPresetsPath + midi_preset_name + ".json").c_str());
}
