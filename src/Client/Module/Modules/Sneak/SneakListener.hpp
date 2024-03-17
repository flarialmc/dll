#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class SneakListener : public Listener {

    Module *module;
    bool toggleSneaking = false;
    bool toggled = false;

    void onKey(KeyEvent &event) override { // TODO: it lets sneak key up through (flickers sneak once)
        if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
            toggleSneaking = !toggleSneaking;
        }
    };

    void onTick(TickEvent &event) override {
        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                MoveInputComponent *handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
                if (toggleSneaking) {
                    handler->sneaking = true;
                    toggled = true;
                }
                if (!toggleSneaking and toggled) {
                    handler->sneaking = false;
                    toggled = false;
                }
            }
        }
    }

public:
    explicit SneakListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

