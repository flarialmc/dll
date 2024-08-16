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

    void onRender(RenderEvent &event) override {
        if (
                SDK::hasInstanced && SDK::clientInstance != nullptr &&
                SDK::clientInstance->getLocalPlayer() != nullptr &&
                SDK::clientInstance->getLocalPlayer()->getgamemode() != nullptr
                ) {

            if(SDK::currentScreen != "hud_screen") return;

            auto progress_value = (*SDK::clientInstance->getLocalPlayer()->getgamemode()).lastBreakProgress *100;
            std::string progress = std::format("{:.0f}%", progress_value);
            this->module->normalRender(16, progress);
        }
    }


public:
    explicit BlockBreakIndicatorListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

