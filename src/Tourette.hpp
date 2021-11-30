#pragma once

#include "plugin.hpp"

#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>
#include <random>

using namespace rack::dsp;


struct Tourette : Module {

    struct AvgBuf {

        unsigned int len;

        float sum = 0.0;
        float avg = 0.0;

        unsigned int pos = 0;
        std::vector<float> buf;

        void setLen(unsigned int len) {
            this->len = len;
            buf.resize(len);
            buf.assign(len, 0.0);
            sum = 0.f;
            avg = 0.f;
            pos = 0;
        }

        void step(float v) {
            v = std::fabs(v);

            buf[pos++] = v;

            if(pos >= len) {
                pos = 0;
            }

            sum += v - buf[pos];
            avg = sum / ((float) len / 1.f);
        }

    };

    struct Sample {
        float l;
        float r;
    };

    struct SampleBuf {
        Tourette *module;
        unsigned int id;

        bool full = false;
        bool recording = false;
        bool playing = false;
        unsigned int pos = 0;
        std::vector<Sample> buf;
        unsigned int played = 0;

        bool inputA;

        float attack = 0.0f;
        float release = 0.0f;

        void setFull(bool full) {
            this->full = full;
            module->lights[id * 3].setBrightness(full);
        }

        void startRecording(bool a) {
            inputA = a;
            playing = false;
            pos = 0;
            buf.clear();
            recording = true;
            module->lights[id * 3 + 2].setBrightness(1.f);
        }

        void stopRecording(bool full = false) {
            recording = false;
            this->setFull(full);
            played = 0;
            module->lights[id * 3 + 2].setBrightness(0.f);
        }

        void startPlaying(float attack, float release) {
            pos = 0;
            playing = true;
            this->attack = attack;
            this->release = release;
            module->lights[id * 3].setBrightness(0.f);
            module->lights[id * 3 + 1].setBrightness(1.f);
        }

        void stopPlaying() {
            playing = false;
            pos = 0;
            played++;
            module->lights[id * 3].setBrightness(full);
            module->lights[id * 3 + 1].setBrightness(0.f);
        }

        void record(float vl, float vr = 0) {
            buf.push_back({vl, vr});
        }

        Sample play() {
            if(pos < buf.size()) {

                float fpos = (float) pos / buf.size();
                Sample val = buf[pos++];

                if(attack > 0.0f && fpos < attack) {
                    val.l *= fpos / attack;
                    val.r *= fpos / attack;
                }

                if(release > 0.0f && fpos > 1.0 - release) {
                    val.l *= (1.0f - fpos) / release;
                    val.r *= (1.0f - fpos) / release;
                }

                return val;
            }
            else {
                stopPlaying();
                return {0, 0};
            }
        }

    };


    static const unsigned int BUFS_CNT = 16;


    enum ParamIds {
        THRESH_LO_PARAM,
        THRESH_HI_PARAM,
        MIN_LEN_PARAM,
        MAX_LEN_PARAM,
        REPEATS_PARAM,
        RAND_PARAM,
        ATTACK_PARAM,
        RELEASE_PARAM,
        POLY_PARAM,
        STEREO_PARAM,
        NUM_PARAMS
    };
    enum InputIds {
        SIG_A_INPUT,
        SIG_B_INPUT,
        PLAY_INPUT,
        NUM_INPUTS
    };
    enum OutputIds {
        SIG_A_OUTPUT,
        SIG_B_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds {
        ENUMS(BUF_LIGHTS, BUFS_CNT * 3),
        NUM_LIGHTS
    };

    bool stateA = false;
    bool stateB = false;

    int recordingA = -1;
    int recordingB = -1;

    bool fRecordA = false;
    bool fRecordB = false;

    AvgBuf avgBufA;
    AvgBuf avgBufB;

    float threshLoDb = 0.f;
    float threshHiDb = 0.f;
    float threshLo = 0.f;
    float threshHi = 0.f;

    bool rnd = false;

    dsp::SchmittTrigger playTrigger;

    SampleBuf bufs[BUFS_CNT];
    std::vector<unsigned int> bufsRange;
    std::default_random_engine rng = std::default_random_engine{};

    bool fPlay = false;

    unsigned int minBufSize = 0;
    unsigned int maxBufSize = 0;

    Tourette() {
		config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(THRESH_LO_PARAM, -60.f, 3.f, -42.f, "Low Threshold", " dB");
        configParam(THRESH_HI_PARAM, -54.f, 6.f, -18.f, "High Threshold", " dB");
        configParam(Tourette::MIN_LEN_PARAM, -0.3f, 0.3, -0.1f, "Min shot length", " s");
        configParam(Tourette::MAX_LEN_PARAM, -1.0f, 1.0, -0.3f, "Max shot length", " s");
        configParam(Tourette::REPEATS_PARAM, 1.0, 16.0, 8.0, "Repeats");
        configParam(Tourette::RAND_PARAM, 0.0, 1.0, 0.5, "Randomness");
        configParam(Tourette::ATTACK_PARAM, 0.0, 1.0, 0.0, "Attack", " s");
        configParam(Tourette::RELEASE_PARAM, 0.0, 1.0, 0.0, "Release", " s");
        configParam(Tourette::POLY_PARAM, 1.0, 8.0, 4.0, "Max polyphony");
        configSwitch(Tourette::STEREO_PARAM, 0.f, 1.f, 0.f, "Stereo mode", {"Split", "Stereo"});

        configInput(SIG_A_INPUT, "Signal A");
        configInput(SIG_B_INPUT, "Signal B");
        configInput(PLAY_INPUT, "Trigger");
        configOutput(SIG_A_OUTPUT, "Signal A");
        configOutput(SIG_B_OUTPUT, "Signal B");

        configBypass(SIG_A_INPUT, SIG_A_OUTPUT);
        configBypass(SIG_B_INPUT, SIG_B_OUTPUT);

        onSampleRateChange();

        srand(static_cast<unsigned>(time(0)));

        for(unsigned int i = 0; i < BUFS_CNT; i++) {
            bufs[i].module = this;
            bufs[i].id = i;
            bufsRange.push_back(i);
        }
    }

    void onSampleRateChange() override;

    void process(const ProcessArgs &args) override;

    void startRecordingA(int i);
    void startRecordingB(int i);
    void doneRecordingA();
    void doneRecordingB();
    void cancelRecordingA();
    void cancelRecordingB();
    void processInputsSplit();
    void processInputsStereo();
    void processBuffers();

};
