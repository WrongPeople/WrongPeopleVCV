#include "Lua.hpp"
#include "components/Lua/LuaDisplay.hpp"


json_t * Lua::dataToJson() {
    json_t *rootJ = json_object();
    json_object_set_new(rootJ, "scriptPath", json_string(scriptPath.c_str()));
    return rootJ;
}

void Lua::dataFromJson(json_t * rootJ) {
    json_t *scriptPathJ = json_object_get(rootJ, "scriptPath");
    if(scriptPathJ) {
        scriptPath = json_string_value(scriptPathJ);
        loadScript();
    }
}

void Lua::onReset() {
    scriptPath = "";
    displayMessage = "Right click to load script";
    unloadScript();
}

void Lua::loadScript() {
    if(scriptPath.empty())
        return;

    unloadScript();
    createLuaState();

    if(luaL_loadfile(L, scriptPath.c_str())) {
        scriptError();
    }
    else {
        if(lua_pcall(L, 0, LUA_MULTRET, 0)) {
            scriptError();
        }
        else {
            scriptLoaded = true;
            displayMessage = string::filename(scriptPath);
            lights[RELOAD_LIGHT_GREEN].setBrightness(1);
            lights[RELOAD_LIGHT_RED].setBrightness(0);
        }
    }
}

void Lua::unloadScript() {
    scriptLoaded = false;
    lights[RELOAD_LIGHT_GREEN].setBrightness(0);
    lights[RELOAD_LIGHT_RED].setBrightness(0);

    if(L) {
        lua_close(L);
        L = NULL;
    }

    displayMode = DISABLED_MODE;
    clearScriptLogMessages();
    clearScriptPoints();
    clearScriptValues();

    scopeTrig();
    scopeScale = 0.1f; // 5 V/div
    scopePos = 0.f;
    scopeTime = 0.01f / SCOPE_TIME_BASE; // 10 ms/div
    scopeTrigThreshold = 0.f;
    scopeTrigValueIndex = 0;
}

void Lua::scriptError() {
    scriptError(lua_tostring(L, -1));
    lua_pop(L, 1);
}

void Lua::scriptError(const char * err) {
    scriptLoaded = false;
    displayMessage = err;
    std::cerr << "Lua Script error: " << displayMessage << std::endl;
    lights[RELOAD_LIGHT_GREEN].setBrightness(0);
    lights[RELOAD_LIGHT_RED].setBrightness(1);
}

void Lua::clearScriptLogMessages() {
    scriptLogMessagesOffset = 0;
    scriptLogMessagesCount = 0;
    for(int i = 0; i < SCRIPT_LOG_LEN; i++) {
        scriptLogMessages[i] = "";
    }
}

void Lua::clearScriptPoints() {
    for(int i = 0; i < SCRIPT_POINTS; i++) {
        scriptPoints[i] = ScriptPoint{};
    }
}

void Lua::clearScriptValues() {
    for(int i = 0; i < SCRIPT_VALUES; i++) {
        scriptValues[i] = ScriptValue{};
    }
}

void Lua::createLuaState() {
    L = luaL_newstate();
    luaL_openlibs(L);

    setGlobalClosure("setDisplayMode", Lua::scriptSetDisplayMode);

    // display, log mode
    setGlobalClosure("clearLog", Lua::scriptClearLog);
    setGlobalClosure("log", Lua::scriptLog);

    // display, points mode
    setGlobalClosure("showPoint", Lua::scriptShowPoint);
    setGlobalClosure("hidePoint", Lua::scriptHidePoint);
    setGlobalClosure("setPoint", Lua::scriptSetPoint);

    // display, values/scope modes
    setGlobalClosure("showValue", Lua::scriptShowValue);
    setGlobalClosure("hideValue", Lua::scriptHideValue);
    setGlobalClosure("setValue", Lua::scriptSetValue);

    // display, scope mode
    setGlobalClosure("setScopeScale", Lua::scriptSetScopeScale);
    setGlobalClosure("setScopePos", Lua::scriptSetScopePos);
    setGlobalClosure("setScopeTime", Lua::scriptSetScopeTime);
    setGlobalClosure("setScopeTrigThreshold", Lua::scriptSetScopeTrigThreshold);
    setGlobalClosure("setScopeTrigValue", Lua::scriptSetScopeTrigValue);
    setGlobalClosure("scopeTrig", Lua::scriptScopeTrig);

    // inputs
    setGlobalClosure("isInputConnected", Lua::scriptIsInputConnected);
    setGlobalClosure("getVoltage", Lua::scriptGetVoltage);
    setGlobalClosure("getPolyVoltage", Lua::scriptGetPolyVoltage);
    setGlobalClosure("getNormalVoltage", Lua::scriptGetNormalVoltage);
    setGlobalClosure("getNormalPolyVoltage", Lua::scriptGetNormalPolyVoltage);
    setGlobalClosure("getVoltageSum", Lua::scriptGetVoltageSum);
    setGlobalClosure("getChannels", Lua::scriptGetChannels);
    setGlobalClosure("isMonophonic", Lua::scriptIsMonophonic);
    setGlobalClosure("isPolyphonic", Lua::scriptIsPolyphonic);

    // outputs
    setGlobalClosure("isOutputConnected", Lua::scriptIsOutputConnected);
    setGlobalClosure("setVoltage", Lua::scriptSetVoltage);
    setGlobalClosure("setChannels", Lua::scriptSetChannels);

    for(auto const &lib : scriptLibs) {
        if(luaL_dofile(L, asset::plugin(pluginInstance, lib).c_str())) {
            scriptError();
        }
    }
}

void Lua::setGlobalFunction(const char * name, lua_CFunction fn) {
    lua_pushcfunction(L, fn);
    lua_setglobal(L, name);
}

void Lua::setGlobalClosure(const char *name, lua_CFunction fn) {
    lua_pushlightuserdata(L, this);
    lua_pushcclosure(L, fn, 1);
    lua_setglobal(L, name);
}


void Lua::process(const ProcessArgs &args) {

    if(reloadTrigger.process(params[RELOAD_PARAM].getValue())) {
        loadScript();
    }

    if(scriptLoaded) {
        lua_getglobal(L, "process");
        if(lua_isfunction(L, -1)) {
            lua_pushnumber(L, args.sampleRate);
            lua_pushnumber(L, args.sampleTime);
            if(lua_pcall(L, 2, 0, 0)) {
                scriptError();
            }
        }
        else {
            lua_pop(L, 1);
        }
    }

    if(displayMode == SCOPE_MODE) {
        processScope(args);
    }

}

void Lua::processScope(const ProcessArgs &args) {
    int frameCount = (int) std::ceil(scopeTime * args.sampleRate);

    if(scopeBufferIndex < SCOPE_BUFFER_SIZE) {
        if(++scopeFrameIndex > frameCount) {
            scopeFrameIndex = 0;
            for(int i = 0; i < SCOPE_VALUES; i++) {
                scopeBuffers[i][scopeBufferIndex] = scriptValues[i].val;
            }
            scopeBufferIndex++;
        }
    }

    if(scopeBufferIndex < SCOPE_BUFFER_SIZE) {
        return;
    }

    scopeFrameIndex++;

    if(scopeTrigValueIndex != -1) {
        float trigVoltage = scriptValues[scopeTrigValueIndex].val;
        if(scopeTrigger.process(rescale(trigVoltage, scopeTrigThreshold, scopeTrigThreshold + 0.001f, 0.f, 1.f))) {
            scopeTrig();
            return;
        }
    }

    const float holdTime = 0.5f;
    if(scopeFrameIndex * args.sampleTime >= holdTime) {
        scopeTrig();
        return;
    }
}

void Lua::scopeTrig() {
    scopeTrigger.reset();
    scopeBufferIndex = 0;
    scopeFrameIndex = 0;
}


struct LoadScriptItem : MenuItem {
    Lua *module;

    void onAction(const event::Action &e) override {
        std::string dir = module->scriptPath.empty() ? "" : rack::string::directory(module->scriptPath).c_str();
        osdialog_filters *filters = osdialog_filters_parse("Lua Script:lua,luna,lunaire,anair");
        char *path = osdialog_file(OSDIALOG_OPEN, dir.empty() ? "" : dir.c_str(), NULL, filters);
        if(path) {
            module->scriptPath = path;
            free(path);
            module->loadScript();
        }
        osdialog_filters_free(filters);
    }
};


struct LuaWidget : ModuleWidget {

    LuaWidget(Lua *module) {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/Lua.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addParam(createParam<ButtonMedium>(LUA_RELOAD_PARAM_POS, module, Lua::RELOAD_PARAM));

        addChild(createLight<ButtonMediumLight<LightGreenRed>>(
            ButtonMedium::lightPos(LUA_RELOAD_PARAM_POS),
            module, Lua::RELOAD_LIGHT_GREEN));

        LuaDisplay *display = new LuaDisplay(LUA_DISPLAY_POS, LUA_DISPLAY_SIZE, module);
        addChild(display);

        for(unsigned int p = 0; p < Lua::SCRIPT_PORTS; p++) {
            addInput(createInput<PortGreen>(LUA_SCRIPT_INPUTS_POS[p], module, Lua::SCRIPT_INPUTS + p));
            addOutput(createOutput<PortRed>(LUA_SCRIPT_OUTPUTS_POS[p], module, Lua::SCRIPT_OUTPUTS + p));
        }
    }

    void appendContextMenu(Menu *menu) override {
        Lua *module = dynamic_cast<Lua*>(this->module);
        assert(module);

        menu->addChild(new MenuEntry);

        LoadScriptItem *loadScriptItem = new LoadScriptItem;
        loadScriptItem->text = "Load Script";
        loadScriptItem->module = module;
        menu->addChild(loadScriptItem);
    }

};


Model *modelLua = createModel<Lua, LuaWidget>("Lua");
