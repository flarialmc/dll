#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <Windows.h>
#include <string>

class BlockBreakIndicatorListener : public Listener {

    Module *module;

    float lastProgress = 0.0f;
    float currentProgress = 0.0f;

    void onRender(RenderEvent &event) override {
        if (
                SDK::hasInstanced && SDK::clientInstance != nullptr &&
                SDK::clientInstance->getLocalPlayer() != nullptr &&
                SDK::clientInstance->getLocalPlayer()->getGamemode() != nullptr
                ) {

            if (CPSListener::GetLeftHeld()) {
                auto progress= (*SDK::clientInstance->getLocalPlayer()->getGamemode()).getLastBreakProgress() * 100;
                if(lastProgress != progress) {
                    if(lastProgress < progress || progress == 0.f) {
                        currentProgress = progress;
                    }
                    lastProgress = progress;
                }
            } else {
                currentProgress = 0.0f;
            }

            std::string progress = std::format("{:.0f}%", currentProgress);
            this->module->normalRender(16, progress);
        }
    }


public:
    explicit BlockBreakIndicatorListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

