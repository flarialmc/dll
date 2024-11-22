#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class rgbListener : public Listener {

public:

    void onTick(TickEvent &event) override {
        if (FlarialGUI::rgbHue > 359 - Client::settings.getSettingByName<float>("rgb_speed")->value)
            FlarialGUI::rgbHue = 0;
        else FlarialGUI::rgbHue += Client::settings.getSettingByName<float>("rgb_speed")->value;
        FlarialGUI::rgbColor = FlarialGUI::HSVtoColorF(FlarialGUI::rgbHue, Client::settings.getSettingByName<float>(
                "rgb_saturation")->value, Client::settings.getSettingByName<float>("rgb_value")->value);
    }

public:
    explicit rgbListener(const char string[5]) {
        this->name = string;
    }
};
