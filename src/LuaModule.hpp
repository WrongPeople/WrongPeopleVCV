#pragma once

#include <iostream>
#include "plugin.hpp"
#include "osdialog.h"

#define LUA_COMPAT_APIINTCASTS
#include "lua.hpp"

// Based on Frank Buss's experimental module
// (https://github.com/FrankBuss/FrankBussRackPlugin/blob/experimental/src/Lua.cpp)

// Scope code based on Andrew Belt's code from Scope module.
// (https://github.com/VCVRack/Fundamental/blob/v1/src/Scope.cpp)


struct Lua : Module {

    static const int SCRIPT_PORTS = 8;
    static const int SCRIPT_LOG_LEN = 9;
    static const int SCRIPT_LOG_MSG_LEN = 27;
    static const int SCRIPT_POINTS = 3;
    static const int SCRIPT_VALUES = 18;

    static const int SCOPE_DIVS = 4;
    static const int SCOPE_VALUES = 4;
    static const int SCOPE_BUFFER_SIZE = 512;

    constexpr static const float SCOPE_TIME_BASE = SCOPE_BUFFER_SIZE / SCOPE_DIVS;

    enum ParamIds {
        RELOAD_PARAM,
        NUM_PARAMS
    };

    enum InputIds {
        ENUMS(SCRIPT_INPUTS, SCRIPT_PORTS),
        NUM_INPUTS
    };

    enum OutputIds {
        ENUMS(SCRIPT_OUTPUTS, SCRIPT_PORTS),
        NUM_OUTPUTS
    };

    enum LightIds {
        RELOAD_LIGHT_GREEN,
        RELOAD_LIGHT_RED,
        NUM_LIGHTS
    };

    dsp::SchmittTrigger reloadTrigger;

    bool scriptLoaded = false;
    std::string scriptPath = "";
    std::string displayMessage = "";

    lua_State *L = NULL;

    static const std::vector<std::string> scriptLibs;
    static const std::vector<std::vector<std::string>> scriptExamples;

    enum DisplayMode {
        DISABLED_MODE,
        LOG_MODE,
        POINTS_MODE,
        VALUES_MODE,
        SCOPE_MODE,
        NUM_DISPLAY_MODES
    };
    DisplayMode displayMode;

    std::string scriptLogMessages[SCRIPT_LOG_LEN] = {""};
    unsigned int scriptLogMessagesCount = 0;
    unsigned int scriptLogMessagesOffset = 0;

    struct ScriptPoint {
        bool shown = false;
        float x = 0.f;
        float y = 0.f;
        bool dirEnabled = false;
        float dir = 0.f;
    };
    ScriptPoint scriptPoints[SCRIPT_POINTS];

    struct ScriptValue {
        bool shown = false;
        float val = 0.f;
    };
    ScriptValue scriptValues[SCRIPT_VALUES];

    float scopeBuffers[SCOPE_VALUES][SCOPE_BUFFER_SIZE] = {{0}};
    int scopeBufferIndex = 0;
    int scopeFrameIndex = 0;
    dsp::SchmittTrigger scopeTrigger;

    float scopeScale;
    float scopePos;
    float scopeTime;
    float scopeTrigThreshold;
    int scopeTrigValueIndex;

    Lua() {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(RELOAD_PARAM, 0.0, 1.0, 0.0, "Reload Script");

        for (int i = 0; i < SCRIPT_PORTS; i++) {
            configInput(SCRIPT_INPUTS + i, string::f("Input %d", i));
            configOutput(SCRIPT_OUTPUTS + i, string::f("Output %d", i));
        }
        onReset();
    }

    bool createLuaState();
    void setGlobalFunction(const char *name, lua_CFunction fn);
    void setGlobalClosure(const char *name, lua_CFunction fn);

    void loadScript();
    void unloadScript();
    void scriptError();
    void scriptError(const char * err);
    void clearScriptLogMessages();
    void addScriptLogMessage(std::string msg);
    void clearScriptPoints();
    void clearScriptValues();

    void processScope(const ProcessArgs &args);
    void scopeTrig();

    void onReset() override;
    json_t * dataToJson() override;
    void dataFromJson(json_t *rootJ) override;
    void process(const ProcessArgs &args) override;


    // void setDisplayMode(string mode)
    static int scriptSetDisplayMode(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));

        std::string mode = luaL_checkstring(L, 1);
        if(mode == "disabled")
            module->displayMode = DISABLED_MODE;
        else if(mode == "log")
            module->displayMode = LOG_MODE;
        else if(mode == "points")
            module->displayMode = POINTS_MODE;
        else if(mode == "values")
            module->displayMode = VALUES_MODE;
        else if(mode == "scope")
            module->displayMode = SCOPE_MODE;
        else
            luaL_argerror(L, 1, "Available modes: disabled, log, points, values, scope");

        return 0;
    }


    // void clearLog()
    static int scriptClearLog(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        module->clearScriptLogMessages();
        return 0;
    }

    // void log(string message)
    static int scriptLog(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));

        std::string msg = luaL_checkstring(L, 1);

        unsigned long lines = msg.length() / SCRIPT_LOG_MSG_LEN;

        if(msg.length() % SCRIPT_LOG_MSG_LEN > 0)
            lines++;

        for(unsigned long l = 0; l < lines; l++) {
            module->addScriptLogMessage(msg.substr(l * SCRIPT_LOG_MSG_LEN, SCRIPT_LOG_MSG_LEN));
        }

        std::cout << "Lua: " << msg << std::endl;

        return 0;
    }


    // void showPoint(int point)
    static int scriptShowPoint(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int point = _scriptCheckPointIndex(L);
        module->scriptPoints[point].shown = true;
        return 0;
    }

    // void hidePoint(int point)
    static int scriptHidePoint(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int point = _scriptCheckPointIndex(L);
        module->scriptPoints[point].shown = false;
        return 0;
    }

    // void setPoint(int point, float x, float y, float dir = nil)
    static int scriptSetPoint(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));

        int point = _scriptCheckPointIndex(L);
        module->scriptPoints[point].x = clamp((float) luaL_checknumber(L, 2), -10.f, 10.f);
        module->scriptPoints[point].y = clamp((float) luaL_checknumber(L, 3), -10.f, 10.f);
        if(lua_isnoneornil(L, 4)) {
            module->scriptPoints[point].dir = 0.f;
            module->scriptPoints[point].dirEnabled = false;
        }
        else {
            module->scriptPoints[point].dir = clamp((float) luaL_checknumber(L, 4), -180.f, 180.f);
            module->scriptPoints[point].dirEnabled = true;
        }

        return 0;
    }


    // void showValue(int index)
    static int scriptShowValue(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int index = _scriptCheckValueIndex(L);
        module->scriptValues[index].shown = true;
        return 0;
    }

    // void hideValue(int index)
    static int scriptHideValue(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int index = _scriptCheckValueIndex(L);
        module->scriptValues[index].shown = false;
        return 0;
    }

    // void setValue(int index, float value)
    static int scriptSetValue(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int index = _scriptCheckValueIndex(L);
        module->scriptValues[index].val = (float) luaL_checknumber(L, 2);
        return 0;
    }


    // void setScopeScale(float scale)
    static int scriptSetScopeScale(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        float scale = (float) luaL_checknumber(L, 1);
        luaL_argcheck(L, scale > 0.f, 1, "Invalid scale");
        module->scopeScale = 0.5f / scale;
        return 0;
    }

    // void setScopePos(float pos)
    static int scriptSetScopePos(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        float pos = (float) luaL_checknumber(L, 1);
        luaL_argcheck(L, pos >= -10.f && pos <= 10.f, 1, "Position must be in range -10 ... 10");
        module->scopePos = pos;
        return 0;
    }

    // void setScopeTime(float time)
    static int scriptSetScopeTime(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        float time = (float) luaL_checknumber(L, 1);
        luaL_argcheck(L, time > 0.f && time <= 2000.f, 1, "Invalid time");
        module->scopeTime = (time / 1000.f) / SCOPE_TIME_BASE;
        return 0;
    }

    // void setScopeTrigThreshold(float value)
    static int scriptSetScopeTrigThreshold(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        float value = (float) luaL_checknumber(L, 1);
        luaL_argcheck(L, value >= -10.f && value <= 10.f, 1, "Threshold must be in range -10 ... 10");
        module->scopeTrigThreshold = value;
        return 0;
    }

    // void setScopeTrigValue(int index)
    static int scriptSetScopeTrigValue(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int index = luaL_checkint(L, 1);
        luaL_argcheck(L, index >= -1 && index < SCOPE_VALUES, 1,
            string::f("Available values: 0 ... %d or -1 for disable", SCOPE_VALUES - 1).c_str());
        module->scopeTrigValueIndex = index;
        return 0;
    }

    // void scopeTrig()
    static int scriptScopeTrig(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        module->scopeTrig();
        return 0;
    }


    // bool isInputConnected(int input)
    static int scriptIsInputConnected(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushboolean(L, module->inputs[_scriptCheckPortIndex(L)].isConnected());
        return 1;
    }

    // float getVoltage(int input, int channel = 0)
    static int scriptGetVoltage(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getVoltage(_scriptCheckChannelIndex(L, 2)));
        return 1;
    }

    // float getPolyVoltage(int input, int channel)
    static int scriptGetPolyVoltage(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getPolyVoltage(_scriptCheckChannelIndex(L, 2, true)));
        return 1;
    }

    // float getNormalVoltage(int input, float normalVoltage, int channel = 0)
    static int scriptGetNormalVoltage(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getNormalVoltage(
            (float) luaL_checknumber(L, 2),
            _scriptCheckChannelIndex(L, 3)
        ));
        return 1;
    }

    // float getNormalPolyVoltage(int input, float normalVoltage, int channel)
    static int scriptGetNormalPolyVoltage(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getNormalPolyVoltage(
            (float) luaL_checknumber(L, 2),
            _scriptCheckChannelIndex(L, 3)
        ));
        return 1;
    }

    // float getVoltageSum(int input)
    static int scriptGetVoltageSum(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getVoltageSum());
        return 1;
    }

    // int getChannels(int input)
    static int scriptGetChannels(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushnumber(L, module->inputs[_scriptCheckPortIndex(L)].getChannels());
        return 1;
    }

    // bool isMonophonic(int input)
    static int scriptIsMonophonic(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushboolean(L, module->inputs[_scriptCheckPortIndex(L)].isMonophonic());
        return 1;
    }

    // bool isPolyphonic(int input)
    static int scriptIsPolyphonic(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushboolean(L, module->inputs[_scriptCheckPortIndex(L)].isPolyphonic());
        return 1;
    }


    // bool isOutputConnected(int output)
    static int scriptIsOutputConnected(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        lua_pushboolean(L, module->outputs[_scriptCheckPortIndex(L)].isConnected());
        return 1;
    }

    // void setVoltage(int output, float voltage, int channel = 0)
    static int scriptSetVoltage(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        module->outputs[_scriptCheckPortIndex(L)].setVoltage(
            (float) luaL_checknumber(L, 2),
            _scriptCheckChannelIndex(L, 3)
        );
        return 0;
    }

    // void setChannels(int output, int channels)
    static int scriptSetChannels(lua_State *L) {
        Lua *module = (Lua *) lua_touserdata(L, lua_upvalueindex(1));
        int channels = luaL_checkint(L, 2);
        luaL_argcheck(L, channels >= 0 && channels <= 16, 2, "Invalid number of channels");
        module->outputs[_scriptCheckPortIndex(L)].setChannels(channels);
        return 0;
    }


private:

    static inline int _scriptCheckPointIndex(lua_State *L, int numArg = 1) {
        int index = luaL_checkint(L, numArg);
        luaL_argcheck(L, index >= 0 && index < SCRIPT_POINTS, numArg,
                      string::f("Available points: 0 ... %d", SCRIPT_POINTS - 1).c_str());
        return index;
    }

    static inline int _scriptCheckValueIndex(lua_State *L, int numArg = 1, int max = SCRIPT_VALUES) {
        int index = luaL_checkint(L, numArg);
        luaL_argcheck(L, index >= 0 && index < max, numArg,
                      string::f("Available values: 0 ... %d", max - 1).c_str());
        return index;
    }

    static inline int _scriptCheckPortIndex(lua_State *L, int numArg = 1) {
        int index = luaL_checkint(L, numArg);
        luaL_argcheck(L, index >= 0 && index < SCRIPT_PORTS, numArg,
                      string::f("Available ports: 0 ... %d", SCRIPT_PORTS - 1).c_str());
        return index;
    }

    static inline int _scriptCheckChannelIndex(lua_State *L, int numArg, bool req = false) {
        int index = req ? luaL_checkint(L, numArg) : luaL_optint(L, numArg, 0);
        luaL_argcheck(L, index >= 0 && index < 16, numArg, "Available channels: 0 ... 16");
        return index;
    }

};
