#pragma once

#include <format>
#include <windows.h>
#include <unknwn.h>
#include "../../../../Client.hpp"

class GUIMouseListener : public Listener {
public:
    void onMouse(MouseEvent &event) {

        if (event.getMouseX() != 0) MC::mousePos.x = event.getMouseX();
        if (event.getMouseY() != 0) MC::mousePos.y = event.getMouseY();
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
