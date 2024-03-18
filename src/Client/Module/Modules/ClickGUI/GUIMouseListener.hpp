#pragma once

#include <format>
#include <windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../GUI/Engine/Engine.hpp"
#include "../../../Module/Manager.hpp"
#include "../CPS/CPSCounter.hpp"

class GUIMouseListener : public Listener {

    Module *module;

    void onMouse(MouseEvent &event) override {

        MC::mousePos.x = event.getMouseX();
        MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;

        if (event.getButton() == MouseButton::Scroll) {
            accumilatedPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::scrollposmodifier
                                                                           : -FlarialGUI::scrollposmodifier;
            accumilatedBarPos += (event.getAction() == MouseAction::ScrollUp) ? FlarialGUI::barscrollposmodifier
                                                                              : -FlarialGUI::barscrollposmodifier;
        }

        if (module->active) event.cancel();

    };

public:

    static float inline accumilatedPos = 1;
    static float inline accumilatedBarPos = 1;

    explicit GUIMouseListener(const char string[5], Module *emodule) {
        this->name = string;
        this->module = emodule;
    }
};
