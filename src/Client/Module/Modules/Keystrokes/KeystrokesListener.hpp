#pragma once

#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../../SDK/SDK.hpp"
#include "../../../../Utils/Utils.hpp"
#include "Keystrokes.hpp"
#include <Windows.h>
#include <chrono>


class KeystrokesListener : public Listener {

    Module* module;

    [[nodiscard]] double Microtime() const {
        return (double(std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count()) / double(1000000));
    }

    void onRender(RenderEvent& event) override {

        if(SDK::CurrentScreen == "hud_screen")
        if (module->settings.getSettingByName<bool>("enabled")->value) {
            this->module->NormalRender(7, "", "");

        }
    }

public:

    explicit KeystrokesListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

