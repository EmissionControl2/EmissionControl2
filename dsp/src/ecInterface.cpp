//ecInterface.cpp 

/**** Emission Control LIB ****/
#include "ecInterface.h"
#include "utility.h"

/**** AlloLib LIB ****/
#include "al/io/al_File.hpp"


using namespace al;

/**** ecInterface Implementation ****/

void ecInterface::onInit() {
	granulator.init(&audioIO());

	std::string execPath = util::getExecutablePath();
	File f(execPath);
	//Set output directory for presets.
	mPresets.setRootPath(f.directory(execPath) + "presets/");
	//Set output directory of recorded files.
	soundOutput = f.directory(execPath) + "soundOutput/";

	audioIO().append(mRecorder);

	// Load in all files in {ExecutableLocation}/samples/
	initialDirectory = granulator.loadInitSoundFiles();
}

void ecInterface::onCreate() {

	al::imguiInit(); 
	mPresets 
		<< *granulator.grainRate.mParameter << *granulator.grainRate.mLowRange << *granulator.grainRate.mHighRange
		<< granulator.grainRateLFO << *granulator.modGrainRateWidth.mParameter
		<< *granulator.asynchronicity.mParameter << *granulator.asynchronicity.mLowRange << *granulator.asynchronicity.mHighRange
		<< granulator.asyncLFO << *granulator.modAsynchronicityWidth.mParameter
		<< *granulator.intermittency.mParameter << *granulator.intermittency.mLowRange << *granulator.intermittency.mHighRange
		<< granulator.intermittencyLFO << *granulator.modIntermittencyWidth.mParameter
		<< *granulator.streams.mParameterInt << *granulator.streams.mLowRange << *granulator.streams.mHighRange
		<< granulator.streamsLFO << *granulator.modStreamsWidth.mParameter
		<< *granulator.grainDurationMs.mParameter << *granulator.grainDurationMs.mLowRange << *granulator.grainDurationMs.mHighRange
		<< granulator.grainDurationLFO << *granulator.modGrainDurationWidth.mParameter
		<< *granulator.envelope.mParameter << *granulator.envelope.mLowRange << *granulator.envelope.mHighRange
		<< granulator.envelopeLFO << *granulator.modEnvelopeWidth.mParameter
		<< *granulator.tapeHead.mParameter << *granulator.tapeHead.mLowRange << *granulator.tapeHead.mHighRange
		<< granulator.tapeHeadLFO << *granulator.modTapeHeadWidth.mParameter
		<< *granulator.transposition.mParameter << *granulator.transposition.mLowRange << *granulator.transposition.mHighRange
		<< granulator.transpositionLFO << *granulator.modTranspositionWidth.mParameter
		<< *granulator.volumeDB.mParameter << *granulator.volumeDB.mLowRange << *granulator.volumeDB.mHighRange
		<< granulator.volumeLFO << *granulator.modVolumeWidth.mParameter
		<< *granulator.pan.mParameter << *granulator.pan.mLowRange << *granulator.pan.mHighRange
		<< granulator.panLFO << *granulator.modPanWidth.mParameter
		<< *granulator.soundFile.mParameterInt << *granulator.soundFile.mLowRange << *granulator.soundFile.mHighRange
		<< granulator.soundFileLFO << *granulator.modSoundFileWidth.mParameter
		<< *granulator.modSineFrequency.mParameter << *granulator.modSineFrequency.mLowRange 
		<< *granulator.modSineFrequency.mHighRange
		<< *granulator.modSinePhase.mParameter << *granulator.modSinePhase.mLowRange << *granulator.modSinePhase.mHighRange
		<< *granulator.modSquareFrequency.mParameter << *granulator.modSquareFrequency.mLowRange 
		<< *granulator.modSquareFrequency.mHighRange
		<< *granulator.modSquareWidth.mParameter << *granulator.modSquareWidth.mLowRange << *granulator.modSquareWidth.mHighRange
		<< *granulator.modSawFrequency.mParameter << *granulator.modSawFrequency.mLowRange << *granulator.modSawFrequency.mHighRange
		<< *granulator.modSawWidth.mParameter << *granulator.modSawWidth.mLowRange << *granulator.modSawWidth.mHighRange;

	granulator.grainRate.mParameter->displayName("##grainRate");
	granulator.grainRate.mLowRange->displayName("##grainRateLow");
	granulator.grainRate.mHighRange->displayName("##grainRateHigh");
	granulator.grainRateLFO.displayName("##grainRateLFO");
	granulator.modGrainRateWidth.mParameter->displayName("##modGrainRateWidth");
	granulator.asynchronicity.mParameter->displayName("##asynchronicity");
	granulator.asynchronicity.mLowRange->displayName("##asynchronicityLow");
	granulator.asynchronicity.mHighRange->displayName("##asynchronicityHigh");
	granulator.asyncLFO.displayName("##asyncLFO");
	granulator.modAsynchronicityWidth.mParameter->displayName("##modAsynchronicityWidth");
	granulator.intermittency.mParameter->displayName("##intermittency");
	granulator.intermittency.mLowRange->displayName("##intermittencyLow");
	granulator.intermittency.mHighRange->displayName("##intermittencyHigh");
	granulator.intermittencyLFO.displayName("##intermittencyLFO");
	granulator.modIntermittencyWidth.mParameter->displayName("##modIntermittencyWidth");
	granulator.streams.mParameterInt->displayName("##streams");
	granulator.streams.mLowRange->displayName("##streamsLow");
	granulator.streams.mHighRange->displayName("##streamsHigh");
	granulator.streamsLFO.displayName("##streamsLFO");
	granulator.modStreamsWidth.mParameter->displayName("##modStreamsWidth");
	granulator.grainDurationMs.mParameter->displayName("##grainDurationMs");
	granulator.grainDurationMs.mLowRange->displayName("##grainDurationMsLow");
	granulator.grainDurationMs.mHighRange->displayName("##grainDurationMsHigh");
	granulator.grainDurationLFO.displayName("##grainDurationMs");
	granulator.modGrainDurationWidth.mParameter->displayName("##modGrainDurationWidth");
	granulator.envelope.mParameter->displayName("##envelope");
	granulator.envelope.mLowRange->displayName("##envelopeLow");
	granulator.envelope.mHighRange->displayName("##envelopeHigh");
	granulator.envelopeLFO.displayName("##envelopeLFO");
	granulator.modEnvelopeWidth.mParameter->displayName("##modEnvelopeWidth");
	granulator.tapeHead.mParameter->displayName("##tapeHead");
	granulator.tapeHead.mLowRange->displayName("##tapeHeadLow");
	granulator.tapeHead.mHighRange->displayName("##tapeHeadHigh");
	granulator.tapeHeadLFO.displayName("##tapeHeadLFO");
	granulator.modTapeHeadWidth.mParameter->displayName("##modTapeHeadWidth");
	granulator.transposition.mParameter->displayName("##transposition");
	granulator.transposition.mLowRange->displayName("##playbackRateLow");
	granulator.transposition.mHighRange->displayName("##playbackRateHigh");
	granulator.transpositionLFO.displayName("##transposition");
	granulator.modTranspositionWidth.mParameter->displayName("##modTranspositionWidth");
	granulator.volumeDB.mParameter->displayName("##volumeDB"); 
	granulator.volumeDB.mLowRange->displayName("##volumeDBLow");
	granulator.volumeDB.mHighRange->displayName("##volumeDBHigh");
	granulator.volumeLFO.displayName("##volumeLFO");
	granulator.modVolumeWidth.mParameter->displayName("##modVolumeWidth");
	granulator.pan.mParameter->displayName("##pan"); 
	granulator.pan.mLowRange->displayName("##PanLow");
	granulator.pan.mHighRange->displayName("##PanHigh");
	granulator.panLFO.displayName("##PanLFO");
	granulator.modPanWidth.mParameter->displayName("##modPanWidth");
	granulator.soundFile.mParameterInt->displayName("##soundFile"); 
	granulator.soundFile.mLowRange->displayName("##soundFileLow");
	granulator.soundFile.mHighRange->displayName("##soundFileHigh");
	granulator.soundFileLFO.displayName("##soundFileLFO");
	granulator.modSoundFileWidth.mParameter->displayName("##modSoundFileWidth");

	granulator.modSineFrequency.mParameter->displayName("##modSineFrequency");
	granulator.modSineFrequency.mLowRange->displayName("##modSineFrequencyLow");
	granulator.modSineFrequency.mHighRange->displayName("##modSineFrequencyHigh");
	granulator.modSinePhase.mParameter->displayName("##modSinePhase");
	granulator.modSinePhase.mLowRange->displayName("##modSinePhaseLow");
	granulator.modSinePhase.mHighRange->displayName("##modSinePhaseHigh");
	granulator.modSquareFrequency.mParameter->displayName("##modSquareFrequency");
	granulator.modSquareFrequency.mLowRange->displayName("##modSquareFrequencyLow");
	granulator.modSquareFrequency.mHighRange->displayName("##modSquareFrequencyHigh");
	granulator.modSquareWidth.mParameter->displayName("##modSquareWidth");
	granulator.modSquareWidth.mLowRange->displayName("##modSquareWidthLow");
	granulator.modSquareWidth.mHighRange->displayName("##modSquareWidthHigh");
	granulator.modSawFrequency.mParameter->displayName("##modSawFrequency");
	granulator.modSawFrequency.mLowRange->displayName("##modSawFrequencyLow");
	granulator.modSawFrequency.mHighRange->displayName("##modSawFrequencyHigh");
	granulator.modSawWidth.mParameter->displayName("##modSawWidth");
	granulator.modSawWidth.mLowRange->displayName("##modSawWidthLow");
	granulator.modSawWidth.mHighRange->displayName("##modSawWidthHigh");

}

void ecInterface::onSound(AudioIOData &io) {
	granulator.onProcess(io);
}

void ecInterface::onDraw(Graphics &g) {
	g.clear(background);
	al::imguiBeginFrame();

	//Draw GUI

	ParameterGUI::beginPanel("LFO Controls", 25, 25,600);

	granulator.modSineFrequency.drawRangeSlider();
	granulator.modSinePhase.drawRangeSlider();
	granulator.modSquareFrequency.drawRangeSlider();
	granulator.modSquareWidth.drawRangeSlider();
	granulator.modSawFrequency.drawRangeSlider();
	granulator.modSawWidth.drawRangeSlider();
	
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Granulator Controls", 675, 250,700,-1);
	granulator.grainRate.drawRangeSlider();
	granulator.asynchronicity.drawRangeSlider();
	granulator.intermittency.drawRangeSlider();
	granulator.streams.drawRangeSlider();
	granulator.grainDurationMs.drawRangeSlider();
	granulator.envelope.drawRangeSlider();
	granulator.tapeHead.drawRangeSlider();
	granulator.transposition.drawRangeSlider();
	granulator.volumeDB.drawRangeSlider();
	granulator.pan.drawRangeSlider();
	granulator.soundFile.drawRangeSlider();

	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Modulation Wave", 525, 250, 150, -1);
	ParameterGUI::drawMenu(&granulator.grainRateLFO); 
	ParameterGUI::drawMenu(&granulator.asyncLFO);
	ParameterGUI::drawMenu(&granulator.intermittencyLFO);
	ParameterGUI::drawMenu(&granulator.streamsLFO);
	ParameterGUI::drawMenu(&granulator.grainDurationLFO);
	ParameterGUI::drawMenu(&granulator.envelopeLFO);
	ParameterGUI::drawMenu(&granulator.tapeHeadLFO);
	ParameterGUI::drawMenu(&granulator.transpositionLFO);
	ParameterGUI::drawMenu(&granulator.volumeLFO);
	ParameterGUI::drawMenu(&granulator.panLFO);
	ParameterGUI::drawMenu(&granulator.soundFileLFO);
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Modulation Depth", 25, 250, 500, -1);
	
	ParameterGUI::drawParameter(granulator.modGrainRateWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modAsynchronicityWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modIntermittencyWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modStreamsWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modGrainDurationWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modEnvelopeWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modTapeHeadWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modTranspositionWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modVolumeWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modPanWidth.mParameter);
	ParameterGUI::drawParameter(granulator.modSoundFileWidth.mParameter);
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("IO",1125,25,200,200);
	// Draw an interface to Audio IO.
	// This enables starting and stopping audio as well as selecting
	// Audio device and its parameters
	drawAudioIO(&audioIO()); 
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Recorder",950,25);
	drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut(),soundOutput);
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Presets", 625, 25,320,220);
	ParameterGUI::drawPresetHandler(&mPresets,12,4);
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("File Selector",25,535,-1,-1);

	ImGui::Text("%s", currentFile.c_str());
	if (ImGui::Button("Select File")) {
		// When the select file button is clicked, the file selector is shown
		selector.start(initialDirectory);
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
	//std::cout << currentFile << std::endl;
	if(currentFile != previousFile) {
		granulator.loadSoundFile(currentFile);
		previousFile = currentFile;
	}
	ParameterGUI::endPanel();

	ParameterGUI::beginPanel("Info", 525, 535,200,-1);
	ImGui::Text("Number of Active Grains: %.1i ",granulator.getActiveVoices() );
	ParameterGUI::endPanel();
	
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
	if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
		if (io->isOpen()) {
			std::string text;
			text += "Sampling Rate: " + std::to_string(io->fps());
			text += "\nbuffer size: " + std::to_string(io->framesPerBuffer());
			text += "\nin chnls: " + std::to_string(io->channelsIn());
			text += "\nout chnls:" + std::to_string(io->channelsOut());
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
			std::vector<std::string> samplingRates{"44100", "48000", "88100","96000"};
			ImGui::Combo("Sampling Rate", &state.currentSr,
									 ParameterGUI::vector_getter,
									 static_cast<void *>(&samplingRates), samplingRates.size());
			if (ImGui::Button("Start")) {
				io->framesPerSecond(std::stof(samplingRates[state.currentSr]));
				io->framesPerBuffer(1024);
				io->device(AudioDevice(state.currentDevice));
				granulator.setIO(io);
				granulator.resampleSoundFiles();
				io->open();
				io->start();
			}
		}
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
	if(stateMap.find(recorder) == stateMap.end()) {
			stateMap[recorder] = SoundfileRecorderState{0, false};
	}
	SoundfileRecorderState &state = stateMap[recorder];
	ImGui::PushID(std::to_string((unsigned long) recorder).c_str());
	if (ImGui::CollapsingHeader("Record Audio", ImGuiTreeNodeFlags_CollapsingHeader | ImGuiTreeNodeFlags_DefaultOpen)) {
			static char buf1[64] = "test.wav"; ImGui::InputText("Record Name", buf1, 63);
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
							while(File::exists(directory + filename) && counter < 9999) {
								filename = buf1;
								int lastDot = filename.find_last_of(".");
								filename = filename.substr(0, lastDot) + std::to_string(counter++) + filename.substr(lastDot);
							}
						}
						if (!recorder->start(directory + filename, frameRate, numChannels, ringBufferSize)) {
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
