//ecInterface.cpp 

/**** Emission Control LIB ****/
#include "ecInterface.h"

/**** AlloLib LIB ****/
#include "al_ext/soundfile/al_SoundfileRecordGUI.hpp"


using namespace al;

/**** ecInterface Implementation ****/

void ecInterface::onInit() {

  audioIO().append(mRecorder);
  granulator.init();
  granulator.loadSoundFile("voicePop.wav");
  
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
    << *granulator.streams.mParameterInt 
    << granulator.streamsLFO << *granulator.modStreamsWidth.mParameter
    << *granulator.grainDurationMs.mParameter << *granulator.grainDurationMs.mLowRange << *granulator.grainDurationMs.mHighRange
    << granulator.grainDurationLFO << *granulator.modGrainDurationWidth.mParameter
    << *granulator.envelope.mParameter << *granulator.envelope.mLowRange << *granulator.envelope.mHighRange
    << granulator.envelopeLFO << *granulator.modEnvelopeWidth.mParameter
    << *granulator.tapeHead.mParameter << *granulator.tapeHead.mLowRange << *granulator.tapeHead.mHighRange
    << granulator.tapeHeadLFO << *granulator.modTapeHeadWidth.mParameter
    << *granulator.playbackRate.mParameter << *granulator.playbackRate.mLowRange << *granulator.playbackRate.mHighRange
    << granulator.playbackRateLFO << *granulator.modPlaybackRateWidth.mParameter
    << *granulator.volumeDB.mParameter << *granulator.volumeDB.mLowRange << *granulator.volumeDB.mHighRange
    << granulator.volumeLFO << *granulator.modVolumeWidth.mParameter
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
  granulator.asynchronicity.mParameter->displayName("##asynchronicity");
  granulator.asynchronicity.mLowRange->displayName("##asynchronicityLow");
  granulator.asynchronicity.mHighRange->displayName("##asynchronicityHigh");
  granulator.asyncLFO.displayName("##asyncLFO");
  granulator.intermittency.mParameter->displayName("##intermittency");
  granulator.intermittency.mLowRange->displayName("##intermittencyLow");
  granulator.intermittency.mHighRange->displayName("##intermittencyHigh");
  granulator.intermittencyLFO.displayName("##intermittencyLFO");
  granulator.streams.mParameterInt->displayName("##streams");
  granulator.streamsLFO.displayName("##streamsLFO");
  granulator.grainDurationMs.mParameter->displayName("##grainDurationMs");
  granulator.grainDurationMs.mLowRange->displayName("##grainDurationMsLow");
  granulator.grainDurationMs.mHighRange->displayName("##grainDurationMsHigh");
  granulator.grainDurationLFO.displayName("##grainDurationMs");
  granulator.envelope.mParameter->displayName("##envelope");
  granulator.envelope.mLowRange->displayName("##envelopeLow");
  granulator.envelope.mHighRange->displayName("##envelopeHigh");
  granulator.envelopeLFO.displayName("##envelopeLFO");
  granulator.tapeHead.mParameter->displayName("##tapeHead");
  granulator.tapeHead.mLowRange->displayName("##tapeHeadLow");
  granulator.tapeHead.mHighRange->displayName("##tapeHeadHigh");
  granulator.tapeHeadLFO.displayName("##tapeHeadLFO");
  granulator.playbackRate.mParameter->displayName("##playbackRate");
  granulator.playbackRate.mLowRange->displayName("##playbackRateLow");
  granulator.playbackRate.mHighRange->displayName("##playbackRateHigh");
  granulator.playbackRateLFO.displayName("##playbackRate");
  granulator.volumeDB.mParameter->displayName("##volumeDB"); 
  granulator.volumeDB.mLowRange->displayName("##volumeDBLow");
  granulator.volumeDB.mHighRange->displayName("##volumeDBHigh");
  granulator.volumeLFO.displayName("##volumeLFO");
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

  ParameterGUI::beginPanel("Recorder",950,25);
  SoundFileRecordGUI::drawRecorderWidget(&mRecorder, audioIO().framesPerSecond(), audioIO().channelsOut());
  ParameterGUI::endPanel();

  ParameterGUI::beginPanel("LFO Controls", 25, 25,600);

  granulator.modSineFrequency.draw();
  granulator.modSinePhase.draw();
  granulator.modSquareFrequency.draw();
  granulator.modSquareWidth.draw();
  granulator.modSawFrequency.draw();
  granulator.modSawWidth.draw();
  
  ParameterGUI::endPanel();

  ParameterGUI::beginPanel("Granulator Controls", 675, 250,700,-1);
  granulator.grainRate.draw();
  granulator.asynchronicity.draw();
  granulator.intermittency.draw();
  ParameterGUI::drawParameterInt(granulator.streams.mParameterInt,"");
  granulator.grainDurationMs.draw();
  granulator.envelope.draw();
  granulator.tapeHead.draw();
  granulator.playbackRate.draw();
  granulator.volumeDB.draw();

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

  ParameterGUI::beginPanel("Presets", 625, 25);
  ParameterGUI::drawPresetHandler(&mPresets,12,4);
  ParameterGUI::endPanel();

  ImGui::Text("Number of Active Grains: %.1i ",granulator.getActiveVoices() );
  
  al::imguiEndFrame();

  al::imguiDraw();
}
