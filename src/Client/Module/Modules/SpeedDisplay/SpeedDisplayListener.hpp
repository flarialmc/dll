#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "../../../Client.hpp"
#include <Windows.h>
#include <thread>

class SpeedDisplayListener : public Listener {

    Module *module;

    Vec3<float> PrevPos;
    std::string speed;

    void onRender(RenderEvent &event) override {
        this->module->normalRender(15, speed);
    }

    void onTick(TickEvent &event) override {
        if (!SDK::clientInstance->getLocalPlayer())
            return;
        auto stateVectorComponent = SDK::clientInstance->getLocalPlayer()->getStateVectorComponent();
        if(stateVectorComponent != nullptr) {
            speed = std::format("{:.2f}", stateVectorComponent->Pos.dist(PrevPos) * 20);
            PrevPos = stateVectorComponent->Pos;
        }
    }

public:
    explicit SpeedDisplayListener(const char string[5], Module *module) {
        this->name = string;
        this->module = module;
    }
};

