#pragma once

#include "../../../../Hook/Hooks/Input/MouseHook.hpp"
#include "../../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class rgbListener : public Listener {
public:
    void onTick(TickEvent &event) {
        if (FlarialGUI::rgbHue > 359 - Client::settings.getSettingByName<float>("rgb_speed")->value)
            FlarialGUI::rgbHue = 0;
        else FlarialGUI::rgbHue += Client::settings.getSettingByName<float>("rgb_speed")->value;
        FlarialGUI::rgbColor = FlarialGUI::HSVtoColorF(FlarialGUI::rgbHue, Client::settings.getSettingByName<float>(
                "rgb_saturation")->value, Client::settings.getSettingByName<float>("rgb_value")->value);
    }

    rgbListener() {
        Listen(this, TickEvent, &rgbListener::onTick);
    }

    ~rgbListener() {
        Deafen(this, TickEvent, &rgbListener::onTick);
    }
};
