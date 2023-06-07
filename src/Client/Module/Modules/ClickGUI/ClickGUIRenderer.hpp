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
            Logger::info("E");
            Vec2<float> center = Constraints::CenterConstraint(50, 50, "x", 1, 1);

            FlarialGUI::RoundedRect(center.x, center.y, D2D1::ColorF(D2D1::ColorF::Black));
            Logger::info("Sports");
        }

    }

    void onKey(KeyEvent &event) override {

        if(event.GetKey() == module->keybind && event.GetAction() == ActionType::RELEASED) enabled = !enabled;
    }

public:
    explicit ClickGUIRenderer(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
    }
};
