#pragma once

#include "../../../../SDK/SDK.hpp"
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class TabListListener : public Listener {

    Module* module;

    void onRender(RenderEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->settings.getSettingByName<bool>("enabled")->value) {

                this->module->NormalRender(20, "", "");

            }
    }

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) {
            module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;
        }

        if (!module->IsKeybind(event.keys)) module->settings.getSettingByName<bool>("enabled")->value = false;

    };

public:
    explicit TabListListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};