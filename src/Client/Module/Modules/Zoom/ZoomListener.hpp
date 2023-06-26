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


        if(event.GetAction() == MouseAction::SCROLL_UP) zoomValue += 0.05;
        if(event.GetAction() != MouseAction::SCROLL_UP && event.GetButton() == MouseButton::Scroll) zoomValue -= 0.05;
        
    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind) module->settings.getSettingByName<bool>("enabled")->value = !module->settings.getSettingByName<bool>("enabled")->value;

    };

public:
    static float inline zoomValue = 0.5f;
    explicit ZoomListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }
};
