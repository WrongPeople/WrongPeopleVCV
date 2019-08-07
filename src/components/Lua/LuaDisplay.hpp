#pragma once

#include "lua.hpp"


struct LuaDisplay : TransparentWidget {

    Lua *module;
    std::shared_ptr<Font> font;

    Rect valuesDisplay;
    Rect scopeDisplay;

    const NVGcolor pointColors[Lua::SCRIPT_POINTS] = {
        COLOR_RED_2,
        COLOR_GREEN_2,
        COLOR_YELLOW_2,
    };

    const NVGcolor scopeColors[Lua::SCOPE_VALUES] = {
        COLOR_RED_2,
        COLOR_GREEN_2,
        COLOR_YELLOW_2,
        COLOR_BLUE_2,
    };


    LuaDisplay(Vec pos, Vec size, Lua *module) {
        box.pos = pos;
        box.size = size;
        this->module = module;
        font = APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/ShareTechMono-Regular.ttf"));
        // 150.000000 143.297241
        valuesDisplay = Rect(7.f, 29.f, 105.f, 105.f);
        scopeDisplay = Rect(0.f, 33.f, size.x, size.y - 47.f);
    }

    void draw(const DrawArgs &args) override {
        if(!module)
            return;

        nvgFontSize(args.vg, 12);
        nvgFontFaceId(args.vg, font->handle);
        nvgTextLetterSpacing(args.vg, -2);
        nvgFillColor(args.vg, module->scriptLoaded ? COLOR_BLUE_3 : COLOR_RED_3);
        nvgTextBox(args.vg, 6, 13, box.size.x - 10.f, module->displayMessage.c_str(), NULL);

        if(!module->scriptLoaded)
            return;

        nvgStrokeColor(args.vg, COLOR_YELLOW_3);
        nvgStrokeWidth(args.vg, 0.5);
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, 0, 18.5);
        nvgLineTo(args.vg, box.size.x, 18.5);
        nvgStroke(args.vg);

        switch(module->displayMode) {
            case Lua::DISABLED_MODE:
                return;
            case Lua::LOG_MODE: {
                nvgFillColor(args.vg, COLOR_GREEN_3);

                int line;
                int count = module->scriptLogMessagesCount;
                int offset = count < Lua::SCRIPT_LOG_LEN ? 0 : module->scriptLogMessagesOffset;
                for(int i = 0; i < count; i++) {
                    line = i + offset;
                    if(line >= Lua::SCRIPT_LOG_LEN)
                        line -= Lua::SCRIPT_LOG_LEN;
                    nvgText(args.vg, 7, 34 + 13 * i, module->scriptLogMessages[line].c_str(), NULL);
                }
            } break;
            case Lua::POINTS_MODE: {
                nvgStrokeColor(args.vg, COLOR_ORANGE_2);
                nvgStrokeWidth(args.vg, 0.3);
                nvgBeginPath(args.vg);
                nvgRect(args.vg, valuesDisplay.pos.x, valuesDisplay.pos.y, valuesDisplay.size.x, valuesDisplay.size.y);
                nvgStroke(args.vg);

                nvgStrokeColor(args.vg, COLOR_PURPLE_2);
                nvgBeginPath(args.vg);
                nvgMoveTo(args.vg, valuesDisplay.pos.x + valuesDisplay.size.x / 2.f, valuesDisplay.pos.y);
                nvgLineTo(args.vg, valuesDisplay.pos.x + valuesDisplay.size.x / 2.f, valuesDisplay.pos.y + valuesDisplay.size.y);
                nvgStroke(args.vg);
                nvgBeginPath(args.vg);
                nvgMoveTo(args.vg, valuesDisplay.pos.x, valuesDisplay.pos.y + valuesDisplay.size.y / 2.f);
                nvgLineTo(args.vg, valuesDisplay.pos.x + valuesDisplay.size.x, valuesDisplay.pos.y + valuesDisplay.size.y / 2.f);
                nvgStroke(args.vg);

                nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

                Lua::ScriptPoint p;
                for(unsigned int i = 0; i < Lua::SCRIPT_POINTS; i++) {
                    p = module->scriptPoints[i];
                    if(!p.shown)
                        continue;

                    float x = (p.x + 10.f) * (valuesDisplay.size.x / 20.f) + valuesDisplay.pos.x;
                    float y = (-p.y + 10.f) * (valuesDisplay.size.y / 20.f) + valuesDisplay.pos.y;

                    nvgFillColor(args.vg, pointColors[i]);
                    nvgStrokeColor(args.vg, pointColors[i]);
                    nvgStrokeWidth(args.vg, 1.5f);

                    nvgBeginPath(args.vg);
                    nvgCircle(args.vg, x, y, 1.5f);
                    nvgFill(args.vg);

                    if(p.dirEnabled) {
                        float angle = nvgDegToRad(p.dir);
                        nvgBeginPath(args.vg);
                        nvgMoveTo(args.vg, x, y);
                        nvgLineTo(args.vg, x + sinf(angle) * 5.f, y - cosf(angle) * 5.f);
                        nvgStroke(args.vg);
                    }

                    nvgText(args.vg, box.size.x - 1, 24 + 39 * i, string::f("%.2f", p.x).c_str(), NULL);
                    nvgText(args.vg, box.size.x - 1, 37 + 39 * i, string::f("%.2f", p.y).c_str(), NULL);
                    nvgText(args.vg, box.size.x - 1, 50 + 39 * i, string::f("%.1f", p.dir).c_str(), NULL);
                }
            } break;
            case Lua::VALUES_MODE: {
                nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

                Lua::ScriptValue v;
                float x, y;
                for(int i = 0; i < Lua::SCRIPT_VALUES; i++) {
                    v = module->scriptValues[i];

                    if(!v.shown)
                        continue;

                    if(i < Lua::SCRIPT_VALUES / 2) {
                        x = box.size.x / 2 - 10;
                        y = 24 + 13 * i;
                    }
                    else {
                        x = box.size.x - 7;
                        y = 24 + 13 * (i - Lua::SCRIPT_VALUES / 2);
                    }

                    nvgFillColor(args.vg, nvgRGB(154, 154, 154));
                    nvgText(args.vg, x - 42, y, string::f("%d: ", i).c_str(), NULL);

                    nvgFillColor(args.vg, v.val == 0.f ? COLOR_ORANGE_4 : (v.val > 0.f ? COLOR_GREEN_3 : COLOR_RED_3));
                    nvgText(args.vg, x, y, string::f("%.5f", v.val).c_str(), NULL);
                }
            } break;
            case Lua::SCOPE_MODE: {
                drawScope(args);
            } break;
            default:
                return;
        }
    }

    void drawScope(const DrawArgs &args) {
        nvgStrokeColor(args.vg, COLOR_ORANGE_2);
        nvgStrokeWidth(args.vg, 0.3f);
        float divX = scopeDisplay.size.x / Lua::SCOPE_DIVS;
        float divY = scopeDisplay.size.y / 4;
        for(int x = 1; x < Lua::SCOPE_DIVS; x++) {
            nvgBeginPath(args.vg);
            nvgMoveTo(args.vg, scopeDisplay.pos.x + divX * x, scopeDisplay.pos.y);
            nvgLineTo(args.vg, scopeDisplay.pos.x + divX * x, scopeDisplay.pos.y + scopeDisplay.size.y);
            nvgStroke(args.vg);
        }
        nvgStrokeColor(args.vg, COLOR_PURPLE_2);
        for(int y = 0; y < 5; y++) {
            nvgBeginPath(args.vg);
            nvgMoveTo(args.vg, scopeDisplay.pos.x, scopeDisplay.pos.y + divY * y);
            nvgLineTo(args.vg, scopeDisplay.pos.x + scopeDisplay.size.x, scopeDisplay.pos.y + divY * y);
            nvgStroke(args.vg);
        }

        nvgStrokeWidth(args.vg, 1.5f);
        nvgTextAlign(args.vg, NVG_ALIGN_RIGHT | NVG_ALIGN_TOP);

        for(int i = 0; i < Lua::SCOPE_VALUES; i++) {
            if(!module->scriptValues[i].shown)
                continue;

            nvgFillColor(args.vg, scopeColors[i]);
            float x, y;
            if(i < Lua::SCOPE_VALUES / 2) {
                x = (box.size.x / 2) * (i + 1) - 20;
                y = 21;
            }
            else {
                x = (box.size.x / 2) * (i - Lua::SCOPE_VALUES / 2 + 1) - 20;
                y = box.size.y - 12;
            }
            nvgText(args.vg, x, y, string::f("%.5f", module->scriptValues[i].val).c_str(), NULL);

            nvgStrokeColor(args.vg, scopeColors[i]);
            drawScopeWaveform(args, module->scopeBuffers[i], module->scopePos, module->scopeScale);
        }

        float trigPos = (module->scopeTrigThreshold + module->scopePos) * module->scopeScale;
        drawScopeTrig(args, trigPos);
    }

    void drawScopeWaveform(const DrawArgs &args, float *buffer, float offset, float gain) {
        nvgSave(args.vg);
        nvgScissor(args.vg, scopeDisplay.pos.x, scopeDisplay.pos.y, scopeDisplay.size.x, scopeDisplay.size.y);
        nvgBeginPath(args.vg);

        for(int i = 0; i < Lua::SCOPE_BUFFER_SIZE; i++) {
            Vec v;
            v.x = (float) i / (Lua::SCOPE_BUFFER_SIZE - 1);
            v.y = (buffer[i] + offset) * gain / 2.f + 0.5f;
            Vec p;
            p.x = rescale(v.x, 0.f, 1.f, scopeDisplay.pos.x, scopeDisplay.pos.x + scopeDisplay.size.x);
            p.y = rescale(v.y, 0.f, 1.f, scopeDisplay.pos.y + scopeDisplay.size.y, scopeDisplay.pos.y);
            if (i == 0)
                nvgMoveTo(args.vg, p.x, p.y);
            else
                nvgLineTo(args.vg, p.x, p.y);
        }

        nvgLineCap(args.vg, NVG_ROUND);
        nvgMiterLimit(args.vg, 2.f);
        nvgGlobalCompositeOperation(args.vg, NVG_LIGHTER);
        nvgStroke(args.vg);
        nvgResetScissor(args.vg);
        nvgRestore(args.vg);
    }

    void drawScopeTrig(const DrawArgs &args, float pos) {
        nvgScissor(args.vg, scopeDisplay.pos.x, scopeDisplay.pos.y, scopeDisplay.size.x, scopeDisplay.size.y);

        pos = pos / 2.f + 0.5f;
        Vec p = Vec(scopeDisplay.size.x, scopeDisplay.pos.y + scopeDisplay.size.y * (1.f - pos));

        nvgStrokeColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x10));
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, p.x - 10, p.y);
        nvgLineTo(args.vg, 0, p.y);
        nvgClosePath(args.vg);
        nvgStroke(args.vg);

        nvgFillColor(args.vg, nvgRGBA(0xff, 0xff, 0xff, 0x60));
        nvgBeginPath(args.vg);
        nvgMoveTo(args.vg, p.x - 2, p.y - 3);
        nvgLineTo(args.vg, p.x - 7, p.y - 3);
        nvgLineTo(args.vg, p.x - 10, p.y);
        nvgLineTo(args.vg, p.x - 7, p.y + 3);
        nvgLineTo(args.vg, p.x - 2, p.y + 3);
        nvgClosePath(args.vg);
        nvgFill(args.vg);

        nvgResetScissor(args.vg);
    }

};
