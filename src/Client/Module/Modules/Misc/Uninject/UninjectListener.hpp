#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class UninjectListener : public Listener {

public:

    Module* module;

    void onKey(KeyEvent& event) override {

        if (event.GetKey() == VK_F8 && static_cast<ActionType>(event.GetAction()) == ActionType::RELEASED) {
            ModuleManager::terminate();
            Client::disable = true;
        }
    }

public:
    explicit UninjectListener(const char string[5]) {
        this->name = string;
    }
};
