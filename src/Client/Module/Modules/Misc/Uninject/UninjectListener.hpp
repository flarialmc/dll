#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"
class UninjectListener : public Listener {

public:

    Module* module;
    std::string curKeybind = Client::settings.getSettingByName<std::string>("ejectKeybind")->value;

    void onKey(KeyEvent& event) override {
        if (curKeybind != Client::settings.getSettingByName<std::string>("ejectKeybind")->value) {
            curKeybind = Client::settings.getSettingByName<std::string>("ejectKeybind")->value;
            return;
        }
        if (event.GetKey() == Utils::GetStringAsKey(Client::settings.getSettingByName<std::string>("ejectKeybind")->value) && static_cast<ActionType>(event.GetAction()) == ActionType::RELEASED) {
            ModuleManager::terminate();
            Client::disable = true;
        }
    }

public:
    explicit UninjectListener(const char string[5]) {
        this->name = string;
    }
};
