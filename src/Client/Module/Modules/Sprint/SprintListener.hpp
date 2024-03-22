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

    Module *module;
    bool toggleSprinting = false;

    void onKey(KeyEvent &event) override {
            if (module->isKeybind(event.keys) && module->isKeyPartOfKeybind(event.key)) {
                toggleSprinting = !toggleSprinting;
            }
    };

    void onRender(RenderEvent &event) override {
        if(!module->isEnabled() || SDK::currentScreen != "hud_screen") return;

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                    std::string text = "Standing";
                    if (SDK::clientInstance->getLocalPlayer()->getActorFlag(1)) {
                        text = "Sneaking";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(57)) {
                        text = "Swimming";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(32)) {
                        text = "Gliding";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(76)) {
                        text = "Sleeping";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(3)) {
                        text = "Sprinting";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(34)) {
                        text = "Walking";
                        this->module->normalRender(5, text);
                    } else {
                        this->module->normalRender(5, text);
                    }

                }
        }
    }

    void onTick(TickEvent &event) override {
        if (SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                MoveInputComponent *handler = SDK::clientInstance->getLocalPlayer()->getMoveInputHandler();

                if (handler->forward) {
                    if (module->settings.getSettingByName<bool>("always")->value) {
                        handler->sprinting = true;
                    } else {
                        if (toggleSprinting) handler->sprinting = toggleSprinting;
                    }
                }
            }
        }
    }

public:
    explicit SprintListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

