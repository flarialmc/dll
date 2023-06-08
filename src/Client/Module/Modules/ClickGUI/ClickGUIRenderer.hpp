#pragma once
#include <format>
#include <Windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"
#include "../Module.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../GUI/Engine/Constraints.hpp"

class ClickGUIRenderer : public Listener {

    bool enabled = false;
    Module* module;

    void onRender(RenderEvent &event) override {


        if(enabled) {

            //FlarialGUI::RoundedButton(center.x, center.y);

            float baseWidth = Constraints::RelativeConstraint(0.78);
            float baseHeight = Constraints::RelativeConstraint(0.7);

            Vec2<float> center = Constraints::CenterConstraint(baseWidth, baseHeight, "r", 1, 1);
            Vec2<float> round = Constraints::RoundingConstraint(50, 50);

            FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(18.0f / 255.0f, 14.0f / 255.0f, 15.0f / 255.0f), baseWidth, baseHeight, round.x, round.x);
        }

    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind && event.GetAction() == ActionType::PRESSED) enabled = !enabled;
    }

public:
    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
    }
};
