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
    Module *module;

    void onAttack(AttackEvent &event) override {
        if(std::chrono::high_resolution_clock::now() - last_hit > std::chrono::milliseconds(480)) {
            Combo++;
            last_hit = std::chrono::high_resolution_clock::now();
        }
    }

    void onTick(TickEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer())
            return;

        auto LP = reinterpret_cast<LocalPlayer *>(event.getActor());
        if (LP->getHurtTime() != 0)
            Combo = 0;
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) Combo = 0;
    }

    void onRender(RenderEvent &event) override {
        if (module->isEnabled()) {
            auto comboStr = std::to_string(Combo);
            this->module->normalRender(8, comboStr);
        }
    }

public:
    explicit ComboListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

