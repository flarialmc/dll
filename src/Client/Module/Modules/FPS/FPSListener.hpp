#pragma once
#include <format>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/KeyEvent.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include <Windows.h>

class FPSListener : public Listener {

    Module* module;

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind) module->enabled = !module->enabled;
    };

    void onRender(RenderEvent &event) override {

        if(module->enabled) {


            float textWidth = Constraints::RelativeConstraint(0.8);
            float textHeight = Constraints::RelativeConstraint(0.3);
            Vec2<float> center = Constraints::CenterConstraint(textWidth, textHeight);

            FlarialGUI::FlarialText(center.x, center.y, FlarialGUI::to_wide(std::format("FPS: {}", MC::fps)).c_str(), D2D1::ColorF(D2D1::ColorF::White), textWidth, textHeight);

        }

    }

public:
    explicit FPSListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

