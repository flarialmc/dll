#pragma once

#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"
#include "Events/EventManager.hpp"
#include "Events/Events.hpp"

class UninjectListener : public Listener {
private:
    std::string curKeybind = Client::settings.getSettingByName<std::string>("ejectKeybind")->value;
public:
    void onKey(KeyEvent &event) {
        if (curKeybind != Client::settings.getSettingByName<std::string>("ejectKeybind")->value) {
            curKeybind = Client::settings.getSettingByName<std::string>("ejectKeybind")->value;
            return;
        }
        if (event.getKey() ==
            Utils::getStringAsKey(Client::settings.getSettingByName<std::string>("ejectKeybind")->value) &&
            static_cast<ActionType>(event.getAction()) == ActionType::Released) {
            //SwapchainHook::recreate = true;
            Client::disable = true;
        }
    }

    UninjectListener() {
        Listen(this, KeyEvent, &UninjectListener::onKey);
    }
    ~UninjectListener() {
        Deafen(this, KeyEvent, &UninjectListener::onKey);
    }
};
