#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class SnapListener : public Listener {

    Module* module;

   
    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;


        if (!module->IsKeybind(event.keys)) module->settings.getSettingByName<bool>("enabled")->value = false;

    };

public:
    explicit SnapListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
