#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class SprintListener : public Listener {

    Module* module;

    void onKey(KeyEvent& event) override {

        if (module->settings.getSettingByName<bool>("enabled")->value) {
            if (event.GetKey() == 'N' && event.GetAction() == 0) {
                module->settings.getSettingByName<bool>("toggled")->value = !module->settings.getSettingByName<bool>("toggled")->value;
            }
        }

        if (event.GetKey() == this->module->keybind) { module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value; }
    };

    void onRender(RenderEvent& event) override {

        if (module->settings.getSettingByName<bool>("enabled")->value) {

            if (SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {


                }

            }
        }
    }

    void onLocalTick(TickEvent &event) override {

        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                MoveInputComponent* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

                if (module->settings.getSettingByName<bool>("always")->value) {
                    handler->sprinting = true;
                }  else {
                    handler->sprinting = module->settings.getSettingByName<bool>("toggled")->value;
                }
            }
        }
    }

public:
    explicit SprintListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

