#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class HurtColorListener : public Listener {

    Module *module;

    void onGetHurtColor(HurtColorEvent &event) override {
        D2D1_COLOR_F color;
        if (module->settings.getSettingByName<bool>("color_rgb")->value)
            color = FlarialGUI::rgbColor;
        else
            color = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        event.setHurtColorFromD2DColor(color, module->settings.getSettingByName<float>("colorOpacity")->value);
    }

public:
    explicit HurtColorListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
