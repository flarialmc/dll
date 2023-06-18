#pragma once
#include <format>
#include <Windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"

class GUIMouseListener : public Listener {

    void onMouse(MouseEvent &event) override {

       MC::mousepos.x = event.mouseX;
       MC::mousepos.y = event.mouseY;
       MC::mousebutton = event.GetButton();
       MC::mouseaction = event.GetAction();

       if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::PRESS) MC::held = true;
        if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::RELEASE) MC::held = false;

    };

public:
    explicit GUIMouseListener(const char string[5]) {
        this->name = string;
    }
};
