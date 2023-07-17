#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include "../../../../SDK/SDK.hpp"
#include <Windows.h>

class ZoomListener : public Listener {

    Module* module;

    void onMouse(MouseEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen")
            if (this->module->settings.getSettingByName<bool>("enabled")->value) {

                if (event.GetAction() == MouseAction::SCROLL_UP) zoomValue -= this->module->settings.getSettingByName<float>("modifier")->value;
                if (event.GetAction() != MouseAction::SCROLL_UP && event.GetButton() == MouseButton::Scroll)
                    zoomValue += this->module->settings.getSettingByName<float>("modifier")->value;


                if (zoomValue < 1) zoomValue = 1;
                else if (zoomValue > 90.0f) zoomValue = 90.0f;

                if (event.GetAction() == MouseAction::SCROLL_UP || event.GetAction() != MouseAction::SCROLL_UP && event.GetButton() == MouseButton::Scroll) {
                    event.SetButton(MouseButton::None);
                    event.SetAction(MouseAction::RELEASE);

                }
            }

    }

    void onKey(KeyEvent& event) override {

        if (SDK::CurrentScreen == "hud_screen" and !ModuleManager::getModule("ClickGUI")->settings.getSettingByName<bool>("enabled")->value)
            if (module->IsKeybind(event.keys) && module->IsKeyPartOfKeybind(event.key)) {
                module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;
                if (!module->settings.getSettingByName<bool>("SaveModifier")->value) zoomValue = 30.0f;
            }

        if (!module->IsKeybind(event.keys)) module->settings.getSettingByName<bool>("enabled")->value = false;

    };

public:
    static float inline zoomValue = 30.0f;
    explicit ZoomListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
