#pragma once

using namespace rack;


extern Plugin * pluginInstance;


struct PortBlue : SvgPort {
    PortBlue() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortBlue.svg")));
    }
};

struct PortGreen : SvgPort {
    PortGreen() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortGreen.svg")));
    }
};

struct PortOrange : SvgPort {
    PortOrange() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortOrange.svg")));
    }
};

struct PortPurple : SvgPort {
    PortPurple() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortPurple.svg")));
    }
};

struct PortRed : SvgPort {
    PortRed() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortRed.svg")));
    }
};

struct PortYellow : SvgPort {
    PortYellow() {
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ComponentLibrary/PortYellow.svg")));
    }
};

