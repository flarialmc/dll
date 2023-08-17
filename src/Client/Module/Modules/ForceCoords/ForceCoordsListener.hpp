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

                    Vec3<float> Pos = SDK::clientInstance->getLocalPlayer()->stateVector->Pos;
                    //Vec3<float> PrevPos = SDK::clientInstance->getLocalPlayer()->stateVector->PrevPos;
                    //Vec3<float> vel = SDK::clientInstance->getLocalPlayer()->stateVector->velocity;

                    std::string cords = std::to_string(static_cast<int>(Pos.x)) + ", " + std::to_string(static_cast<int>(Pos.y)) + ", " + std::to_string(static_cast<int>(Pos.z)); 
                    //std::string cords1 = std::to_string(static_cast<int>(PrevPos.x)) + ", " + std::to_string(static_cast<int>(PrevPos.y)) + ", " + std::to_string(static_cast<int>(PrevPos.z));
                    //std::string cords2 = std::to_string(static_cast<int>(vel.x)) + ", " + std::to_string(static_cast<int>(vel.y)) + ", " + std::to_string(static_cast<int>(vel.z));
                    this->module->NormalRender(6, module->settings.getSettingByName<std::string>("text")->value, cords);


                }

            }
        }
    }
    Vec3<float> PrevPoss;
    void onLocalTick(TickEvent& event) override {
        Vec3<float> Pos = SDK::clientInstance->getLocalPlayer()->stateVector->Pos;
        float* timer = SDK::clientInstance->getMinecraft()->timer;

        
        std::cout << std::to_string(Pos.dist(PrevPoss) * 20) << std::endl;
        PrevPoss = Pos;
    }

public:
    explicit ForceCoordsListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

