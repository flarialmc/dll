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
                    if (SDK::clientInstance->getLocalPlayer()->getActorFlag(34) and !SDK::clientInstance->getLocalPlayer()->getActorFlag(3)) {
                        if (module->settings.getSettingByName<bool>("always")->value) {
                            SDK::clientInstance->getLocalPlayer()->setSprinting(true);
                        }
                        else if (module->settings.getSettingByName<bool>("toggled")->value) {
                            SDK::clientInstance->getLocalPlayer()->setSprinting(module->settings.getSettingByName<bool>("toggled")->value);
                        }
                    }
                    if (SDK::clientInstance->getLocalPlayer()->getActorFlag(1)) { this->module->NormalRender(5, "", "Sneaking"); }
                    else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(3)) { this->module->NormalRender(5, "", "Sprinting"); }
                    else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(34)) { this->module->NormalRender(5, "", "Walking"); }
                    else { this->module->NormalRender(6, "", "Standing"); }
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

