#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class LookListener : public Listener {

    Module* module;
private:
    Vec2<float> oldRotations;
public:

    void onLocalTick(TickEvent& event) override {
        if (module->settings.getSettingByName<bool>("enabled")->value) {
            event.getActor()->getMovementProxyComponent()->movementProxy->SetRotation(&oldRotations);
      //      event.getActor()->getMovementProxyComponent()->movementProxy->SetYHeadRotation(oldRotations.y);
        }
        else {
            oldRotations = event.getActor()->actorRotationComponent->Rotation;
        }
    }

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;


        if (!module->IsKeybind(event.keys)) module->settings.getSettingByName<bool>("enabled")->value = false;

    };

public:
    explicit LookListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
