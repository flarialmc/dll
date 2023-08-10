#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <chrono>

class ReachListener : public Listener {


    float Reach = 0.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    Module* module;

    void onAttack(AttackEvent& event) override {
        Vec3<float> *LPPos = SDK::clientInstance->getLocalPlayer()->getPosition();
        Vec3<float> *TargetPos = event.getActor()->getPosition();

        Reach = LPPos->dist(*TargetPos);
        if(Reach > 3.0f) Reach = 3.0f;
        last_hit = std::chrono::high_resolution_clock::now();
    }

    void onLocalTick(TickEvent& event) override {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) Reach = 0.0f;

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

