#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"

class EntityListener : public Listener {
public:

    Module *module;

    void onRender(RenderEvent &event) override {

        int entities = 0;

        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr && SDK::getCurrentScreen() == "hud_screen") {

                for(auto& entity : SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList()) {
                    entities++;
                }
            }
        }

        std::string value = std::format("{}", entities);
        this->module->normalRender(24, value);
    }

public:
    explicit EntityListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

