#pragma once

#include <format>
#include <Windows.h>
#include "../../../../Client.hpp"

class GUIMouseListener : public Listener {
public:
    void onMouse(MouseEvent &event) {

        MC::mousePos.x = event.getMouseX();
        MC::mousePos.y = event.getMouseY();
        MC::mouseButton = event.getButton();
        MC::mouseAction = event.getAction();

        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Press) MC::held = true;
        if (event.getButton() != MouseButton::None && event.getAction() == MouseAction::Release) MC::held = false;

    };

    GUIMouseListener() {
        Listen(this, MouseEvent, &GUIMouseListener::onMouse);
    }

    ~GUIMouseListener() {
        Deafen(this, MouseEvent, &GUIMouseListener::onMouse);
    }
};
