#ifndef ECSYNTH_H
#define ECSYNTH_H

/**** Emission Control LIB ****/
#include "const.h"
#include "emissionControl.h"

/**** AlloLib LIB ****/
#include "al/ui/al_Parameter.hpp"
#include "al/scene/al_PolySynth.hpp"

class ecSynth: public al::SynthVoice {
public:

	/**
	 * PUBLIC PARAMETERS OF SYNTH
	 */
	ecParameter grainRate {"Grain_Rate", "", 1, "", 0.1, 100, 0, 500, consts::SINE, 0};
	al::ParameterMenu grainRateLFO {"grainRateLFO"}; 
	ecParameter modGrainRateWidth {"modGrainRateWidth", "", 0, "", 0, 1, 0, 1};
	ecParameter asynchronicity {"Asynchronicity", "", 0.0, "", 0.0, 1.0, 0, 1, consts::SINE};
	al::ParameterMenu asyncLFO {"asyncLFO"}; 
	ecParameter modAsynchronicityWidth {"modAsynchronicityWidth", "", 0, "", 0, 1, 0, 1};
	ecParameter intermittency {"Intermittency", "", 0,"", 0, 1, 0, 1};
	al::ParameterMenu intermittencyLFO {"intermittencyLFO"}; 
	ecParameter modIntermittencyWidth {"modIntermittencyWidth", "", 0, "", 0, 1, 0, 1};
	ecParameterInt streams {"Streams", "", 1,"", 1, 12, 1, 20};
	al::ParameterMenu streamsLFO {"streamsLFO"}; 
	ecParameter modStreamsWidth {"modStreamsWidth", "", 0, "", 0, 1, 0, 1};

	ecParameter grainDurationMs {"Grain_Dur_Ms", "", 250, "", 0.01, 1000, 0.0000001, 10000};
	al::ParameterMenu grainDurationLFO {"grainDurationLFO"}; 
	ecParameter modGrainDurationWidth {"modGrainDurationWidth", "", 0, "", 0, 1, 0, 1};
	ecParameter envelope {"Envelope", "", 0.5, "", 0, 1, 0, 1};
	al::ParameterMenu envelopeLFO {"envelopeLFO"}; 
	ecParameter modEnvelopeWidth {"modEnvelopeWidth", "", 0, "", 0, 1, 0, 1};
	ecParameter tapeHead{"Tape_Head", "", 0.5, "", 0, 1, 0, 1};
	al::ParameterMenu tapeHeadLFO {"tapeHeadLFO"}; 
	ecParameter modTapeHeadWidth {"modTapeHeadWidth", "", 0, "", 0, 1, 0, 1};
	ecParameter playbackRate {"Playback_Rate", "       ", 1, "", -2, 2, -20, 20};
	al::ParameterMenu playbackRateLFO {"playbackRateLFO"}; 
	ecParameter modPlaybackRateWidth {"modPlaybackRateWidth", "", 0, "", 0, 1, 0, 1};

	ecParameter volumeDB {"Volume_DB", "", -6, "", -60, 6, -180, 24};
	al::ParameterMenu volumeLFO {"volumeLFO"}; 
	ecParameter modVolumeWidth {"modVolumeWidth", "", 0, "", 0, 1, 0, 1};

	ecParameterInt soundFile {"Sound_File", "", 1, "", 1, mClipNum, 1, mClipNum};
	al::ParameterMenu soundFileLFO {"soundFileLFO"}; 
	ecParameter modSoundFileWidth {"modSoundFileWidth", "", 0, "", 0, 1, 0, 1};

	ecParameter modSineFrequency {"modSineFrequency", "",1, "", 0.01, 40, FLT_MIN, 1000};
	ecParameter modSinePhase {"modSinePhase", "", 0, "", 0, 1, 0, 1};
	ecParameter modSquareFrequency {"modSquareFrequency", "",1,"", 0.01, 40,FLT_MIN, 1000};
	ecParameter modSquareWidth {"modSquareWidth", "",0.5,"", 0, 1, 0, 1};
	ecParameter modSawFrequency {"modSawFrequency", "",1,"", 0.01, 40, FLT_MIN, 1000};
	ecParameter modSawWidth {"modSawWidth", "",1,"", 0, 1, 0, 1};

	/**
	 * @brief Initilialize the synth.
	 */
	virtual void init(al::AudioIOData* io);

	/**
	 * @brief Set data needed to perform granulation correctly.
	 */
	void setIO(al::AudioIOData* io);

	/**
	 * @brief Audio rate processing of synth.
	 */
	virtual void onProcess(al::AudioIOData& io) override;

	/**
	 * @brief Runs every time a voice is triggered.
	 */
	virtual void onTriggerOn() override;

	/**
	 * @brief Runs every time a voice is marked as inactive.
	 */
	virtual void onTriggerOff() override;

	/**
	 * @brief Load sound file into memory.
	 * 
	 * @param[in] The the filepath to the audio file.
	 */
	void loadSoundFile(std::string fileName);

	/**
	 * @brief Load sound files from designated sample folder.
	 * 
	 * @return Return the directory path where the files were loaded in from.
	 */
	std::string loadInitSoundFiles();

	/**
	 * @brief Resample all files in SoundClip to match globalSamplingRate.
	 */
	void resampleSoundFiles();

	/**
	 * @brief Clear sound files stored in memory.
	 * 
	 */
	void clearSoundFiles();

	/**
	 * @brief Print out what the synth is doing at runtime. 
	 *        Used for debugging purposes.
	 * 
	 * @param[in] Verbose toggled on if true, toggled off otherwise.
	 */
	void verbose(bool toggle) {
		grainSynth.verbose(toggle);
	}

	/**
	 * @brief Return the number of active voices. 
	 *        ie, The number of voices running in the audio callback loop.
	 * 
	 * @return The number of active voices.
	 */
	int getActiveVoices() {return mActiveVoices;}

	/**
	 * WORK IN PROGRESS
	 * @brief Throttles the amount of grains that can be active at once. TODO 
	 * 
	 * @param[in] The amount of time to the throttle the synth in seconds.
	 * @param[in] The ratio in which to throttle the synth. A number from 0 to 1.
	 */
	void throttle(float time, float ratio);

private:
	float mGlobalSamplingRate, mPrevSR;

	al::PolySynth grainSynth {al::PolySynth::TIME_MASTER_AUDIO}; /* Polyhpony and interface to audio callback */
	voiceScheduler grainScheduler; /* Schedule grains */
	std::vector<util::buffer<float>*> soundClip; /* Store audio buffers in memory */
	int mClipNum = 0; /* Number of sound files being stored in memory */
	int mModClip;

	int controlRateCounter = 0;
	int mActiveVoices = 0;
	int mCounter = 0;
	float mAvgActiveVoices = 0;
	float mPeakCPU;
	float mAvgCPU;
	int *mPActiveVoices = nullptr;
 
	/**
	 * Global Modulators for Synth
	 */
	ecModulator modSine {consts::SINE, 1, 1};
	ecModulator modSquare {consts::SQUARE};
	ecModulator modSaw {consts::SAW};
	ecModulator modNoise {consts::NOISE};

	float modSineValue, modSquareValue, modSawValue, modNoiseValue;
	
};

#endif
