/**
 * ecInterface.h
 *
 * AUTHOR: Jack Kilgore
 */

#ifndef ECINTERFACE_H
#define ECINTERFACE_H

/**** Emission Control LIB ****/
#include "ecSynth.h"

/**** AlloLib LIB ****/
#include "al/app/al_App.hpp"
#include "al/ui/al_FileSelector.hpp"
#include "al/ui/al_ParameterGUI.hpp"
#include "al/ui/al_PresetHandler.hpp"
#include "al_ext/soundfile/al_OutputRecorder.hpp"

class ecInterface : public al::App {
   public:
    /**
     * @brief Initilialize the synth interface.
     */
    virtual void onInit() override;

    /**
     * @brief Run once on starup.
     */
    virtual void onCreate() override;

    /**
     * @brief Audio rate processing of synth.
     */
    virtual void onSound(al::AudioIOData &io) override;

    /**
     * @brief Draw rate processing of synth interface.
     */
    virtual void onDraw(al::Graphics &g) override;

   private:
    float background = 0.21;
    ecSynth granulator;
    al::PresetHandler mPresets;
    al::OutputRecorder mRecorder;
    al::FileSelector selector;
    std::string soundOutput;
    std::string execPath;
    al::File f;
    std::string currentFile = "No file selected";
    std::string previousFile = "No file selected";
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
                             ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings;
    int framecounter = 0;
    std::vector<float> streamHistory = std::vector<float>(100, 0);
    int streamHistoryIndex = 0;
    int oscSize = 48000;

    void drawAudioIO(al::AudioIO *io);

    void drawLFOcontrol(ecSynth &synth, int lfoNumber);

    void drawModulationControl(al::ParameterMenu &menu, al::Parameter *slider);

    void setGUIColors();
};

/**
 * @brief Modified version of al's soundfilerecordGUI.
 *
 * @param[in] Output recorder object.
 *
 * @param[in] Path of directory where the outputted sound files will be stored.
 *
 * @param[in] Frame rate of outputted file.
 *
 * @param[in] Number of channels of outputted file.
 *
 * @param[in] Amount of space allocated for sound.
 */
static void drawRecorderWidget(al::OutputRecorder *recorder, double frameRate, uint32_t numChannels,
                               std::string directory = "", uint32_t bufferSize = 0);

#endif