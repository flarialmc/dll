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
        if (SDK::hasInstanced && SDK::clientInstance != nullptr) {
            if (SDK::clientInstance->getLocalPlayer() != nullptr && SDK::getCurrentScreen() == "hud_screen") {

                int entityCount = SDK::clientInstance->getLocalPlayer()->getLevel()->getRuntimeActorList().size();
                std::string str = std::format("{}", entityCount);

                this->module->normalRender(24, str);
            }
        }
    }

public:
    explicit EntityListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

