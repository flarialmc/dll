#pragma once


#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"

#include "../../../../SDK/SDK.hpp"
#include <Windows.h>
#include <chrono>

class ReachListener : public Listener {


    float Reach = 0.0f;
    std::chrono::time_point<std::chrono::high_resolution_clock> last_hit;
    Module *module;
    // TODO: Better reach calculation
    void onAttack(AttackEvent &event) override {
        Reach = event.getActor()->getlevel()->getHitResult().distance();
        last_hit = std::chrono::high_resolution_clock::now();
    }

    void onTick(TickEvent &event) override {
        std::chrono::duration<double> duration = std::chrono::high_resolution_clock::now() - last_hit;
        if (duration.count() >= 15) Reach = 0.0f;

    }

    void onRender(RenderEvent &event) override {

        if (module->isEnabled()) {

            std::ostringstream oss;
            oss << std::fixed << std::setprecision(2) << Reach;
            std::string reachString = oss.str();

            this->module->normalRender(9, reachString);

        }

    }

public:
    explicit ReachListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }

};

