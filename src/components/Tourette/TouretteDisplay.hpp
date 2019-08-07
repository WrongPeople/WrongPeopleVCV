#pragma once

#include "../../Tourette.hpp"


struct TouretteDisplay : TransparentWidget {

    Tourette *module;

    TouretteDisplay(Vec pos, Vec size, Tourette *module) {
        box.pos = pos.plus(Vec(1.f, 0.f));
        box.size = size.minus(Vec(2.f, 0.f));
        this->module = module;
    }

    void draw(const DrawArgs &args) override {
        if(!module)
            return;

        float posLo = normalizeDb(module->params[Tourette::THRESH_LO_PARAM].getValue());
        float posHi = normalizeDb(module->params[Tourette::THRESH_HI_PARAM].getValue());

        // input a
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 2.0, normalizeDb(amplitudeToDb(module->avgBufA.avg / 5.f)), 4.0);
        nvgFillColor(args.vg, module->recordingA == -1 ? COLOR_BLUE_2 : COLOR_RED_2);
        nvgFill(args.vg);

        // input b
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 7.0, normalizeDb(amplitudeToDb(module->avgBufB.avg / 5.f)), 4.0);
        nvgFillColor(args.vg, module->recordingB == -1 ? COLOR_BLUE_2 : COLOR_RED_2);
        nvgFill(args.vg);

        // threshold low
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0.0, 0.0, posLo, box.size.y);
        nvgFillColor(args.vg, nvgTransRGBAf(COLOR_GREEN_1, 0.05));
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, posLo, 0.0);
        nvgLineTo(args.vg, posLo, box.size.y);
        nvgStrokeColor(args.vg, COLOR_GREEN_2);
        nvgStrokeWidth(args.vg, 1.0);
        nvgStroke(args.vg);

        // threshold high
        nvgBeginPath(args.vg);
        nvgRect(args.vg, posHi, 0.0, box.size.x - posHi, box.size.y);
        nvgFillColor(args.vg, nvgTransRGBAf(COLOR_GREEN_1, 0.05));
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, posHi, 0.0);
        nvgLineTo(args.vg, posHi, box.size.y);
        nvgStrokeColor(args.vg, COLOR_GREEN_2);
        nvgStrokeWidth(args.vg, 1.0);
        nvgStroke(args.vg);

    }

    inline float normalizeDb(float v) {
        return v > -INFINITY ? clamp((v + 60.f) / 66.f, 0.f, 1.f) * box.size.x : 0;
    }

};

