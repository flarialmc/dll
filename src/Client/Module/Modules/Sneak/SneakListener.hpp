#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class SneakListener : public Listener {

    Module* module;
    bool ToggleSneaking = false;
    bool toggled = false;

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {
                if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) {
                    ToggleSneaking = !ToggleSneaking;
                }
            }
    };

    void onLocalTick(TickEvent& event) override {

        if (module->settings.getSettingByName<bool>("enabled")->value) {
            if (SDK::clientInstance != nullptr) {
                if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                    MoveInputComponent* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();
                    if (ToggleSneaking) {
                        handler->sneaking = true;
                        toggled = true;
                    }
                    if (!ToggleSneaking and toggled) {
                        handler->sneaking = false;
                        toggled = false;
                    }
                }
            }
        }
    }

public:
    explicit SneakListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

