#pragma once
#include "../../../../Events/Listener.hpp"
#include "../../../../Client.hpp"

class UninjectListener : public Listener {

public:

    Module* module;

    void onKey(KeyEvent& event) override {

        if (event.GetKey() == 'P' && event.GetAction() == ActionType::RELEASED) {
            ModuleManager::terminate();
            Client::disable = true;
        }
    }

public:
    explicit UninjectListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
