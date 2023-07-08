#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class ReachListener : public Listener {


    float Reach = 0.0f;
    Module* module;

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (event.GetKey() == this->module->keybind) { module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value; }
    };


    void onAttack(AttackEvent& event) override {
        Vec3<float> *LPPos = SDK::clientInstance->getLocalPlayer()->getPosition();
        Vec3<float> *TargetPos = event.getActor()->getPosition();

        Reach = LPPos->dist(*TargetPos);
    }


    void onRender(RenderEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {

                std::ostringstream oss;
                oss << std::fixed << std::setprecision(2) << Reach;
                std::string reachString = oss.str();

                this->module->NormalRender(9, module->settings.getSettingByName<std::string>("text")->value, reachString);

            }

    }

public:
    explicit  ReachListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

