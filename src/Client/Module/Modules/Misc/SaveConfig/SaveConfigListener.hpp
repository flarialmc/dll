#pragma once

#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class SaveConfigListener : public Listener {

public:

    Module *module{};

    void onKey(KeyEvent &event) override {
        if (event.getKey() == VK_F7 && static_cast<ActionType>(event.getAction()) == ActionType::Released) {
            ModuleManager::SaveModulesConfig();
            Client::SaveSettings();
            //FlarialGUI::Notify("Config Saved");
        }
    }

public:
    explicit SaveConfigListener(const char string[5]) {
        this->name = string;
    }
};
