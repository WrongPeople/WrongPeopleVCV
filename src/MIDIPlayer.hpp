#pragma once

#include "plugin.hpp"
#include "osdialog.h"
#include "MidiFile.h"

// Reading MIDI files using Midifile library (https://github.com/craigsapp/midifile)
// MIDI messages processing is based on Core MIDI-CV code (https://github.com/VCVRack/Rack/blob/v1/src/core/MIDI_CV.cpp)


struct MIDIPlayer : Module {

    static const int TRACKS = 6;

    enum ParamIds {
        PLAY_PARAM,
        STOP_PARAM,
        LOOP_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        PLAY_INPUT,
        STOP_INPUT,
        NUM_INPUTS
    };

    enum OutputIds {
        PLAY_OUTPUT,
        STOP_OUTPUT,
        ENUMS(CV_OUTPUTS, TRACKS),
        ENUMS(GATE_OUTPUTS, TRACKS),
        ENUMS(VELOCITY_OUTPUTS, TRACKS),
        ENUMS(RETRIGGER_OUTPUTS, TRACKS),
        NUM_OUTPUTS
    };

    enum LightIds {
        PLAY_LIGHT,
        LOOP_LIGHT,
        ENUMS(TRACK_LIGHTS, TRACKS),
        NUM_LIGHTS
    };

    bool fileLoaded = false;
    int trackCount = 0;
    std::string filePath = "";
    std::string fileName = "";
    float fileDuration = 0.0;
    std::string fileDurationStr = "";
    smf::MidiFile midiFile;

    int track = 0;
    int channel = -1;

    bool playing = false;
    float playingTime = 0.0;
    long playingEvent = 0;

    bool loop = false;

    dsp::SchmittTrigger playTrigger;
    dsp::SchmittTrigger stopTrigger;
    dsp::SchmittTrigger loopTrigger;

    dsp::PulseGenerator playPulse;
    dsp::PulseGenerator stopPulse;

    int channels;
    enum PolyMode {
        ROTATE_MODE,
        REUSE_MODE,
        RESET_MODE,
        NUM_POLY_MODES
    };
    PolyMode polyMode;

    bool pedal[TRACKS];
    int rotateIndex[TRACKS];

    uint8_t notes[TRACKS][16] = {0};
    bool gates[TRACKS][16] = {false};
    uint8_t velocities[TRACKS][16] = {0};
    dsp::PulseGenerator retriggerPulses[TRACKS][16];
    std::vector<uint8_t> heldNotes;

    MIDIPlayer() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(PLAY_PARAM, 0.0, 1.0, 0.0, "Play");
        configParam(STOP_PARAM, 0.0, 1.0, 0.0, "Stop");
        configParam(LOOP_PARAM, 0.0, 1.0, 0.0, "Loop");
        heldNotes.reserve(128);
        onReset();
    }

    void loadFile();
    void setTrackCount(int trackCount);
    void setLoop(bool loop);
    void play();
    void stop();
    void rewind();

    void processMessage(int track, smf::MidiMessage *msg);
    void processCC(int track, smf::MidiMessage *msg);
    void pressNote(int track, uint8_t note, int channel);
    void releaseNote(int track, uint8_t note);
    void pressPedal(int track);
    void releasePedal(int track);
    int assignChannel(int track, uint8_t note);
    void panic();

    void setChannels(int channels);
    void setPolyMode(PolyMode polyMode);

    void onReset() override;
    json_t * dataToJson() override;
    void dataFromJson(json_t *rootJ) override;

    void process(const ProcessArgs &args) override;

};
