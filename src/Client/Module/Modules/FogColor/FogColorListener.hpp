#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class FogColorListener : public Listener {

    Module *module;

    void onGetFogColor(FogColorEvent &event) override {

        /*
        D2D1_COLOR_F color;
        if (module->settings.getSettingByName<bool>("color_rgb")->value)
            color = FlarialGUI::rgbColor;
        else
            color = FlarialGUI::HexToColorF(module->settings.getSettingByName<std::string>("color")->value);

        event.setFogColorFromD2DColor(color, module->settings.getSettingByName<float>("colorOpacity")->value);
        */
    }

public:
    explicit FogColorListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};
