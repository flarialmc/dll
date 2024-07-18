#pragma once

#include <format>
#include <Windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/Data.hpp"

class GUIMouseListener : public Listener {

    void onMouse(MouseEvent &event) override {

        MC::mousePos.x = event.getMouseX();
        MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;

    };

public:
    explicit GUIMouseListener(const char string[5]) {
        this->name = string;
    }
};
