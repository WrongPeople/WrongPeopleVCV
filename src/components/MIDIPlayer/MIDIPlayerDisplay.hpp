#pragma once

#include "../../MIDIPlayer.hpp"


struct MIDIPlayerDisplay : TransparentWidget {

    MIDIPlayer *module;
    std::shared_ptr<Font> font;

    MIDIPlayerDisplay(Vec pos, Vec size, MIDIPlayer *module) {
        box.pos = pos;
        box.size = size;
        this->module = module;
        font = APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/ShareTechMono-Regular.ttf"));
    }

    void draw(const DrawArgs &args) override {
        nvgFontSize(args.vg, 12);
        nvgFontFaceId(args.vg, font->handle);
        nvgTextLetterSpacing(args.vg, -2);
        nvgFillColor(args.vg, module && module->fileLoaded ? COLOR_BLUE_3 : COLOR_RED_3);
        nvgTextBox(args.vg, 8, 13, box.size.x - 10.f,
            module && module->fileLoaded ? module->fileName.c_str() : "Right click to load file", NULL);

        if(module && module->fileLoaded) {
            std::string timeStr = timeToString(module->playingTime) + " / " + module->fileDurationStr;

            nvgFontSize(args.vg, 16);
            nvgFontFaceId(args.vg, font->handle);
            nvgTextLetterSpacing(args.vg, 0);
            nvgFillColor(args.vg, COLOR_GREEN_3);
            nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_BOTTOM);
            nvgText(args.vg, box.size.x / 2, box.size.y - 8, timeStr.c_str(), NULL);
        }
    }

};
