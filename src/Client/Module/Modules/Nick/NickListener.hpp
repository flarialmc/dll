#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class NickListener : public Listener {
    Module* module;
    bool enabled = false;
    std::string original;
public:

    void onLocalTick(TickEvent &event) override {

        if(original.empty()) original = SDK::clientInstance->getLocalPlayer()->playerName;

        if(enabled != module->settings.getSettingByName<bool>("enabled")->value) {
            enabled = module->settings.getSettingByName<bool>("enabled")->value;

            if(enabled) {

                std::string val = module->settings.getSettingByName<std::string>("nick")->value;
                SDK::clientInstance->getLocalPlayer()->setNametag(&val);
                SDK::clientInstance->getLocalPlayer()->playerName = val;

            } else {

                std::string val = original;
                SDK::clientInstance->getLocalPlayer()->setNametag(&val);
                SDK::clientInstance->getLocalPlayer()->playerName =  val;

            }
        }
    }

    explicit NickListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
