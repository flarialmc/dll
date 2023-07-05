#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../Module.hpp"
#include <Windows.h>

class ZoomListener : public Listener {

    Module* module;

    void onMouse(MouseEvent &event) override {


        if(this->module->settings.getSettingByName<bool>("enabled")->value){

            if (event.GetAction() == MouseAction::SCROLL_UP) zoomValue -= 10;
            if (event.GetAction() != MouseAction::SCROLL_UP && event.GetButton() == MouseButton::Scroll)
                zoomValue += 10;

            if(zoomValue < 1) zoomValue = 1;
            else if (zoomValue > 90.0f) zoomValue = 90.0f;

            if(event.GetAction() == MouseAction::SCROLL_UP || event.GetAction() != MouseAction::SCROLL_UP && event.GetButton() == MouseButton::Scroll)
            event.SetButton(MouseButton::None);
        }
        
    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind) module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;

    };

public:
    static float inline zoomValue = 30.0f;
    explicit ZoomListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
