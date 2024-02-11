#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Hook/Hooks/Game/RaknetTick.hpp"
#include <Windows.h>

class  IPDisplayListener : public Listener {

    Module* module;
    std::string IP = "";
    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {

                    if (SDK::clientInstance->getRakNetConnector() != nullptr) {
                         IP = SDK::clientInstance->getRakNetConnector()->JoinedIp;

                         if (IP == "") {
                             IP = "world";
                         }
                             this->module->NormalRender(10, module->settings.getSettingByName<std::string>("text")->value, IP);
                    }

                    else {
                        this->module->NormalRender(10, module->settings.getSettingByName<std::string>("text")->value, "world");
                    }
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

