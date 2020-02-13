//emissionControl.cpp

/**** Emission Control LIB ****/
#include "emissionControl.h"

/**** AlloLib LIB ****/
#include "al/io/al_Imgui.hpp"

/**** CSTD LIB ****/
#include <iostream>

using namespace al;

/******* grainEnvelope *******/

void grainEnvelope::setSamplingRate(float samplingRate) {
	mSamplingRate = samplingRate;
	mExpoEnv.setSamplingRate(mSamplingRate);
	mTurkeyEnv.setSamplingRate(mSamplingRate);
	mRExpoEnv.setSamplingRate(mSamplingRate);

}

float grainEnvelope::operator()() {
	if(mEnvelope < 0 || mEnvelope > 1)  //exponential envelope case 
		mEnvelope = 0; 
	
	if (mEnvelope < 0.5) { //exponetial and turkey envelope interpolation
	mRExpoEnv.increment();
	return ((mExpoEnv() * (1 - mEnvelope*2)) + (mTurkeyEnv() * mEnvelope*2) );
	} else if (mEnvelope == 0.5) { //turkey envelope case 
	mRExpoEnv.increment();
	mExpoEnv.increment();
	return mTurkeyEnv();
	} else if (mEnvelope <= 1) { // turkey and reverse exponential envelope interpolation
	mExpoEnv.increment();
	return ((mTurkeyEnv() * (1 - (mEnvelope-0.5) * 2)) + (mRExpoEnv() * (mEnvelope - 0.5) * 2) );
	} else { //fails silently but gracefully
		mRExpoEnv.increment();
		mExpoEnv.increment();
		return mTurkeyEnv();
	}
} 

void grainEnvelope::reset() {
	mExpoEnv.set();
	mRExpoEnv.set();
	mTurkeyEnv.set();
}

void grainEnvelope::setDuration(float duration) {
	if (duration <= 0) {
		mDuration = 1;
	} else
		mDuration = duration;
	mTurkeyEnv.set(duration);
	mExpoEnv.set(duration, 0);
	mRExpoEnv.set(duration, 1);
}

void grainEnvelope::setEnvelope(float envelope) {
	if (envelope > 1)
		mEnvelope = 1;
	else if (envelope < 0)
		mEnvelope = 0;
	else
		mEnvelope = envelope;
}

bool grainEnvelope::done() { return mTurkeyEnv.done(); }


/******* ecModulator *******/

float ecModulator::operator()() {
	if (mModWaveform == consts::SINE) {
		return mLFO.cos();
	} else if (mModWaveform == consts::SAW) {
		return mLFO.up2();
	} else if (mModWaveform == consts::SQUARE) {
		return mLFO.stair();
	} else if (mModWaveform == consts::NOISE) {
		return rand.uniform(-1.0, 1.0);
	} else {
		return mLFO.cos();
	}
}

void ecModulator::setWaveform(consts::waveform modWaveform) {
	if (modWaveform != consts::SINE && modWaveform != consts::SAW &&
			modWaveform != consts::SQUARE && modWaveform != consts::NOISE) {
		std::cerr << "invalid waveform" << std::endl;
		return;
	}
	mModWaveform = modWaveform;
}

void ecModulator::setFrequency(float frequency) {
	mLFO.freq(frequency);
}

void ecModulator::setWidth(float width) {
		mWidth = width;
		mLFO.mod(width);
}

void ecModulator::setPhase(float phase) { 
	mLFO.phase(phase); 
}

/******* ecParameter *******/

ecParameter::ecParameter(std::string parameterName, float defaultValue,
						float defaultMin, float defaultMax ,
						float absMin, float absMax,
						consts::waveform modWaveform,
						bool independentMod) {
	mParameter = new Parameter{parameterName, defaultValue, defaultMin, defaultMax};
	mLowRange =
			new Parameter{(parameterName + "Low").c_str(), defaultMin, absMin, absMax};
	mHighRange =
			new Parameter{(parameterName + "High").c_str(), defaultMax, absMin, absMax};
	mMin = defaultMin;
	mMax = defaultMax;
	mModWaveform = modWaveform;
	mIndependentMod = independentMod;
	if (mIndependentMod)  // if true, this parameter will have its own modulator
		mModulator = new ecModulator{mModWaveform, 1, 1};
}


ecParameter::ecParameter(std::string parameterName, std::string Group,
						float defaultValue, std::string prefix,
						float defaultMin, float defaultMax,
						float absMin, float absMax,
						consts::waveform modWaveform,
						bool independentMod) {
	mParameter =
			new Parameter{parameterName, Group, defaultValue, prefix, defaultMin, defaultMax};
	mLowRange = new Parameter{
			(parameterName + "Low").c_str(), Group, defaultMin, prefix, absMin, absMax};
	mHighRange = new Parameter{
			(parameterName + "High").c_str(), Group, defaultMax, prefix, absMin, absMax};
	mMin = defaultMin;
	mMax = defaultMax;
	mModWaveform = modWaveform;
	mIndependentMod = independentMod;
	if (mIndependentMod)  // if true, this parameter will have its own modulator
		mModulator = new ecModulator{mModWaveform, 1, 1};
}

ecParameter::~ecParameter() {
		delete mParameter;
		delete mLowRange;
		delete mHighRange;
		if (mIndependentMod) delete mModulator;
}

void ecParameter::setWaveformIndex(int index) {
	switch (index) {
		case 0:
			mModWaveform = consts::SINE;
			break;
		case 1:
			mModWaveform = consts::SQUARE;
			break;
		case 2:
			mModWaveform = consts::SAW;
			break;
		case 3:
			mModWaveform = consts::NOISE;
			break;
		default:
			mModWaveform = consts::SINE;
	}
}

void ecParameter::setIndependentMod(bool independentMod) {
	mIndependentMod = independentMod;
	if (mIndependentMod && mModulator == nullptr)
		mModulator = new ecModulator{mModWaveform, 1, 1};
	else
		delete mModulator;
}

float ecParameter::getModParam(float modSineValue, float modSquareValue, float modSawValue,
													float modNoiseValue, float modWidth) {
	float temp;
	switch (mModWaveform) {
		case consts::SINE: {
			temp = mParameter->get() + (modSineValue * modWidth * (mHighRange->get() - mLowRange->get()) ); //WIIIPS
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		case consts::SQUARE:{
			temp = mParameter->get() + (modSquareValue * modWidth * (mHighRange->get() - mLowRange->get()) ); 
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		case consts::SAW: {
			temp = mParameter->get() + (modSawValue * modWidth * (mHighRange->get() - mLowRange->get()) ); 
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		case consts::NOISE: {
			temp = mParameter->get() + (modNoiseValue * modWidth * (mHighRange->get() - mLowRange->get()) ); 
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		default: {
			temp = mParameter->get() + (modSineValue * modWidth * (mHighRange->get() - mLowRange->get()) ); 
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}
	}
}

float ecParameter::getModParam(float modWidth) {
	if (!mIndependentMod) {
		std::cerr << "PARAMETER must have independence set to true if you want "
									"to use this getModParam function\n";
		return -9999999999;
	}
	float temp = mParameter->get() + ( (*mModulator)() * modWidth *  (mHighRange->get() - mLowRange->get())  );
	if(temp > mHighRange->get())
		return mHighRange->get();
	else if(temp < mLowRange->get())
		return mLowRange->get();
	else return temp;
}

void ecParameter::drawRangeSlider() {
	float valueSlider, valueLow, valueHigh;
	bool changed;
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	valueLow = mLowRange->get();
	changed = ImGui::DragFloat((mLowRange->displayName()).c_str(), &valueLow,
															0.1, mLowRange->min(), mLowRange->max());
	ImGui::SameLine();
	if (changed) mLowRange->set(valueLow);
	// if(valueLow > mHighRange->get()) mParameter->min(mMin);
	mParameter->min(valueLow);

	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
	valueSlider = mParameter->get();
	changed =
			ImGui::SliderFloat((mParameter->displayName()).c_str(), &valueSlider,
													mParameter->min(), mParameter->max());
	if (changed) mParameter->set(valueSlider);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	valueHigh = mHighRange->get();
	changed = ImGui::DragFloat((mHighRange->displayName()).c_str(), &valueHigh,
															0.1, mHighRange->min(), mHighRange->max());
	if (changed) mHighRange->set(valueHigh);
	mParameter->max(valueHigh);

	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	ImGui::Text((mParameter->getName()).c_str());
	ImGui::PopItemWidth();
}


/******* ecParameterInt *******/

ecParameterInt::ecParameterInt(std::string parameterName, std::string Group,
							int defaultValue, std::string prefix,
							int defaultMin, int defaultMax,
							int absMin, int absMax,
							consts::waveform modWaveform,
							bool independentMod) {
	mParameterInt =
			new ParameterInt{parameterName, Group, defaultValue, prefix, defaultMin, defaultMax};
	mLowRange = 
			new ParameterInt{(parameterName + "Low").c_str(), Group, defaultMin, prefix, absMin, absMax};
	mHighRange = 
			new ParameterInt{(parameterName + "High").c_str(), Group, defaultMax, prefix, absMin, absMax};
	mMin = defaultMin;
	mMax = defaultMax;
	mModWaveform = modWaveform;
	mIndependentMod = independentMod;
	if (mIndependentMod)  // if true, this parameter will have its own modulator
		mModulator = new ecModulator{mModWaveform, 1, 1};
}

ecParameterInt::~ecParameterInt() {
	delete mParameterInt;
	delete mLowRange;
	delete mHighRange;
	if (mIndependentMod) delete mModulator;
}

void ecParameterInt::setIndependentMod(bool independentMod) {
	mIndependentMod = independentMod;
	if (mIndependentMod && mModulator == nullptr)
		mModulator = new ecModulator{mModWaveform, 1, 1};
	else
		delete mModulator;
}

void ecParameterInt::setWaveformIndex(int index) {
	switch (index) {
		case 0:
			mModWaveform = consts::SINE;
			break;
		case 1:
			mModWaveform = consts::SQUARE;
			break;
		case 2:
			mModWaveform = consts::SAW;
			break;
		case 3:
			mModWaveform = consts::NOISE;
			break;
		default:
			mModWaveform = consts::SINE;
	}
}

int ecParameterInt::getModParam(float modSineValue, float modSquareValue, float modSawValue,
								float modNoiseValue, float modWidth) {
	int temp;
	switch (mModWaveform) {
		case consts::SINE: {
			temp = mParameterInt->get() + (modSineValue * modWidth * (mHighRange->get() - mLowRange->get()) ); //WIIIPS
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->min())
				return mLowRange->min();
			else return temp;
		}

		case consts::SQUARE:{
			temp = mParameterInt->get() + (modSquareValue * modWidth * (mHighRange->get() - mLowRange->get()) );
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		case consts::SAW: {
			temp = mParameterInt->get() + (modSawValue * modWidth * (mHighRange->get() - mLowRange->get()) );
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		case consts::NOISE: {
			temp = mParameterInt->get() + (modNoiseValue * modWidth * (mHighRange->get() - mLowRange->get()) );
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}

		default: {
			temp = mParameterInt->get() * ((modSineValue * modWidth) + 1);
			if(temp > mHighRange->get())
				return mHighRange->get();
			else if(temp < mLowRange->get())
				return mLowRange->get();
			else return temp;
		}
	}
}

int ecParameterInt::getModParam(float modWidth) {
	if (!mIndependentMod) {
		std::cerr << "PARAMETER must have independence set to true if you want "
									"to use this getModParam function\n";
		return -99999;
	}
	int temp = mParameterInt->get() + ( (*mModulator)() * modWidth *  (mHighRange->get() - mLowRange->get())  );
	if(temp > mHighRange->get())
		return mHighRange->get();
	else if(temp < mLowRange->get())
		return mLowRange->get();
	else return temp;
}

void ecParameterInt::drawRangeSlider() {
	int valueSlider, valueLow, valueHigh;
	bool changed;
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	valueLow = mLowRange->get();
	changed = ImGui::DragInt((mLowRange->displayName()).c_str(), &valueLow,
															0.1, mLowRange->min(), mLowRange->max());
	ImGui::SameLine();
	if (changed) mLowRange->set(valueLow);
	mParameterInt->min(valueLow);
	
	// if(valueLow > mHighRange->get()) mParameter->min(mMin);

	ImGui::PopItemWidth();
	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.6f);
	valueSlider = mParameterInt->get();
	changed =
			ImGui::SliderInt((mParameterInt->displayName()).c_str(), &valueSlider,
													mParameterInt->min(), mParameterInt->max());
	if (changed) mParameterInt->set(valueSlider);
	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.1f);
	valueHigh = mHighRange->get();
	changed = ImGui::DragInt((mHighRange->displayName()).c_str(), &valueHigh,
															0.1, mHighRange->min(), mHighRange->max());

	if (changed)  mHighRange->set(valueHigh);
	 
	mParameterInt->max(valueHigh);

	ImGui::PopItemWidth();

	ImGui::SameLine();
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.2f);
	ImGui::Text((mParameterInt->getName()).c_str());
	ImGui::PopItemWidth();
}

/******* Grain Class *******/

void Grain::init() {
	gEnv.reset();
	mBPF.type(gam::RESONANT);
}

void Grain::configureGrain(grainParameters& list, float samplingRate) {
	float startSample, endSample;

	mPActiveVoices = list.activeVoices;

	if (list.modGrainDurationWidth > 0)
		setDurationMs(list.grainDurationMs.getModParam(
				list.modSineVal, list.modSquareVal, list.modSawVal, list.modNoiseVal,
				list.modGrainDurationWidth));
	else
		setDurationMs(list.grainDurationMs.getParam());

	if (list.modEnvelopeWidth > 0)
		gEnv.set(mDurationMs / 1000,
							list.envelope.getModParam(list.modSineVal, list.modSquareVal,
																				list.modSawVal, list.modNoiseVal,
																				list.modEnvelopeWidth));
	else
		gEnv.set(mDurationMs / 1000, list.envelope.getParam());

	this->source = list.source;

	if (list.modTapeHeadWidth > 0) //NOTE: the tape head wraps around to the beginning of the buffer when it exceeds its buffer size.
		startSample = source->size *
									(list.tapeHead.getModParam(
											list.modSineVal, list.modSquareVal, list.modSawVal,
											list.modNoiseVal, list.modTapeHeadWidth));
	else
		startSample = source->size * list.tapeHead.getParam();

	if (list.modTranspositionWidth > 0)
		endSample = startSample + source->channels * (
					(mDurationMs / 1000) * samplingRate *
					abs(list.transposition.getModParam(
													list.modSineVal, list.modSquareVal, list.modSawVal,
													list.modNoiseVal, list.modTranspositionWidth)
					) );
	else
		endSample = startSample + source->channels * ( (mDurationMs / 1000) * samplingRate * abs(list.transposition.getParam()) );

	index.setSamplingRate(samplingRate); // Set sampling rate of line function moving through audio buffer.

	if (list.transposition.getParam() < 0)
		index.set(endSample, startSample, mDurationMs / 1000);
	else
		index.set(startSample, endSample, mDurationMs / 1000);


	// Store modulated volume value of grain IF it is being modulated.
	// list.volumeDB.setParam(powf(10,list.volumeDB.getParam()/20));
	if(list.modVolumeWidth > 0 ) 
		mAmp = list.volumeDB.getModParam(
								list.modSineVal, list.modSquareVal, list.modSawVal,
								list.modNoiseVal, list.modVolumeWidth);
	else 
		mAmp = list.volumeDB.getParam();

	// Convert volume from db to amplitude
	mAmp = powf(10,mAmp/20);

	// Store modulated pan value of grain IF it is being modulated.
	if(list.modPanWidth > 0 ) 
		mPan = list.pan.getModParam(
								list.modSineVal, list.modSquareVal, list.modSawVal,
								list.modNoiseVal, list.modPanWidth); 
	else 
		mPan = list.pan.getParam();

	mPan = std::sqrt(mPan + 1) * 0.5; //Normalize the pan parameter and set up for equal power using square root.
	/**Set sampling rate of envelope**/
	gEnv.setSamplingRate(samplingRate);
	mAmp = mAmp * powf(*mPActiveVoices + 1,-0.36787698193); //  1/e 

	// FILTERING SETUP

	float level = list.resonance.getModParam(
		list.modSineVal, list.modSquareVal, list.modSawVal,
		list.modNoiseVal, list.modPanWidth);
	level = (level + 0.001) * 80;
	mBPF.res(level);
	mBPF.level(0.25);
	mBPF.freq(list.filter.getModParam(
		list.modSineVal, list.modSquareVal, list.modSawVal,
		list.modNoiseVal, list.modPanWidth)
	);
}

void Grain::onProcess(al::AudioIOData& io) {
	while (io()) {
		envVal = gEnv();
		sourceIndex = index();
		
		if (sourceIndex > source->size) sourceIndex -= source->size;

		if(source->channels == 1) {
			currentSample = mBPF.nextBP(source->get(sourceIndex));
			io.out(0) += currentSample * envVal * (1-mPan) * mAmp;
			io.out(1) += currentSample * envVal * mPan * mAmp;
		}
		else if (source->channels == 2) {
			io.out(0) += mBPF.nextBP(source->get(sourceIndex))
						* envVal * (1-mPan) * mAmp;
			io.out(1) += mBPF.nextBP(source->get(sourceIndex + 1))
						* envVal * mPan * mAmp;
		}

		if (gEnv.done()) {
			*mPActiveVoices -= 1;  // This will remove a grain from the active list.
			free();
			break;
		}
	}
}

void Grain::onTriggerOn() {

}

/******* voiceScheduler *******/

void voiceScheduler::configure(double frequency, double async, double intermittence) {
	if (async > 1.0) {
		async = 1.0;
	} else if (async < 0.0) {
		async = 0.0;
	}
	if (intermittence > 1.0) {
		intermittence = 1.0;
	} else if (intermittence < 0.0) {
		intermittence = 0.0;
	}
	mAsync = async;
	mFrequency = frequency;
	mIntermittence = intermittence;
	mIncrement = mFrequency / mSamplingRate;
}

bool voiceScheduler::trigger() {
	if (mCounter >= 1.0) {
		mCounter -= 1.0;
		if (rand.uniform() < mIntermittence) return false;
		mCounter += rand.uniform(-mAsync, mAsync);
		mCounter += mIncrement;
		return true;
	}
	mCounter += mIncrement;
	return false;
}

void voiceScheduler::setPolyStream(consts::streamType type, int numStreams) {
	if (type == consts::synchronous) {
		setFrequency(static_cast<double>(mFrequency * numStreams));
	} else {
		std::cerr << "Not implemented yet, please try again later.\n";
	}
}


/******* flowControl *******/

bool flowControl::throttle(float time, float ratio, int activeVoices) {
	if (mCounter < time * consts::SAMPLE_RATE) {
		mCounter++;
		mAvgActiveVoices += activeVoices;
		return false;
	} else {
		mCounter++;
		mAvgActiveVoices /= mCounter;
		mCounter = 0;
	}

	float adaptThresh;

	if (getPeakCPU() > adaptThresh) {
		return true;
	}
	if (getAvgCPU() > adaptThresh) {
		return true;
	} else {
		return false;
	}
}
