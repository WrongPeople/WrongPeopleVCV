#pragma once

using namespace rack;


extern Plugin * pluginInstance;


template <typename BASE>
struct SnapKnob : BASE {
    SnapKnob() {
        this->snap = true;
    }
};


struct KnobMedium : SvgKnob {
    KnobMedium() {
        minAngle = -0.85f * (float) M_PI;
        maxAngle = 0.85f * (float) M_PI;
    }
};


struct KnobMediumBlue : KnobMedium {
    KnobMediumBlue() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumBlue.svg")));
    }
};

struct KnobMediumGreen : KnobMedium {
    KnobMediumGreen() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumGreen.svg")));
    }
};

struct KnobMediumOrange : KnobMedium {
    KnobMediumOrange() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumOrange.svg")));
    }
};

struct KnobMediumPurple : KnobMedium {
    KnobMediumPurple() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumPurple.svg")));
    }
};

struct KnobMediumRed : KnobMedium {
    KnobMediumRed() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumRed.svg")));
    }
};

struct KnobMediumYellow : KnobMedium {
    KnobMediumYellow() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/KnobMediumYellow.svg")));
    }
};

