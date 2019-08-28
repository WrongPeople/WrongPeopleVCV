#include "MIDIPlayer.hpp"
#include "components/MIDIPlayer/MIDIPlayerDisplay.hpp"


void MIDIPlayer::onReset() {
    stop();
    rewind();
    channels = 1;
    polyMode = ROTATE_MODE;
    fileLoaded = false;
    filePath = "";
    setTrackCount(0);
    setLoop(false);
}

json_t * MIDIPlayer::dataToJson() {
    json_t *rootJ = json_object();
    json_object_set_new(rootJ, "channels", json_integer(channels));
    json_object_set_new(rootJ, "polyMode", json_integer(polyMode));
    json_object_set_new(rootJ, "filePath", json_string(filePath.c_str()));
    json_object_set_new(rootJ, "loop", json_boolean(loop));
    return rootJ;
}

void MIDIPlayer::dataFromJson(json_t *rootJ) {
    json_t *channelsJ = json_object_get(rootJ, "channels");
    if(channelsJ)
        setChannels((int) json_integer_value(channelsJ));

    json_t *polyModeJ = json_object_get(rootJ, "polyMode");
    if(polyModeJ)
        setPolyMode((PolyMode) json_integer_value(polyModeJ));

    json_t *filePathJ = json_object_get(rootJ, "filePath");
    if(filePathJ) {
        filePath = json_string_value(filePathJ);
        loadFile();
    }

    json_t *loopJ = json_object_get(rootJ, "loop");
    if(loopJ) {
        setLoop(json_boolean_value(loopJ));
    }
}

void MIDIPlayer::loadFile() {
    stop();
    rewind();
    fileLoaded = false;
    setTrackCount(0);

    if(filePath.empty())
        return;

    if(midiFile.readUnoptimized(filePath)) {
        setTrackCount(midiFile.getTrackCount());
        midiFile.doTimeAnalysis();
        midiFile.linkNotePairs();
        midiFile.joinTracks();
        fileName = string::filename(filePath);
        fileDuration = (float) midiFile.getFileDurationInSeconds();
        fileDurationStr = timeToString(fileDuration);
        fileLoaded = true;
    }
}

void MIDIPlayer::setTrackCount(int trackCount) {
    this->trackCount = trackCount;
    for(int t = 0; t < TRACKS; t++) {
        lights[TRACK_LIGHTS + t].setBrightness(t < trackCount);
    }
}

void MIDIPlayer::setLoop(bool loop) {
    this->loop = loop;
    lights[LOOP_LIGHT].setBrightness(loop);
}

void MIDIPlayer::play() {
    if(fileLoaded) {
        rewind();
        panic();
        playing = true;
        lights[PLAY_LIGHT].setBrightness(1.f);
        playPulse.trigger();
    }
}

void MIDIPlayer::stop() {
    if(playing) {
        playing = false;
        lights[PLAY_LIGHT].setBrightness(0.f);
        stopPulse.trigger();
        panic();
    }
}

void MIDIPlayer::rewind() {
    playingTime = 0.0;
    playingEvent = 0;
}

void MIDIPlayer::process(const ProcessArgs &args) {

    if(playTrigger.process(params[PLAY_PARAM].getValue() + inputs[PLAY_INPUT].getVoltage())) {
        play();
    }

    if(stopTrigger.process(params[STOP_PARAM].getValue() + inputs[STOP_INPUT].getVoltage())) {
        stop();
    }

    if(loopTrigger.process(params[LOOP_PARAM].getValue())) {
        setLoop(!loop);
    }

    if(playing) {
        for(int i = 0; i < 500; i++) {
            if(playingEvent >= midiFile[track].size()) {
                if(loop)
                    play();
                else
                    stop();
                break;
            }
            if(midiFile[track][playingEvent].seconds > playingTime) {
                playingTime += args.sampleTime;
                break;
            }
            if(channel < 0 || channel == midiFile[track][playingEvent].getChannel()) {
                processMessage(midiFile[track][playingEvent].track, &midiFile[track][playingEvent]);
            }
            playingEvent++;
        }
    }

    for(int t = 0; t < TRACKS; t++) {
        outputs[CV_OUTPUTS + t].setChannels(channels);
        outputs[GATE_OUTPUTS + t].setChannels(channels);
        outputs[VELOCITY_OUTPUTS + t].setChannels(channels);
        outputs[RETRIGGER_OUTPUTS + t].setChannels(channels);

        if(t < trackCount) {
            for(int c = 0; c < channels; c++) {
                outputs[CV_OUTPUTS + t].setVoltage((notes[t][c] - 60.f) / 12.f, c);
                outputs[GATE_OUTPUTS + t].setVoltage(gates[t][c] ? 10.f : 0.f, c);
                outputs[VELOCITY_OUTPUTS + t].setVoltage(rescale(velocities[t][c], 0, 127, 0.f, 10.f), c);
                outputs[RETRIGGER_OUTPUTS + t].setVoltage(retriggerPulses[t][c].process(args.sampleTime) ? 10.f : 0.f, c);
            }
        }
    }

    outputs[PLAY_OUTPUT].setVoltage(playPulse.process(args.sampleTime) ? 10.f : 0.f);
    outputs[STOP_OUTPUT].setVoltage(stopPulse.process(args.sampleTime) ? 10.f : 0.f);
}

void MIDIPlayer::processMessage(int track, smf::MidiMessage *msg) {
    switch(msg->getCommandByte() >> 4) { // status
        case 0x8: // note off
            releaseNote(track, (uint8_t) msg->getKeyNumber());
            break;
        case 0x9: // note on
            if(msg->getVelocity() > 0) {
                int c = assignChannel(track, (uint8_t) msg->getKeyNumber());
                velocities[track][c] = (uint8_t) msg->getVelocity();
                pressNote(track, (uint8_t) msg->getKeyNumber(), c);
            }
            else {
                releaseNote(track, (uint8_t) msg->getKeyNumber());
            }
            break;
        case 0xa: // key aftertouch
            break;
        case 0xb: // cc
            processCC(track, msg);
            break;
        case 0xd: // channel aftertouch
            break;
        case 0xe: // pitch wheel
            break;
        case 0xf: // system
            break;
        default:
            break;
    }
}

void MIDIPlayer::processCC(int track, smf::MidiMessage *msg) {
    switch(msg->getControllerNumber()) {
        case 0x01: // mod
            break;
        case 0x40: // sustain
            if(msg->getControllerValue() >= 64)
                pressPedal(track);
            else
                releasePedal(track);
            break;
        default:
            break;
    }
}

void MIDIPlayer::pressNote(int track, uint8_t note, int channel) {
    auto it = std::find(heldNotes.begin(), heldNotes.end(), note);
    if(it != heldNotes.end())
        heldNotes.erase(it);
    heldNotes.push_back(note);
    notes[track][channel] = note;
    gates[track][channel] = true;
    retriggerPulses[track][channel].trigger();
}

void MIDIPlayer::releaseNote(int track, uint8_t note) {
    auto it = std::find(heldNotes.begin(), heldNotes.end(), note);
    if(it != heldNotes.end())
        heldNotes.erase(it);
    if(pedal[track])
        return;
    if(channels == 1) {
        if(note == notes[track][0] && !heldNotes.empty()) {
            uint8_t lastNote = heldNotes.back();
            notes[track][0] = lastNote;
            gates[track][0] = true;
            return;
        }
    }
    for(int c = 0; c < channels; c++) {
        if(notes[track][c] == note)
            gates[track][c] = false;
    }
}

void MIDIPlayer::pressPedal(int track) {
    pedal[track] = true;
}

void MIDIPlayer::releasePedal(int track) {
    pedal[track] = false;
    for(int c = 0; c < 16; c++) {
        gates[track][c] = false;
    }
    for(uint8_t note : heldNotes) {
        for(int c = 0; c < channels; c++) {
            if(notes[track][c] == note) {
                gates[track][c] = true;
            }
        }
    }
    if(channels == 1) {
        if(!heldNotes.empty()) {
            uint8_t lastNote = heldNotes.back();
            notes[track][0] = lastNote;
        }
    }
}

int MIDIPlayer::assignChannel(int track, uint8_t note) {
    if(channels == 1)
        return 0;

    switch(polyMode) {
        case REUSE_MODE:
            for(int c = 0; c < channels; c++) {
                if(notes[track][c] == note)
                    return c;
            }
            // fallthrough
        case ROTATE_MODE:
            for(int i = 0; i < channels; i++) {
                rotateIndex[track]++;
                if(rotateIndex[track] >= channels)
                    rotateIndex[track] = 0;
                if(!gates[track][rotateIndex[track]])
                    return rotateIndex[track];
            }
            rotateIndex[track]++;
            if(rotateIndex[track] >= channels)
                rotateIndex[track] = 0;
            return rotateIndex[track];
        case RESET_MODE:
            for(int c = 0; c < channels; c++) {
                if(!gates[track][c])
                    return c;
            }
            return channels - 1;
        default:
            return 0;
    }
}

void MIDIPlayer::panic() {
    for(int t = 0; t < TRACKS; t++) {
        pedal[t] = false;
        for(int c = 0; c < 16; c++) {
            notes[t][c] = 60;
            gates[t][c] = false;
            velocities[t][c] = 0;
        }
        pedal[t] = false;
        rotateIndex[t] = -1;
        heldNotes.clear();
    }
}

void MIDIPlayer::setChannels(int channels) {
    if(this->channels == channels)
        return;
    this->channels = channels;
    panic();
}

void MIDIPlayer::setPolyMode(PolyMode polyMode) {
    if(this->polyMode == polyMode)
        return;
    this->polyMode = polyMode;
    panic();
}


struct LoadFileItem : MenuItem {
    MIDIPlayer *module;

    void onAction(const event::Action &e) override {
        std::string dir = module->filePath.empty() ? "" : rack::string::directory(module->filePath).c_str();
        osdialog_filters *filters = osdialog_filters_parse("MIDI File:mid,midi");
        char *path = osdialog_file(OSDIALOG_OPEN, dir.empty() ? "" : dir.c_str(), NULL, filters);
        if(path) {
            module->filePath = path;
            free(path);
            module->loadFile();
        }
        osdialog_filters_free(filters);
    }
};

struct ChannelsValueItem : MenuItem {
    MIDIPlayer *module;
    int channels;
    void onAction(const event::Action &e) override {
        module->setChannels(channels);
    }
};

struct ChannelsItem : MenuItem {
    MIDIPlayer *module;
    Menu * createChildMenu() override {
        Menu *menu = new Menu();
        for(int channels = 1; channels <= 16; channels++) {
            ChannelsValueItem *item = new ChannelsValueItem();
            item->text = channels == 1 ? "Monophonic" : string::f("%d", channels);
            item->rightText = CHECKMARK(module->channels == channels);
            item->module = module;
            item->channels = channels;
            menu->addChild(item);
        }
        return menu;
    }
};

struct PolyModeValueItem : MenuItem {
    MIDIPlayer *module;
    MIDIPlayer::PolyMode polyMode;
    void onAction(const event::Action &e) override {
        module->setPolyMode(polyMode);
    }
};

struct PolyModeItem : MenuItem {
    MIDIPlayer *module;
    Menu *createChildMenu() override {
        Menu *menu = new Menu;
        std::vector<std::string> polyModeNames = {
            "Rotate",
            "Reuse",
            "Reset",
        };
        for(int i = 0; i < MIDIPlayer::NUM_POLY_MODES; i++) {
            MIDIPlayer::PolyMode polyMode = (MIDIPlayer::PolyMode) i;
            PolyModeValueItem *item = new PolyModeValueItem;
            item->text = polyModeNames[i];
            item->rightText = CHECKMARK(module->polyMode == polyMode);
            item->module = module;
            item->polyMode = polyMode;
            menu->addChild(item);
        }
        return menu;
    }
};

struct PanicItem : MenuItem {
    MIDIPlayer *module;
    void onAction(const event::Action &e) override {
        module->panic();
    }
};


struct MIDIPlayerWidget : ModuleWidget {

    MIDIPlayerWidget(MIDIPlayer *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MIDIPlayer.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addInput(createInput<PortPurple>(MIDIPLAYER_PLAY_INPUT_POS, module, MIDIPlayer::PLAY_INPUT));
        addInput(createInput<PortPurple>(MIDIPLAYER_STOP_INPUT_POS, module, MIDIPlayer::STOP_INPUT));

        addOutput(createOutput<PortOrange>(MIDIPLAYER_PLAY_OUTPUT_POS, module, MIDIPlayer::PLAY_OUTPUT));
        addOutput(createOutput<PortOrange>(MIDIPLAYER_STOP_OUTPUT_POS, module, MIDIPlayer::STOP_OUTPUT));

        addParam(createParam<ButtonMedium>(MIDIPLAYER_PLAY_PARAM_POS, module, MIDIPlayer::PLAY_PARAM));
        addParam(createParam<ButtonMedium>(MIDIPLAYER_STOP_PARAM_POS, module, MIDIPlayer::STOP_PARAM));
        addParam(createParam<ButtonMedium>(MIDIPLAYER_LOOP_PARAM_POS, module, MIDIPlayer::LOOP_PARAM));

        addChild(createLight<ButtonMediumLight<LightGreen>>(
            ButtonMedium::lightPos(MIDIPLAYER_PLAY_PARAM_POS),
            module, MIDIPlayer::PLAY_LIGHT));

        addChild(createLight<ButtonMediumLight<LightBlue>>(
            ButtonMedium::lightPos(MIDIPLAYER_LOOP_PARAM_POS),
            module, MIDIPlayer::LOOP_LIGHT));

        MIDIPlayerDisplay *display = new MIDIPlayerDisplay(MIDIPLAYER_DISPLAY_POS, MIDIPLAYER_DISPLAY_SIZE, module);
        addChild(display);

        for(int t = 0; t < MIDIPlayer::TRACKS; t++) {
            addChild(createLight<SmallLight<RedLight>>(MIDIPLAYER_TRACK_LIGHTS_POS[t], module, MIDIPlayer::TRACK_LIGHTS + t));
            addOutput(createOutput<PortYellow>(MIDIPLAYER_CV_OUTPUTS_POS[t], module, MIDIPlayer::CV_OUTPUTS + t));
            addOutput(createOutput<PortOrange>(MIDIPLAYER_GATE_OUTPUTS_POS[t], module, MIDIPlayer::GATE_OUTPUTS + t));
            addOutput(createOutput<PortYellow>(MIDIPLAYER_VELOCITY_OUTPUTS_POS[t], module, MIDIPlayer::VELOCITY_OUTPUTS + t));
            addOutput(createOutput<PortOrange>(MIDIPLAYER_RETRIGGER_OUTPUTS_POS[t], module, MIDIPlayer::RETRIGGER_OUTPUTS + t));
        }

    }

    void appendContextMenu(Menu *menu) override {
        MIDIPlayer *module = dynamic_cast<MIDIPlayer*>(this->module);
        assert(module);

        menu->addChild(new MenuEntry);

        LoadFileItem *loadFileItem = new LoadFileItem;
        loadFileItem->text = "Load File";
        loadFileItem->module = module;
        menu->addChild(loadFileItem);

        menu->addChild(new MenuEntry());

        ChannelsItem *channelsItem = new ChannelsItem;
        channelsItem->text = "Polyphony channels";
        channelsItem->rightText = string::f("%d", module->channels) + " " + RIGHT_ARROW;
        channelsItem->module = module;
        menu->addChild(channelsItem);

        PolyModeItem *polyModeItem = new PolyModeItem;
        polyModeItem->text = "Polyphony mode";
        polyModeItem->rightText = RIGHT_ARROW;
        polyModeItem->module = module;
        menu->addChild(polyModeItem);

        PanicItem *panicItem = new PanicItem;
        panicItem->text = "Panic";
        panicItem->module = module;
        menu->addChild(panicItem);
    }

};


Model *modelMIDIPlayer = createModel<MIDIPlayer, MIDIPlayerWidget>("MIDIPlayer");