#pragma once
#include <format>
#include <Windows.h>
#include "../../../Events/Listener.hpp"
#include "../../../Events/Input/MouseEvent.hpp"
#include "../../../../Utils/Logger/Logger.hpp"
#include "../../../GUI/D2D.hpp"
#include "../../../GUI/Engine/Engine.hpp"

class GUIMouseListener : public Listener {

    void onMouse(MouseEvent &event) override {

       MC::mousepos.x = event.mouseX;
       MC::mousepos.y = event.mouseY;
       MC::mousebutton = event.GetButton();
       MC::mouseaction = event.GetAction();

       if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::PRESS) MC::held = true;
        if(event.GetButton() != MouseButton::None && event.GetAction() == MouseAction::RELEASE) MC::held = false;

        if(event.GetButton() == MouseButton::Scroll)
        {
            FlarialGUI::scrollpos += (event.GetAction() == MouseAction::SCROLL_UP) ? FlarialGUI::scrollposmodifier : -FlarialGUI::scrollposmodifier;
            FlarialGUI::barscrollpos += (event.GetAction() == MouseAction::SCROLL_UP) ? FlarialGUI::barscrollposmodifier : -FlarialGUI::barscrollposmodifier;
        }

    };

public:
    explicit GUIMouseListener(const char string[5]) {
        this->name = string;
    }
};
