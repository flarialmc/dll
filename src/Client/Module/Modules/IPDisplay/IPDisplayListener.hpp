#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include <Windows.h>

class  IPDisplayListener : public Listener {

    Module* module;

    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {
                    
                    auto IP = SDK::raknetConnector->JoinedIp;

                    this->module->NormalRender(10, module->settings.getSettingByName<std::string>("text")->value, IP);

                }

            }
        }
    }

public:
    explicit  IPDisplayListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

