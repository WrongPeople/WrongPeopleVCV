#include "plugin.hpp"


Plugin * pluginInstance;


void init(Plugin * p) {
    pluginInstance = p;

    p->addModel(modelTourette);
    p->addModel(modelMIDIPlayer);
    p->addModel(modelLua);
}

