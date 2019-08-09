// Example of how to use DynamicScene
//
// A DynamicScene manages the insertion and removal of PositionedVoice nodes in
// its rendering graph. A DynamicScene has three rendering contexts: The
// update() or simulation context, where state and internal changes should be
// computed, and the onProcess() contexts for audio and graphics.
//
// By: Andres Cabrera January 2019
//

#include "Gamma/Oscillator.h"
#include "Gamma/Envelope.h"

#include "al/util/scene/al_DynamicScene.hpp"
#include "al/core/app/al_App.hpp"
#include "al/core/graphics/al_Shapes.hpp"

using namespace gam;
using namespace al;

// A DynamicScene manages the insertion and removal of
// PositionedVoice nodes in its rendering graph.
// A DynamicScene has three rendering contexts:
// The update() or simulation context, where state and
// internal changes should be computed, and the onProcess()
// contexts for audio and graphics.

// The DynamicScene will contain "SimpleVoice" agents that
// inherit from PositionedVoice
struct SimpleVoice : public PositionedVoice {

    Parameter mFreq {"Freq"};

    Sine<> mOsc;
    AD<> mAmpEnv {2.0f, 2.0f};

    Mesh mMesh;
    SimpleVoice()
    {
        addTorus(mMesh);
        mMesh.primitive(Mesh::LINE_STRIP);

        // Register parameters using the stream operator
        // parameters registered this way can be set through
        // the setTriggerParams(), and will also allow their
        // values to be stored when using SynthSequencer
        *this << mFreq;

        // The Freq parameter will drive changes in the
        // internal oscillator
        mFreq.registerChangeCallback([this](float value) {
            mOsc.freq(value);
        });
    }

    // The update function will change the position of the agent
    // and decrease the oscillator frequency.
    // It is called before onProcess(AudioIOData& io) and
    // onProcess(Graphics &g), but it can also be run threaded
    // if the DynamicScene is configured to do that.
    // It is called below inside the onAnimate function,
    // i.e. once every frame
    virtual void update(double dt) override {
        mFreq = mFreq * 0.995f;
        auto p = pose();
        p.vec().y = mAmpEnv.value()*3;
        p.vec().x = mFreq/440.0;
        setPose(p);
    }

    // Write your audio code inside this function
    // A Positioned voice is provided its own audio buffer,
    // so overwriting is not a problem. But you must make
    // sure to call setVoiceMaxOutputChannels() if your
    // voice produces more than two channels of output.
    virtual void onProcess(AudioIOData& io) override {
        while(io()){
            io.out(0) = mOsc() * mAmpEnv() * 0.05f;
        }
        if(mAmpEnv.done()) { free();}
    }

    // Write your draw code here. You do not need to do the translation and
    // scaling, as this will be done for you. Only use translations if you
    // want to translate relative to the agent's position.
    virtual void onProcess(Graphics &g) override {
        HSV c;
        c.h = mAmpEnv.value();
        g.color(Color(c));
        g.draw(mMesh);
    }

    // This function will be called every time the agent is inserted in the
    // DynamicScene. Since the agent's memory is recycled, use this function
    // to initialize and reset data
    virtual void onTriggerOn() override {
        Pose p = pose();
        p.vec() = {mFreq/440.0 , 0.0, -10.0};
        setPose(p);
        mAmpEnv.reset();
    }
};


// make an app that contains a SynthSequencer class
// use the render() functions from the SynthSequencer to produce audio and
// graphics in the corresponding callback
struct MyApp : public App
{
    // The number passed to the construtor indicates how many threads are used to compute the scene
    DynamicScene scene {8};

    virtual void onCreate() override {

        scene.showWorldMarker(false);
        scene.registerSynthClass<SimpleVoice>();
        // Preallocate 300 voices
        scene.allocatePolyphony("SimpleVoice", 300);
        scene.prepare(audioIO());
    }

    virtual void onAnimate(double dt) override {
        static double timeAccum = 0.1;
        timeAccum += dt;
        if (timeAccum > 0.1) {
        // Trigger one new voice every 0.05 seconds
            // First get a free voice of type SimpleVoice
            auto *freeVoice = scene.getVoice<SimpleVoice>();
            // Then set its parameters (this voice only has one parameter Freq)
            auto params = std::vector<float>{880.0f};
            freeVoice->setTriggerParams(params);
            // Set a position for it
            // Trigger it (this inserts it into the chain)
            scene.triggerOn(freeVoice);
            timeAccum -= 0.1;
        }

        scene.update(dt); // Update all nodes in the scene
    }

    virtual void onSound(AudioIOData &io) override {
        scene.render(io);
    }

    virtual void onDraw(Graphics &g) override {
        g.clear();
        scene.render(g);
    }
};

int main(){
    MyApp app;
    // tell Gamma the sample rate
    app.initAudio(44100., 512, 2,2);
    Domain::master().spu(app.audioIO().framesPerSecond());
    app.start();
}