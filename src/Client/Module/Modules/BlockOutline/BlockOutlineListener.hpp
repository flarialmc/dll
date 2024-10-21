#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "BlockOutline.hpp"
#include <Windows.h>
#include <chrono>

class BlockOutlineListener : public Listener {

    Module *module;
    MCCColor* selectionColor;

    void onColorChange() {
        // change our color
        D2D1_COLOR_F color;
        if (module->settings.getSettingByName<bool>("color_rgb")->value) color = FlarialGUI::rgbColor;
        else color = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        selectionColor->r = color.r;
        selectionColor->g = color.g;
        selectionColor->b = color.b;
        selectionColor->a = module->settings.getSettingByName<float>("colorOpacity")->value;
    }

    void onTick(TickEvent &event) override {
        if (module->settings.getSettingByName<bool>("color_rgb")->value)
            onColorChange();
    }

public:
    explicit BlockOutlineListener(const char string[5], Module *module, MCCColor* color) {
        this->name = string;
        this->module = module;
        this->selectionColor = color;
    }
};