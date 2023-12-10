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

class ComboListener : public Listener {


    int Combo = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    Module* module;

    void onAttack(AttackEvent& event) override {
        Combo++;
        last_hit = std::chrono::high_resolution_clock::now();
    }

    void onLocalTick(TickEvent& event) override {
        if (!SDK::clientInstance->getLocalPlayer())
            return;

        auto LP = reinterpret_cast<LocalPlayer*>(event.getActor());
         if (LP->hurtTime != 0)
             Combo = 0;
         std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
         if (duration.count() >= 15) Combo = 0;
    }

    void onRender(RenderEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {

                this->module->NormalRender(8, module->settings.getSettingByName<std::string>("text")->value, std::to_string(Combo));

            }

    }

public:
    explicit  ComboListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

