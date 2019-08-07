#pragma once


#include <functional>


class TextDisplay : public TransformWidget {

public:

    std::shared_ptr<Font> font;
    int size;
    int * colorHandle;
    NVGcolor textColor;

    TextDisplay(Vec pos, std::function<std::string()> getValue) {
        font = APP->window->loadFont(asset::plugin(pluginInstance, "res/fonts/ShareTechMono-Regular.ttf"));
        size = 14;
        box.pos = pos;
        _getValue = getValue;
    }

    void draw(const DrawArgs &args) override {
        nvgFontSize(args.vg, size);
        nvgFontFaceId(args.vg, font->handle);
        nvgTextLetterSpacing(args.vg, 0.f);
        nvgFillColor(args.vg, nvgRGB(0xff, 0xff, 0xff));
        nvgTextAlign(args.vg, NVG_ALIGN_CENTER | NVG_ALIGN_TOP);
        nvgText(args.vg, 0.f, 0.f, _value.c_str(), NULL);
    }

    void step() override {
        _value = _getValue();
    }

private:

    std::function<std::string()> _getValue;
    std::string _value;

};
