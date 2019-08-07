#pragma once


extern Plugin * pluginInstance;


template <typename BASE>
struct ButtonMediumLight : BASE {
    ButtonMediumLight() {
        this->box.size = mm2px(Vec(6.f, 6.f));
    }
    void drawHalo(const Widget::DrawArgs &args) override {}
};

struct ButtonMedium : SvgSwitch {

    static Vec lightPos(Vec buttonPos) {
        return buttonPos.plus(mm2px(Vec(1.1, 1.1)));
    }

    ButtonMedium() {
        momentary = true;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/Button_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/Button_1.svg")));
    }
};


struct SwitchHorizontal : SvgSwitch {
    SwitchHorizontal() {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/SwitchHorizontal_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/SwitchHorizontal_1.svg")));
    }
};

struct SwitchVertical : SvgSwitch {
    SwitchVertical() {
        momentary = false;
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/SwitchVertical_0.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/SwitchVertical_1.svg")));
    }
};

