#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include "../../../../SDK/Client/Actor/Gamemode.hpp"
#include "../../Manager.hpp"

class SprintListener : public Listener {

    Module* module;
    bool ToggleSprinting = false;

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {
                if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) {
                    ToggleSprinting = !ToggleSprinting;
                }
            }
    };

    void onRender(RenderEvent& event) override {
        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {
                if (module->settings.getSettingByName<bool>("status")->value) {
                    if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                        if (SDK::clientInstance->getLocalPlayer() != nullptr) {

                            if (SDK::clientInstance->getLocalPlayer()->getActorFlag(1)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Sneaking");
                            }
                            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(57)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Swimming");
                            }
                            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(32)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Gliding");
                            }
                            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(76)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Sleeping");
                            }
                            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(3)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Sprinting");
                            }
                            else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(34)) {
                                this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Walking");
                            }
                            else { this->module->NormalRender(5, module->settings.getSettingByName<std::string>("text")->value, "Standing"); }

                        }

                    }
                }
            }
    }

    void onLocalTick(TickEvent& event) override {
        if (module->settings.getSettingByName<bool>("enabled")->value) {
            if (SDK::clientInstance != nullptr) {
                if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                    MoveInputComponent* handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

                    if (handler->forward) {
                        if (module->settings.getSettingByName<bool>("always")->value) {
                            handler->sprinting = true;
                        }
                        else {
                            if(ToggleSprinting) handler->sprinting = ToggleSprinting;
                        }
                    }
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

