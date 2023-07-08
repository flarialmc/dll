#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class ComboListener : public Listener {


    int Combo = 0;
    Module* module;


    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (event.GetKey() == this->module->keybind) { module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value; }
    };


    void onAttack(AttackEvent& event) override {
        Combo++;
    }

    void onLocalTick(TickEvent& event) override {
         if (event.getActor()->mobHurtTimeComponent->HurtTime >= 1) {
               Combo = 0;
         }
    }

    void onRender(RenderEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {

                this->module->NormalRender(0, module->settings.getSettingByName<std::string>("text")->value, std::to_string(Combo));

            }

    }

public:
    explicit  ComboListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

