#pragma once


struct LightGreen : GrayModuleLightWidget {
    LightGreen() {
        addBaseColor(COLOR_GREEN_1);
    }
};

struct LightBlue : GrayModuleLightWidget {
    LightBlue() {
        addBaseColor(COLOR_BLUE_1);
    }
};

struct LightGreenRed : GrayModuleLightWidget {
    LightGreenRed() {
        addBaseColor(COLOR_GREEN_1);
        addBaseColor(COLOR_RED_1);
    }
};

struct LightBlueGreenRed : GrayModuleLightWidget {
    LightBlueGreenRed() {
        addBaseColor(COLOR_BLUE_1);
        addBaseColor(COLOR_GREEN_1);
        addBaseColor(COLOR_RED_1);
    }
};

