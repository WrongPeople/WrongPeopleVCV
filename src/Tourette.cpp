#include "Tourette.hpp"
#include "components/Tourette/TouretteDisplay.hpp"


void Tourette::startRecordingA(int i) {
    bufs[i].startRecording(true);
    recordingA = i;
}

void Tourette::startRecordingB(int i) {
    bufs[i].startRecording(false);
    recordingB = i;
}

void Tourette::doneRecordingA() {
    bufs[recordingA].stopRecording(true);
    recordingA = -1;
}

void Tourette::doneRecordingB() {
    bufs[recordingB].stopRecording(true);
    recordingB = -1;
}

void Tourette::cancelRecordingA() {
    bufs[recordingA].stopRecording(false);
    recordingA = -1;
}

void Tourette::cancelRecordingB() {
    bufs[recordingB].stopRecording(false);
    recordingB = -1;
}

void Tourette::processInputsSplit() {

    if(inputs[SIG_A_INPUT].isConnected()) {
        avgBufA.step(inputs[SIG_A_INPUT].getVoltage());
        if(avgBufA.avg >= threshHi) {
            if(!stateA) {
                stateA = true;
                if(recordingA == -1)
                    fRecordA = true;
            }
        }
        else if(avgBufA.avg < threshLo) {
            stateA = false;
            fRecordA = false;

            if(recordingA != -1) {
                if(bufs[recordingA].buf.size() < minBufSize) {
                    if(params[MIN_LEN_PARAM].getValue() < 0.0f) {
                        cancelRecordingA();
                    }
                }
                else {
                    doneRecordingA();
                }
            }
        }
    }

    if(inputs[SIG_B_INPUT].isConnected()) {
        avgBufB.step(inputs[SIG_B_INPUT].getVoltage());
        if(avgBufB.avg >= threshHi) {
            if(!stateB) {
                stateB = true;
                if(recordingB == -1)
                    fRecordB = true;
            }
        }
        else if(avgBufB.avg < threshLo) {
            stateB = false;
            fRecordB = false;

            if(recordingB != -1) {
                if(bufs[recordingB].buf.size() < minBufSize) {
                    if(params[MIN_LEN_PARAM].getValue() < 0.0f) {
                        cancelRecordingB();
                    }
                }
                else {
                    doneRecordingB();
                }
            }
        }
    }

}

void Tourette::processInputsStereo() {
    avgBufA.step(inputs[SIG_A_INPUT].getVoltage());
    avgBufB.step(inputs[SIG_B_INPUT].getVoltage());

    if(avgBufA.avg >= threshHi || avgBufB.avg >= threshHi) {
        if(!stateA) {
            stateA = true;
            if(recordingA == -1)
                fRecordA = true;
        }
    }
    else if(avgBufA.avg < threshLo && avgBufB.avg < threshLo) {
        stateA = false;
        fRecordA = false;

        if(recordingA != -1) {
            if(bufs[recordingA].buf.size() < minBufSize) {
                if(params[MIN_LEN_PARAM].getValue() < 0.f)
                    cancelRecordingA();
            }
            else {
                doneRecordingA();
            }
        }
    }
}

void Tourette::processBuffers() {
    unsigned int pc = 0;
    int b;
    Sample out;
    float outA = 0, outB = 0;
    for(unsigned int i = 0; i < BUFS_CNT; i++) {
        b = rnd ? bufsRange[i] : i;

        if(bufs[b].played >= params[REPEATS_PARAM].getValue()) {
            bufs[b].setFull(false);
            bufs[b].played = 0;
        }

        if(fRecordA) {
            if(!bufs[b].recording && !bufs[b].playing && !bufs[b].full) {
                fRecordA = false;
                startRecordingA(b);
            }
        }

        if(fRecordB) {
            if(!bufs[b].recording && !bufs[b].playing && !bufs[b].full) {
                fRecordB = false;
                startRecordingB(b);
            }
        }

        if(fPlay && !bufs[b].recording && !bufs[b].playing && bufs[b].full) {
            bufs[b].startPlaying(params[ATTACK_PARAM].getValue(), params[RELEASE_PARAM].getValue());
            fPlay = false;
        }

        if(bufs[b].recording) {

            if(params[STEREO_PARAM].getValue()) {
                bufs[b].record(inputs[SIG_A_INPUT].getVoltage(), inputs[SIG_B_INPUT].getVoltage());
            }
            else {
                bufs[b].record(inputs[ b == recordingA ? SIG_A_INPUT : SIG_B_INPUT ].getVoltage());
            }

            if(bufs[b].buf.size() >= maxBufSize) {
                bufs[b].stopRecording(params[MAX_LEN_PARAM].getValue() > 0);
                (b == recordingA ? recordingA : recordingB) = -1;
            }
        }

        if(bufs[b].playing) {
            if(pc >= (unsigned int) params[POLY_PARAM].getValue()) {
                bufs[b].stopPlaying();
            }
            else {
                out = bufs[b].play();
                if(params[STEREO_PARAM].getValue()) {
                    outA += out.l;
                    outB += out.r;
                }
                else {
                    (bufs[b].inputA ? outA : outB) += out.l;
                }
                pc++;
            }
        }

    }

    if(params[STEREO_PARAM].getValue() || outputs[SIG_B_OUTPUT].isConnected()) {
        outputs[SIG_A_OUTPUT].setVoltage(outA);
        outputs[SIG_B_OUTPUT].setVoltage(outB);
    }
    else {
        outputs[SIG_A_OUTPUT].setVoltage(outA + outB);
    }
}


void Tourette::onSampleRateChange() {
    avgBufA.setLen((unsigned int) APP->engine->getSampleRate() / 100);
    avgBufB.setLen((unsigned int) APP->engine->getSampleRate() / 100);
}


void Tourette::process(const ProcessArgs &args) {
    if(threshLoDb != params[THRESH_LO_PARAM].getValue()) {
        threshLoDb = params[THRESH_LO_PARAM].getValue();
        threshLo = dbToAmplitude(threshLoDb) * 5.f;
    }
    if(threshHiDb != params[THRESH_HI_PARAM].getValue()) {
        threshHiDb = params[THRESH_HI_PARAM].getValue();
        threshHi = dbToAmplitude(threshHiDb) * 5.f;
    }

    minBufSize = (unsigned int) (fabsf(params[MIN_LEN_PARAM].getValue()) * args.sampleRate);
    maxBufSize = (unsigned int) (fabsf(params[MAX_LEN_PARAM].getValue()) * args.sampleRate);

    if(params[RAND_PARAM].getValue() > 0.0) {

        rnd = (static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) <= params[RAND_PARAM].getValue();

        if(rnd) {
            std::shuffle(bufsRange.begin(), bufsRange.end(), rng);
        }
    }
    else {
        rnd = false;
    }

    if(params[STEREO_PARAM].getValue())
        processInputsStereo();
    else
        processInputsSplit();

    if(playTrigger.process(inputs[PLAY_INPUT].getVoltage())) {
        fPlay = true;
    }

    processBuffers();

    fRecordA = false;
    fRecordB = false;
    fPlay = false;
}


struct TouretteWidget : ModuleWidget {

    TouretteWidget(Tourette *module) {
		setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Tourette.svg")));

        addChild(createWidget<ScrewBlack>(Vec(0, 0)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewBlack>(Vec(0, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewBlack>(Vec(box.size.x - 1 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInput<PortGreen>(TOURETTE_SIG_A_INPUT_POS, module, Tourette::SIG_A_INPUT));
        addInput(createInput<PortGreen>(TOURETTE_SIG_B_INPUT_POS, module, Tourette::SIG_B_INPUT));

        addInput(createInput<PortPurple>(TOURETTE_TRIG_INPUT_POS, module, Tourette::PLAY_INPUT));

        addOutput(createOutput<PortRed>(TOURETTE_SIG_A_OUTPUT_POS, module, Tourette::SIG_A_OUTPUT));
        addOutput(createOutput<PortRed>(TOURETTE_SIG_B_OUTPUT_POS, module, Tourette::SIG_B_OUTPUT));

        TouretteDisplay *display = new TouretteDisplay(TOURETTE_DISPLAY_POS, TOURETTE_DISPLAY_SIZE, module);
        addChild(display);

        for(unsigned int b = 0; b < Tourette::BUFS_CNT; b++) {
            addChild(createLight<TinyLight<LightBlueGreenRed>>(TOURETTE_BUF_LIGHTS_POS[b], module, Tourette::BUF_LIGHTS + b * 3));
        }

        addParam(createParam<KnobMediumGreen>(TOURETTE_THRESH_LO_PARAM_POS, module, Tourette::THRESH_LO_PARAM));
        addParam(createParam<KnobMediumGreen>(TOURETTE_THRESH_HI_PARAM_POS, module, Tourette::THRESH_HI_PARAM));

        addParam(createParam<KnobMediumBlue>(TOURETTE_MIN_LEN_PARAM_POS, module, Tourette::MIN_LEN_PARAM));
        addParam(createParam<KnobMediumBlue>(TOURETTE_MAX_LEN_PARAM_POS, module, Tourette::MAX_LEN_PARAM));

        addParam(createParam<SnapKnob<KnobMediumYellow>>(TOURETTE_REPEATS_PARAM_POS, module, Tourette::REPEATS_PARAM));
        addParam(createParam<KnobMediumOrange>(TOURETTE_RAND_PARAM_POS, module, Tourette::RAND_PARAM));

        addParam(createParam<KnobMediumRed>(TOURETTE_ATTACK_PARAM_POS, module, Tourette::ATTACK_PARAM));
        addParam(createParam<KnobMediumRed>(TOURETTE_RELEASE_PARAM_POS, module, Tourette::RELEASE_PARAM));

        addParam(createParam<SnapKnob<KnobMediumPurple>>(TOURETTE_POLY_PARAM_POS, module, Tourette::POLY_PARAM));

        addParam(createParam<SwitchVertical>(TOURETTE_STEREO_PARAM_POS, module, Tourette::STEREO_PARAM));
    }

};


Model *modelTourette = createModel<Tourette, TouretteWidget>("Tourette");

