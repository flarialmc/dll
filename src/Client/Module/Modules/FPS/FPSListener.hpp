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


            float textWidth = Constraints::RelativeConstraint(0.3);
            float textHeight = Constraints::RelativeConstraint(0.1);
            Vec2<float> round = Constraints::RoundingConstraint(40, 40);

            Vec2<float> rectcenter = Constraints::CenterConstraint(textWidth, textHeight);
            Vec2<float> center = = Constraints::CenterConstraint(textWidth, textHeight);

            FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), textWidth, textHeight, round.x, round.x);
            FlarialGUI::FlarialText(center.x, center.y, FlarialGUI::to_wide(std::format("FPS: {}", MC::fps)).c_str(), D2D1::ColorF(32.0f/255.0f, 26.0f/255.0f, 27.0f/255.0f), textWidth, textHeight);

        }

    }

public:
    explicit FPSListener(const char string[5], Module* module) {
        this->name = string;
        this->module = module;
    }

};

