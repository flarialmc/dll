#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class PingListener : public Listener {

    ;
    Module* module;
    int ping = 0;

    void onRender(RenderEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {
                if (SDK::raknetConnector != nullptr) {
                    if (SDK::raknetConnector->JoinedIp == "") {
                        ping = 0;
                    }

                    else {
                      //  ping = SDK::raknetConnector->getPeer()->getPing();

                    }

                    this->module->NormalRender(11, module->settings.getSettingByName<std::string>("text")->value, std::to_string(ping));
                }
                else {
                    this->module->NormalRender(11, module->settings.getSettingByName<std::string>("text")->value, std::to_string(1));
                }

            }

    }

public:
    explicit  PingListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

