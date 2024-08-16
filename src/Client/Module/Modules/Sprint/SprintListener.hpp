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
        if(!module->isEnabled() || SDK::getCurrentScreen() != "hud_screen") return;

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                    std::string text = "Standing";
                    if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SNEAKING)) {
                        text = "Sneaking";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SWIMMING)) {
                        text = "Swimming";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_GLIDING)) {
                        text = "Gliding";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SLEEPING)) {
                        text = "Sleeping";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(ActorFlags::FLAG_SPRINTING)) {
                        text = "Sprinting";
                        this->module->normalRender(5, text);
                    } else if (SDK::clientInstance->getLocalPlayer()->getActorFlag(FLAG_MOVING)) {
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

