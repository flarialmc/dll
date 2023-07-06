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

class ForceCoordsListener : public Listener {

    Module* module;

    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {

            if (SDK::hasInstanced && SDK::clientInstance != nullptr) {

                if (SDK::clientInstance->getLocalPlayer() != nullptr) {

                    Vec3<float>* Pos = SDK::clientInstance->getLocalPlayer()->getPosition();

                    std::string cords = std::to_string(static_cast<int>(Pos->x)) + ", " + std::to_string(static_cast<int>(Pos->y)) + ", " + std::to_string(static_cast<int>(Pos->z)); 
                    this->module->NormalRender(6, module->settings.getSettingByName<std::string>("text")->value, cords);

                }

            }
        }
    }

public:
    explicit ForceCoordsListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

